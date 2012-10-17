/* ******************************************************************************
*  Real Flow Geometry Import SOP Houdini Extension
*
* $RCSfile: SOP_RF_Import.C,v $
*
* Description : This plugin will import Real Flow data into Houdini
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import.C,v $
*
* $Author: mstory $
*
*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story
*
* See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2004
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */

#include <climits>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <OP/OP_Director.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Math.h>
#include <UT/UT_Interrupt.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <GD/GD_PrimList.h>
#include <GD/GD_Vertex.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Node.h>
#include <OP/OP_Network.h>
#include <OP/OP_Director.h>
#include <OP/OP_NodeFlags.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <GA/GA_AttributeRef.h>
#include <GEO/GEO_AttributeHandle.h>

using namespace std;

#include "SOP_RF_Import.h"
#include "SOP_RF_Import_ReadRFParticleFile.C"
#include "SOP_RF_Import_ReadRFMeshFile.C"
#include "SOP_RF_Import_ReadRFSDFile.C"
#include "SOP_RF_Import_ReadRFRWCFile.C"



/* ******************************************************************************
*  Function Name : SOP_RF_Import_Exception()
*
*  Description : Constructor for an "Real Flow Import Operator Exception" object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_RF_Import_Exception::SOP_RF_Import_Exception(enumErrorList code, enumExceptionSeverity sev)
{

//   std::cout << "SOP_RF_Import_Exception: in constructor ... " << endl;

    this->e_msg = errorMsgs[code].toStdString();
    this->e_code = code;
    this->severity = sev;

};



//SOP_RF_Import_Exception::~SOP_RF_Import_Exception() {

//   std::cout << "SOP_RF_Import_Exception: in destructor ... " << endl;

//   };




/* ******************************************************************************
*  Function Name : OP_RF_Import_Operator()
*
*  Description : Constructor for an "Real Flow Import Operator" object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_RF_Import_Operator::OP_RF_Import_Operator()
    : OP_Operator("rf_import",
                  "Real Flow Import",
                  SOP_RF_Import::myConstructor,
                  SOP_RF_Import::myTemplateList,
                  0,
                  0,
                  SOP_RF_Import::myVariables,
                  OP_FLAG_GENERATOR)
{
}



/* ******************************************************************************
*  Function Name : ~OP_RF_Import_Operator()
*
*  Description : Destructor for an "Real Flow Import Operator"  object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_RF_Import_Operator::~OP_RF_Import_Operator()
{
}


/* ******************************************************************************
*  Function Name : newSopOperator()
*
*  Description :
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
void newSopOperator(OP_OperatorTable * table)
{
    table->addOperator(new OP_RF_Import_Operator());
}


/* ******************************************************************************
*  Data Area
*
*  Description : Various structures for the SOP, primarily for the interface
*
***************************************************************************** */

#define NPARMS_FOLDER_1 6
#define NPARMS_FOLDER_2 15
#define NPARMS_FOLDER_3 2
#define NPARMS_FOLDER_4 4
#define NPARMS_FOLDER_5 1

static PRM_Default  switcherDef[] = {
    PRM_Default(NPARMS_FOLDER_1, "Setup"),
    PRM_Default(NPARMS_FOLDER_2, "Particle"),
    PRM_Default(NPARMS_FOLDER_3, "Mesh"),
    PRM_Default(NPARMS_FOLDER_4, "SD"),
    PRM_Default(NPARMS_FOLDER_5, "RWC"),
};

static PRM_Name     names[] = {
    PRM_Name("fname",   "File Name"),
    PRM_Name("type",    "File Type"),
    PRM_Name("version", ""),
    PRM_Name("info1", ""),
    PRM_Name("info2", ""),
    PRM_Name("info3", ""),

    // Particle attributes page
    PRM_Name("vel",              "Velocity"),
    PRM_Name("force",            "Force"),
    PRM_Name("vorticity",        "Vorticity"),
    PRM_Name("normal",           "Normal"),
    PRM_Name("num_neighbors",    "Number of Neighbors"),
    PRM_Name("texture_vector",   "Texture Vector"),
    PRM_Name("info_bits",        "Information Bits"),
    PRM_Name("age",              "Age"),
    PRM_Name("isolation_time",   "Isolation Time"),
    PRM_Name("viscosity",        "Viscosity"),
    PRM_Name("density",          "Density"),
    PRM_Name("pressure",         "Pressure"),
    PRM_Name("mass",             "Mass"),
    PRM_Name("temperature",      "Temperature"),
    PRM_Name("id",               "Particle ID"),

    // Mesh attributes page
    PRM_Name("m_tex",            "Texture"),
    PRM_Name("m_vel",            "Velocity"),

    // SD attributes page
    PRM_Name("sd_tex",           "Texture (U, V, W)"),
    PRM_Name("sd_CG",            "CG (Position, Velocity, Angular Rotation)"),
    PRM_Name("sd_obj_xform",     "Apply Object Transformations"),
    PRM_Name("sd_CG_xform",      "Apply CG Transformations"),

    // RWC attributes page
    PRM_Name("rwc_vel",           "Velocity"),

    PRM_Name(0)
};

static PRM_Name theImportTypeMenu[] = {
    PRM_Name("0", "Real Flow Particles"),
    PRM_Name("1", "Real Flow Mesh"),
    PRM_Name("2", "Real Flow SD"),
    PRM_Name("3", "Real Wave Cache"),
    PRM_Name(0),
};


static PRM_ChoiceList importTypeMenu((PRM_ChoiceListType)(PRM_CHOICELIST_EXCLUSIVE | PRM_CHOICELIST_REPLACE), theImportTypeMenu);

// Defaults
static PRM_Default nameDefault1(0,"untitled.bin");


// Built the parameter template
PRM_Template SOP_RF_Import::myTemplateList[] = {
    //GUI groups
    PRM_Template(PRM_SWITCHER, 5,   &PRMswitcherName, switcherDef),

    // File name
    PRM_Template(PRM_FILE,      1, &names[0], &nameDefault1,0),

    // Menu for import data type
    PRM_Template(PRM_ORD,       1, &names[1], PRMzeroDefaults,
    &importTypeMenu, 0, SOP_RF_Import::updateTheMenu),

    // Version Number
    PRM_Template(PRM_LABEL,  1, &names[2]),

    // Info lines
    PRM_Template(PRM_LABEL,  1, &names[3]),
    PRM_Template(PRM_LABEL,  1, &names[4]),
    PRM_Template(PRM_LABEL,  1, &names[5]),

    // Particle attributes page
    PRM_Template(PRM_TOGGLE,    1, &names[6], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[7], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[8], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[9], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[10], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[11], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[12], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[13], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[14], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[15], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[16], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[17], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[18], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[19], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[20], 0, 0, 0, 0, 0, 1),

    // Mesh attributes
    PRM_Template(PRM_TOGGLE,    1, &names[21], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[22], 0, 0, 0, 0, 0, 1),

    // SD attributes
    PRM_Template(PRM_TOGGLE,    1, &names[23], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[24], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[25], 0, 0, 0, 0, 0, 1),
    PRM_Template(PRM_TOGGLE,    1, &names[26], 0, 0, 0, 0, 0, 1),

    // RWC attributes page
    PRM_Template(PRM_TOGGLE,    1, &names[27], 0, 0, 0, 0, 0, 1),

    PRM_Template()
};


// Local var enum
enum {
    VAR_PT,
    VAR_NPT
};

// Local variable array
CH_LocalVariable
SOP_RF_Import::myVariables[] = {
    { "PT",     VAR_PT, 0 },
    { "NPT",    VAR_NPT, 0 },
    { 0, 0, 0 },
};



/* ******************************************************************************
*  Function Name : getVariableValue()
*
*  Description : Get out local variables
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
float SOP_RF_Import::getVariableValue(int index, int)
{
    if(myCurrPoint < 0) return 0;

    switch(index) {
    case VAR_PT:
        return myCurrPoint;
    case VAR_NPT:
        return myTotalPoints;
    }

    return 0;
}



/* ******************************************************************************
*  Function Name : myConstructor()
*
*  Description : myConstructor for an "Real Flow Import Operator"  object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */

OP_Node * SOP_RF_Import::myConstructor(OP_Network * net, const char * name, OP_Operator * op)
{
    return new SOP_RF_Import(net, name, op);
}



/* ******************************************************************************
*  Function Name : SOP_RF_Import()
*
*  Description : Constructor for a "Real Flow" import SOP object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_RF_Import::SOP_RF_Import(OP_Network * net, const char * name, OP_Operator * op)
    : SOP_Node(net, name, op)
{
    OP_Node::flags().timeDep = 1;

    myCurrPoint = -1;         // To prevent garbage values from being returned
    myTotalPoints = 0;      // Set the NPT local variable value
    myParmBase = getParmList()->getParmIndex(names[0].getToken());

    myRFParticleFile =   new RealFlow_Particle_File();
    myRFMeshFile =       new RealFlow_Mesh_File();
    myRFSDFile =         new RealFlow_SD_File();
    myRFRWCFile =        new RealFlow_RWC_File();

    myFileType = FTYPE(0);
    myParticleVersion = 0;

    errorMsgs[import_success] = "Real Flow file import successful";
    errorMsgs[import_fail] = "Real Flow file import failed";
    errorMsgs[cookInterrupted] = "Cooking interrupted";

    errorMsgs[endFrameMustBeGreaterThanBeginningFrame] = "End frame must be greater than beginning frame!";
    errorMsgs[currentFrameGreaterThanEndFrame] = "Current frame is past the end frame";
    errorMsgs[currentFrameLessThenZero] = "Current frame is less then zero";
    errorMsgs[currentFrameLessThenOne] = "Current frame is less then one";


    // Particle file error messages
    errorMsgs[canNotOpenRealFlowParticleFileForReading] = "SOP_RF_Import::ReadRFParticleFile(): Can't open Real Flow Particle file for reading";
    errorMsgs[canNotReadTheParticleFileHeader] = "SOP_RF_Import::ReadRFParticleFile(): Can't read Real Flow Particle file header";
    errorMsgs[canNotReadRealFlowParticleData] = "SOP_RF_Import::ReadRFParticleFile(): Can't read Real Flow Particle data";
    errorMsgs[canNotCloseRealFlowParticleFile] = "SOP_RF_Import::ReadRFParticleFile(): Can't close Real Flow Particle file";

    // Mesh file errors
    errorMsgs[canNotOpenRealFlowMeshFileForReading] = "SOP_RF_Import::ReadRFMeshFile(): Can't open Real Flow Mesh file for reading";
    errorMsgs[canNotReadTheMeshFileHeader] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow Mesh file header";
    errorMsgs[notARealFlowMeshFile] = "SOP_RF_Import::ReadRFMeshFile(): Not a Real Flow Mesh file!";
    errorMsgs[numRealFlowMeshVerticesInvalid] = "SOP_RF_Import::ReadRFMeshFile(): Number of vertices in Real Flow mesh file invalid";
    errorMsgs[canNotReadRealFlowMeshData] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow Mesh data";
    errorMsgs[canNotReadTheMeshFileNumFaces] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file number of faces";
    errorMsgs[canNotReadTheMeshFileFaceData] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file face data";
    errorMsgs[canNotReadTheMeshFileTextureChunkCode] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file texture chunk code";
    errorMsgs[canNotReadTheMeshFileNumFluids] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file number of fluids";
    errorMsgs[canNotReadTheMeshFileTextureData] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file texture data";
    errorMsgs[canNotReadTheMeshFileVelocityChunk] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file velocity chunk";
    errorMsgs[canNotReadTheMeshFileVelocityData] = "SOP_RF_Import::ReadRFMeshFile(): Can't read Real Flow mesh file: velocity data";
    errorMsgs[canNotReadTheMeshFileVertextData] = "SOP_RF_Import::ReadRFMeshFile(): Can't read from Real Flow Mesh file: vertex data";
    errorMsgs[canNotReadTheMeshFileEOF] = "SOP_RF_Import::ReadRFMeshFile(): Can't read from Real Flow Mesh file EOF marker";
    errorMsgs[canNotCloseRealFlowMeshFile] = "SOP_RF_Import::ReadRFMeshFile(): Can't close Real Flow Mesh file";

    // RWC file errors
    errorMsgs[canNotOpenRealFlowRWCFileForReading] = "SOP_RF_Import::ReadRFRWCFile(): Can not open Real Flow RWC file for reading";
    errorMsgs[canNotReadTheRWCFileHeader] = "SOP_RF_Import::ReadRFRWCFile(): Can't read Real Flow RWC file header";
    errorMsgs[canNotReadRealFlowRWCData] = "SOP_RF_Import::ReadRFRWCFile(): Can't read Real Flow RWC data";
    errorMsgs[canNotCloseRealFlowRWCFile] = "SOP_RF_Import::ReadRFRWCFile(): Can't close Real Flow RWC file";

    // SD file errors
    errorMsgs[canNotOpenRealFlowSDFileForReading] = "SOP_RF_Import::ReadRFSDFile(): Can't open Real Flow SD file for reading";
    errorMsgs[canNotReadTheSDFileHeader] = "SOP_RF_Import::ReadRFSDFile(): Can't read Real Flow SD file header";
    errorMsgs[canNotReadRealFlowSDData] = "SOP_RF_Import::ReadRFSDFile(): Can't read Real Flow SD data";
    errorMsgs[versionNotSupportedSDFile] = "SOP_RF_Import::ReadRFSDFile(): SD file version not supported, must be version 6 (Real Flow 4) or greater";
    errorMsgs[canNotReadTheSDObjectHeader] = "SOP_RF_Import::ReadRFSDFile(): Can't read object header";
    errorMsgs[canNotReadTheSDFaceCoords] = "SOP_RF_Import::ReadRFSDFile(): Can't read face coordintes";
    errorMsgs[theSDPointCreationInterrupt] = "SOP_RF_Import::ReadRFSDFile(): Point creation interrupted";
    errorMsgs[theSDPolygonCreationInterrupt] = "SOP_RF_Import::ReadRFSDFile(): Polygon creation interrupted";
    errorMsgs[canNotReadTheSDFaceVertexIndex] = "SOP_RF_Import::ReadRFSDFile(): Can't read face index";
    errorMsgs[canNotReadTheSDFaceTexture] = "SOP_RF_Import::ReadRFSDFile(): Can't read face texture";
    errorMsgs[canNotReadTheSDFaceVisibility] = "SOP_RF_Import::ReadRFSDFile(): Can't read face visibility";
    errorMsgs[canNotReadTheSDFaceMaterialIndex] = "SOP_RF_Import::ReadRFSDFile(): Can't read face material index";
    errorMsgs[tooManyObjectsInSDFile] = "SOP_RF_Import::ReadRFSDFile(): Current version only supports 1024 objects per SD file";
    errorMsgs[canNotReadSDCameraData] = "SOP_RF_Import::ReadRFSDFile(): Can't read camera data";
    errorMsgs[incorrectSDCurrentFrame] = "Current frame number does not match SD file frame number";
    errorMsgs[canNotReadSDCameraFrameData] = "SOP_RF_Import::ReadRFSDFile(): Can't read camera frame data";
    errorMsgs[canNotCreateSDRestGeometry] = "SOP_RF_Import::ReadRFSDFile(): Can't create rest geometry";
    errorMsgs[canNotCreateSDAnimatedGeometry] = "SOP_RF_Import::ReadRFSDFile(): Can't create animated geometry";
    errorMsgs[canNotReadSDCurrentFrame] = "SOP_RF_Import::ReadRFSDFile(): Can't read frame number";
    errorMsgs[canNotReadSDObjectFrameHeader] = "SOP_RF_Import::ReadRFSDFile(): Can't read object frame header";
    errorMsgs[theSDAnimGeoCreationInterrupt] = "SOP_RF_Import::ReadRFSDFile(): Cooking interrupted during creating animated geometry";
    errorMsgs[canNotReadSDAnimFaceData] = "SOP_RF_Import::ReadRFSDFile(): Can't read animated face data";
    errorMsgs[theSDAnimGeoCreationPointNULL] = "SOP_RF_Import::ReadRFSDFile(): POINT IS NULL";
    errorMsgs[canNotCloseRealFlowSDFile] = "SOP_RF_Import::ReadRFSDFile(): Can't close Real Flow SD file";


    // Init GUI toggle states
    myGUIState.t_velocity = false;
    myGUIState.t_force = false;
    myGUIState.t_vorticity = false;
    myGUIState.t_normal = false;
    myGUIState.t_num_neighbors = false;
    myGUIState.t_texture_vector = false;
    myGUIState.t_info_bits = false;
    myGUIState.t_age = false;
    myGUIState.t_isolation_time = false;
    myGUIState.t_viscosity = false;
    myGUIState.t_density = false;
    myGUIState.t_pressure = false;
    myGUIState.t_mass = false;
    myGUIState.t_temperature = false;
    myGUIState.t_id = false;

    myGUIState.t_mesh_tex = false;
    myGUIState.t_mesh_vel = false;

    myGUIState.t_sd_tex = false;
    myGUIState.t_sd_cg = false;
    myGUIState.t_sd_obj_xform = false;
    myGUIState.t_sd_cg_xform = false;

    disableParms();
}



/* ******************************************************************************
*  Function Name : ~SOP_RF_Import()
*
*  Description : Destructor for a "Real Flow" import SOP object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_RF_Import::~SOP_RF_Import()
{

    delete(myRFParticleFile);
    delete(myRFMeshFile);
    delete(myRFSDFile);
    delete(myRFRWCFile);

}


/* ******************************************************************************
*  Function Name : updateTheMenu()
*
*  Description : update the menu after the user has made a selection
*
*  Input Arguments : None
*
*  Return Value : int (success?)
*
***************************************************************************** */
int SOP_RF_Import::updateTheMenu(void * data, int index,
                                 float time, const PRM_Template * tplate)
{

    SOP_RF_Import * me = (SOP_RF_Import *) data;

    me->myFileType = me->FTYPE(time);
    me->disableParms();

#ifdef DEBUG
    std::cout << "updateTheMenu() - myFileType = " << me->myFileType << endl;
#endif

    return 1;
}



/* ******************************************************************************
*  Function Name : disableParms()
*
*  Description : Disable (and re-enable) parameters after the use makes a selection
*
*  Input Arguments : None
*
*  Return Value : unsigned num_of_parms_changed
*
***************************************************************************** */
unsigned SOP_RF_Import::disableParms()
{
    unsigned changed = 0;


    // First turn them all off
    for(int i=0; i <= NUM_GUI_PARMS; i++)
        enableParm(i, 0);

#ifdef DEBUG
    std::cout << "SOP_RF_Import::disableParms() myFileType: " << myFileType << endl;
#endif

    switch(myFileType) {

    case 0:
        // Real Flow particle file
        changed  += enableParm("vel", 1);
        changed  += enableParm("force", 1);

        if(myParticleVersion >= 9) {
            changed  += enableParm("vorticity", 1);
        }

        changed  += enableParm("normal", 1);
        changed  += enableParm("num_neighbors", 1);
        changed  += enableParm("texture_vector", 1);
        changed  += enableParm("info_bits", 1);
        changed  += enableParm("age", 1);
        changed  += enableParm("isolation_time", 1);
        changed  += enableParm("viscosity", 1);
        changed  += enableParm("density", 1);
        changed  += enableParm("pressure", 1);
        changed  += enableParm("mass", 1);
        changed  += enableParm("temperature", 1);
        changed  += enableParm("id", 1);
        break;

    case 1:
        // Real Flow mesh file
        changed  += enableParm("m_tex", 1);
        changed  += enableParm("m_vel", 1);
        break;

    case 2:
        // Real Flow SD file
        changed  += enableParm("sd_tex", 1);
        changed  += enableParm("sd_CG", 1);
        changed  += enableParm("sd_obj_xform", 1);
        changed  += enableParm("sd_CG_xform", 1);
        break;

    case 3:
        // Real Flow RWC file
        changed  += enableParm("rwc_vel", 1);
        break;
    }

    // GUI switcher
    changed  += enableParm(0, 1);
    // Filename
    changed  += enableParm("fname", 1);
    // Import file type
    changed  += enableParm("type", 1);
    // Version
    changed  += enableParm("version", 1);

    changed  += enableParm("info1", 1);
    changed  += enableParm("info2", 1);
    changed  += enableParm("info3", 1);

#ifdef DEBUG
    std::cout << "disableParms() - changed: " << changed << endl;
#endif

    return changed;
}



/* ******************************************************************************
*  Function Name : cookMySop()
*
*  Description : Cook this SOP node
*
*  Input Arguments : None
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR SOP_RF_Import::cookMySop(OP_Context & context)
{
    char GUI_str[128];
    OP_ERROR myError;

    float now = context.getTime();
    OP_Node::flags().timeDep = 1;

    myTotalPoints = 0;        // Set the NPT local variable value
    myCurrPoint   = 0;        // Initialize the PT local variable

    FNAME(myFileName, now);
    myFileType = FTYPE(now);

    disableParms();

    sprintf(GUI_str, "%s", "");
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_INFO1, 0, now);
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_INFO2, 0, now);
    sprintf(GUI_str, "Version: %s", SOP_Version.c_str());
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_INFO3, 0, now);

#ifdef DEBUG
    std::cout << "cookMySop() - myFileType = " << myFileType << endl;
#endif

    switch(myFileType) {
    case 0:
        myError = ReadRFParticleFile(context);
        break;
    case 1:
        myError = ReadRFMeshFile(context);
        break;
    case 2:
        myError = ReadRFSDFile(context);
        break;
    case 3:
        myError = ReadRFRWCFile(context);
        break;
    default:
        return error();
    }

    myCurrPoint = -1;
    return myError;
}


/**********************************************************************************/
//  $Log: SOP_RF_Import.C,v $
//  Revision 1.4  2012-08-31 03:00:52  mstory
//  Continued H12 mods.
//
//  Revision 1.3  2012-08-29 03:07:07  mstory
//  Initial changes for H12.
//
//  Revision 1.2  2011-02-11 04:18:01  mstory
//  Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...
//
//
//  --mstory
//
//  Revision 1.1.1.1  2009-01-30 02:24:05  mstory
//  Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.
//
//
//
/**********************************************************************************/
