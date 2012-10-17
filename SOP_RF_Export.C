/* ******************************************************************************
*  Real Flow Geometry Export SOP Houdini Extension
*
*
* $RCSfile: SOP_RF_Export.C,v $
*
* Description : This plugin will export Real Flow SD and BIN files from Houdini
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export.C,v $
*
* $Author: mstory $
*
*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story
*
* See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2005
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */

#include <UT/UT_DSOVersion.h>
#include <UT/UT_EnvControl.h>
#include <UT/UT_Math.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <UT/UT_DMatrix4.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <PRM/PRM_Include.h>
#include <GEO/GEO_Point.h>
#include <GEO/GEO_Primitive.h>
#include <GEO/GEO_Hull.h>
#include <OP/OP_Operator.h>
#include <OP/OP_Director.h>
#include <OP/OP_OperatorTable.h>
#include <SOP/SOP_Guide.h>
#include <SOP/SOP_Node.h>
#include <UT/UT_Interrupt.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GA/GA_AttributeRef.h>

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>

using namespace std;

#include "SOP_RF_Export.h"
#include "SOP_RF_Export_writeRFParticleFile.C"
#include "SOP_RF_Export_writeSDFile.C"
#include "SOP_RF_Export_writeRWCFile.C"
#include "SOP_RF_Export_writeMeshFile.C"



/* ******************************************************************************
*  Function Name : SOP_RF_Export_Exception()
*
*  Description : Constructor for an "Real Flow Export Operator Exception" object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_RF_Export_Exception::SOP_RF_Export_Exception(enumErrorList code, enumExceptionSeverity sev)
{

//   std::cout << "SOP_RF_Export_Exception: in constructor ... " << endl;

    this->e_msg = errorMsgs[code].toStdString();
    this->e_code = code;
    this->severity = sev;

};




//SOP_RF_Export_Exception::~SOP_RF_Export_Exception() {

//   std::cout << "SOP_RF_Export_Exception: in destructor ... " << endl;

//   };





/* ******************************************************************************
*  Function Name : OP_RF_Export_Operator()
*
*  Description : Constructor for an "Real Flow Export Operator"  object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_RF_Export_Operator::OP_RF_Export_Operator()
    : OP_Operator("rf_export",
                  "Real Flow Export",
                  SOP_RF_Export::myConstructor,
                  SOP_RF_Export::myTemplateList,
                  1,
                  65535,
                  0)
{
}



/* ******************************************************************************
*  Function Name : OP_RF_Export_Operator()
*
*  Description :  Destructor for a OP_RF_Export_Operator object
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_RF_Export_Operator::~OP_RF_Export_Operator()
{
}


/* ******************************************************************************
*  Function Name :  newSopOperator()
*
*  Description :
*
*  Input Arguments : OP_OperatorTable *table
*
*  Return Value :
*
***************************************************************************** */
void newSopOperator(OP_OperatorTable * table)
{
    table->addOperator(new OP_RF_Export_Operator());
}



/* ******************************************************************************
*  Function Name : Data Area
*
*  Description :
*
***************************************************************************** */

#define NPARMS_FOLDER_1 9
#define NPARMS_FOLDER_2 3
#define NPARMS_FOLDER_3 17
#define NPARMS_FOLDER_4 2
#define NPARMS_FOLDER_5 2

static PRM_Default  switcherDef[] = {
    PRM_Default(NPARMS_FOLDER_1,    "Setup"),
    PRM_Default(NPARMS_FOLDER_2,    "SD File"),
    PRM_Default(NPARMS_FOLDER_3,    "Particle File"),
    PRM_Default(NPARMS_FOLDER_4,    "RWC File"),
    PRM_Default(NPARMS_FOLDER_5,    "Mesh File"),
};


static PRM_Name names[] = {
    // Setup parameters
    PRM_Name("file_name",  "File Name"),
    PRM_Name("file_format", "File Format"),
    PRM_Name("anim",       "SD Animation"),
    PRM_Name("start_end",  "Start/End"),
    PRM_Name("console",    "Echo Data To Console (debug)"),
    PRM_Name("write_file",  "Write Real Flow File"),
    PRM_Name("info1", ""),
    PRM_Name("info2", ""),
    PRM_Name("info3", ""),

    // SD file parameters
    PRM_Name("object_clr", "Object Color"),
    PRM_Name("obj_xform",  "Apply Object Transform"),
    PRM_Name("mode",       "Mode"),

    // BIN2 particle file parameters
    PRM_Name("fluid_name",    "Fluid Name"),
    PRM_Name("fluid_type",    "Fluid Type"),
    PRM_Name("fps",           "Frames/Sec"),
    PRM_Name("scene_scale",      "Scene Scale"),
    PRM_Name("radius",        "Particle Radius"),
    PRM_Name("pressure_min",  "Pressure (Min)"),
    PRM_Name("pressure_max",  "Pressure (Max)"),
    PRM_Name("pressure_avg",  "Pressure (Avg)"),
    PRM_Name("speed_min",     "Speed (Min)"),
    PRM_Name("speed_max",     "Speed (Max)"),
    PRM_Name("speed_avg",     "Speed (Avg)"),
    PRM_Name("temp_min",      "Temperature (Min)"),
    PRM_Name("temp_max",      "Temperature (Max)"),
    PRM_Name("temp_avg",      "Temperature (Avg)"),
    PRM_Name("emitter_pos",   "Emitter Position"),
    PRM_Name("emitter_rot",   "Emitter Rotation"),
    PRM_Name("emitter_scl",   "Emitter Scale"),

    // RWC file parameters
    PRM_Name("rwc_num_x", "RWC Num X"),
    PRM_Name("rwc_num_z", "RWC Num Z"),

    // Mesh file parameters
    PRM_Name("mesh_vel", "Mesh Vel"),
    PRM_Name("mesh_tex", "Mesh Tex"),

    PRM_Name(0)
};

// Defaults
static PRM_Default nameDefault_filename(0,"untitled");
static PRM_Default nameDefault_fluid_name(0,"My Fluid");
static PRM_Default dataDefault0(0.0);
static PRM_Default dataDefault1(1.0);
static PRM_Default dataDefault0_1(0.1);
static PRM_Default dataDefault100(100.0);
static PRM_Default dataDefault240(240.0);
static PRM_Default dataDefault_100(-100.0);
static PRM_Default dataDefault1000(1000.0);
static PRM_Default dataDefault_1000(-1000.0);
static PRM_Default versionDefault(1);

// Ranges
static PRM_Range  animRange(PRM_RANGE_UI, 1, PRM_RANGE_UI, 300);
static PRM_Range  dataRange1000(PRM_RANGE_UI, -1000, PRM_RANGE_UI, 1000);
static PRM_Range  dataRange100(PRM_RANGE_UI, -100, PRM_RANGE_UI, 100);

// Menus
static PRM_Name theExportTypeMenu[] = {
    PRM_Name("0", "Real Flow SD"),
    PRM_Name("1", "Real Flow Particles"),
    PRM_Name("2", "Real Wave Cache"),
    PRM_Name("3", "Real Wave Mesh"),
    PRM_Name(0),
};

static PRM_Name theAnimMenu[] = {
    PRM_Name("0", "Non-Animated"),
    PRM_Name("1", "Animated"),
    PRM_Name(0),
};

static PRM_Name theModeMenu[] = {
    PRM_Name("0", "Matrix"),
    PRM_Name("1", "Vertex"),
    PRM_Name(0),
};


static PRM_Name theFluidTypeMenu[] = {
    PRM_Name("0", "Gas"),
    PRM_Name("1", "Liquid"),
    PRM_Name("2", "Dumb"),
    PRM_Name("3", "Elastics"),
    PRM_Name(0),
};


static PRM_Name theFPSMenu[] = {
    PRM_Name("0", "30"),
    PRM_Name("1", "25"),
    PRM_Name("2", "24"),
    PRM_Name(0),
};

static PRM_ChoiceList exportTypeMenu((PRM_ChoiceListType)(PRM_CHOICELIST_EXCLUSIVE | PRM_CHOICELIST_REPLACE), theExportTypeMenu);
static PRM_ChoiceList animMenu((PRM_ChoiceListType)(PRM_CHOICELIST_EXCLUSIVE | PRM_CHOICELIST_REPLACE), theAnimMenu);
static PRM_ChoiceList modeMenu((PRM_ChoiceListType)(PRM_CHOICELIST_EXCLUSIVE | PRM_CHOICELIST_REPLACE), theModeMenu);
static PRM_ChoiceList fluidTypeMenu((PRM_ChoiceListType)(PRM_CHOICELIST_EXCLUSIVE | PRM_CHOICELIST_REPLACE), theFluidTypeMenu);
static PRM_ChoiceList FPSMenu((PRM_ChoiceListType)(PRM_CHOICELIST_EXCLUSIVE | PRM_CHOICELIST_REPLACE), theFPSMenu);


// Build the GUI template
PRM_Template SOP_RF_Export::myTemplateList[] = {

    //GUI groups
    PRM_Template(PRM_SWITCHER, 5, &PRMswitcherName, switcherDef),

    // Filename of the file to be exported
    PRM_Template(PRM_FILE, 1, &names[0], &nameDefault_filename, 0),

    // File type to be exported
    PRM_Template(PRM_ORD, 1, &names[1], PRMzeroDefaults, &exportTypeMenu, 0, SOP_RF_Export::updateFileFormat),

    // Animated/Non-Animated Menu
    PRM_Template(PRM_ORD, 1, &names[2], PRMoneDefaults, &animMenu, 0, SOP_RF_Export::updateAnim),
    // Animation Range
    PRM_Template(PRM_BEGINEND_J,2, &names[3], PRMoneDefaults, 0, &animRange),
    // Echo data toggle
    PRM_Template(PRM_TOGGLE, 1, &names[4]),

    // Write the file button
    PRM_Template(PRM_CALLBACK, 1, &names[5], 0, 0, 0, SOP_RF_Export::writeTheFile),

    // Info lines
    PRM_Template(PRM_LABEL,  1, &names[6]),
    PRM_Template(PRM_LABEL,  1, &names[7]),
    PRM_Template(PRM_LABEL,  1, &names[8]),

    // SD Files Widgets
    // Object Color
    PRM_Template(PRM_RGB, 3, &names[9], PRMoneDefaults),
    // Object transform toggle
    PRM_Template(PRM_TOGGLE, 1, &names[10]),
    // Mode Menu
    PRM_Template(PRM_ORD, 1, &names[11], PRMzeroDefaults, &modeMenu, 0, SOP_RF_Export::updateMode),

    // Particle GUI widgets
    // Fluid Name
    PRM_Template(PRM_STRING, 1, &names[12], &nameDefault_fluid_name, 0),
    // Fluid type
    PRM_Template(PRM_ORD, 1, &names[13], PRMzeroDefaults, &fluidTypeMenu, 0, SOP_RF_Export::updateFluidType),
    // FPS
    PRM_Template(PRM_ORD, 1, &names[14], PRMzeroDefaults, &FPSMenu, 0, SOP_RF_Export::updateFPS),
    // Scene scale
    PRM_Template(PRM_FLT_J, 1, &names[15], &dataDefault1),
    // Particle Radius
    PRM_Template(PRM_FLT_J, 1, &names[16], &dataDefault0_1),
    //Pressure (Min)
    PRM_Template(PRM_FLT_J, 1, &names[17], &dataDefault_100, 0, &dataRange100),
    //Pressure (Max)
    PRM_Template(PRM_FLT_J, 1, &names[18], &dataDefault100, 0, &dataRange100),
    //Pressure (Avg)
    PRM_Template(PRM_FLT_J, 1, &names[19], PRMzeroDefaults, 0, &dataRange100),
    // Speed (Min)
    PRM_Template(PRM_FLT_J, 1, &names[20], &dataDefault_100, 0, &dataRange100),
    // Speed (Max)
    PRM_Template(PRM_FLT_J, 1, &names[21], &dataDefault100, 0, &dataRange100),
    // Speed (Avg)
    PRM_Template(PRM_FLT_J, 1, &names[22], PRMzeroDefaults, 0, &dataRange100),
    // Temperature (Min)
    PRM_Template(PRM_FLT_J, 1, &names[23], &dataDefault_1000, 0, &dataRange1000),
    // Temperature (Max)
    PRM_Template(PRM_FLT_J, 1, &names[24], &dataDefault1000, 0, &dataRange1000),
    // Temperature (Avg)
    PRM_Template(PRM_FLT_J, 1, &names[25], PRMzeroDefaults, 0, &dataRange1000),
    // Emitter Position
    PRM_Template(PRM_XYZ_J, 3, &names[26], PRMzeroDefaults),
    // Emitter Rotation
    PRM_Template(PRM_XYZ_J, 3, &names[27], PRMzeroDefaults),
    // Emitter Scale
    PRM_Template(PRM_XYZ_J, 3, &names[28], PRMoneDefaults),

    // RWC Num X & Y
    PRM_Template(PRM_INT, 1, &names[29], PRMoneDefaults),
    PRM_Template(PRM_INT, 1, &names[30], PRMoneDefaults),

    // Mesh
    PRM_Template(PRM_TOGGLE, 1, &names[31], PRMoneDefaults),
    PRM_Template(PRM_TOGGLE, 1, &names[32], PRMoneDefaults),


    PRM_Template()
};



/* ******************************************************************************
*  Function Name :  myConstructor()
*
*  Description : "My Constructor"
*
*  Input Arguments : OP_Network *net, const char *name, OP_Operator *op
*
*  Return Value : OP_Node *
*
***************************************************************************** */
OP_Node * SOP_RF_Export::myConstructor(OP_Network * net, const char * name, OP_Operator * op)
{
    return new SOP_RF_Export(net, name, op);
}


/* ******************************************************************************
*  Function Name :  Constructor()
*
*  Description : The Constructor
*
*  Input Arguments : OP_Network *net, const char *name, OP_Operator *op
*
*  Return Value :
*
***************************************************************************** */
SOP_RF_Export::SOP_RF_Export(OP_Network * net, const char * name, OP_Operator * op)
    : SOP_Node(net, name, op)
{
    // Initialize this object
    myParmBase = getParmList()->getParmIndex(names[0].getToken());
    calledFromCallback = false;
    myStaticAnim = true;
    myMode = false;
    myFileFormat = 0;
    myFluidType = 1;
    myEchoData = false;
    myObjColor[0] = 0;
    myObjColor[1] = 0.5;
    myObjColor[2] = 1;
    myBeginEnd[0] = 1;
    myBeginEnd[1] = 240;

    myNumInputs = 0;
    myStartFrame = 0;
    myEndFrame = 0;

    myRadius = 0.1;
    mySceneScale = 1.0;
    myPressureMin = 0.0;
    myPressureMax = 0.0;
    myPressureAvg = 0.0;
    mySpeedMin = 0.0;
    mySpeedMax = 0.0;
    mySpeedAvg = 0.0;
    myTempMin = 0.0;
    myTempMax = 0.0;
    myTempAvg = 0.0;

    myFPS = 30;

    myObjXform = false;
    objectNames.clear();
    objectTextureNames.clear();

    myCallBackFlags = (enumErrorList)0;
    myupdateAnimStatus = 0;
    myupdateMenuStatus = 0;
    myupdateFluidTypeStatus = 0;
    myupdateFPSStatus = 0;

    errorMsgs[export_success] = "Real Flow file export successful";
    errorMsgs[export_fail] = "Real Flow file export failed";
    errorMsgs[endFrameMustBeGreaterThanBeginningFrame] = "End frame must be greater than beginning frame!";
    errorMsgs[cookInterrupted] = "Cooking interrupted";


    // SD file error messages
    errorMsgs[canNotOpenRealFlowSDFileForWriting] = "Can't open Real Flow SD file for writing";
    errorMsgs[canNotCalculateChunkSizes] = "Could not calculate the chunk sizes";
    errorMsgs[canNotWriteTheSDFileHeader] = "Can't write the SD file header";
    errorMsgs[couldNotLockInputInWriteSDFile] = "Could not lock input in writeSDFile()!";
    errorMsgs[canNotWriteRealFlowSDObjectHeader] = "Can't write Real Flow SD object header!";
    errorMsgs[canNotWriteFaceCoordinates] = "Can't write face coordinates";
    errorMsgs[notTriangularPolygons] = "Not Triangular Polygons!";
    errorMsgs[notAPolygon] = "Not a Polygon!";
    errorMsgs[canNotWriteRestGeo] = "Failed during while writing rest geometry";
    errorMsgs[canNotWriteAnimGeo] = "Failed during while writing animated geometry";
    errorMsgs[couldNotCopyGeo] = "Could not copy geometry";
    errorMsgs[canNotWriteFaceIndex] = "Can't write face index!";
    errorMsgs[canNotWriteFaceTextures] = "Can't write face textures!";
    errorMsgs[canNotWriteFaceCoordinates] = "Can't write the face coordinates!";
    errorMsgs[canNotWriteFaceVisiblityValues] = "Can't write face visiblity values!";
    errorMsgs[canNotWriteFaceMaterialIndex] = "Can't write face material index!";
    errorMsgs[canNotGetTextureFnameString] = "Can't get the texture filename string";
    errorMsgs[canNotWriteFrameNumber] = "Can't write frame number!";
    errorMsgs[canNotWriteObjectFrameHeader] = "Can't write object frame header";
    errorMsgs[canNotCloseTheRealFlowSDFile] = "Can't close the Real Flow SD File!";

    // BIN2 particle file error messages
    errorMsgs[canNotLockInputsInWriteBINFile] = "SOP_RF_Export::writeBINFile(): Could not lock input in writeBINFile()";
    errorMsgs[writeBINFileOpenRFFileErr] = "SOP_RF_Export::writeBINFile(): Can not open Real Flow file for writing in writeBINFile()";
    errorMsgs[canNotBINFileWriteHeaderFrame0] = "SOP_RF_Export::writeBINFile(): Can't write header writeBinFile() (Frame 0) ";
    errorMsgs[canNotWriteTheAdditionalDataSectionFrame0] = "SOP_RF_Export::writeBINFile(): Can't write the [Additional Data] section (Frame 0)";
    errorMsgs[canNotCloseTheRealFlowBINFileFrame0] = "SOP_RF_Export::writeBINFile(): Can't close the Real Flow BIN File! (Frame 0)";
    errorMsgs[couldNotLockInputInWriteBINFile] = "SOP_RF_Export::writeBINFile(): Could not lock input in writeBINFile()!";
    errorMsgs[canNotOpenTheRealFlowParticleBINFile] = "SOP_RF_Export::writeBINFile(): Can't open the Real Flow particle BIN file ";
    errorMsgs[canNotWriteHeaderParticleBINFile] = "SOP_RF_Export::writeBINFile(): Can't write header Real Flow particle BIN file ";
    errorMsgs[canNotWriteParticleDataToBINFile] = "SOP_RF_Export::writeBINFile(): Can't write the particle data to the Real Flow BIN File!";
    errorMsgs[canNotWriteTheAdditionalDataSection] = "SOP_RF_Export::writeBINFile(): Can't write the [Additional Data] section";
    errorMsgs[canNotCloseTheRealFlowBINFile] = "SOP_RF_Export::writeBINFile(): Can't close the Real Flow BIN File!";

    // RWC file error messages
    errorMsgs[canNotLockInputsInWriteRWCFile] = "Could not lock input in writeRWCFile()";
    errorMsgs[canNotWriteHeaderRWCFile] = "Could not write RWC file header in writeRWCFile()";
    errorMsgs[canNotOpenRWCFileForWriting] = "Could not open RWC file for writing in writeRWCFile()";
    errorMsgs[canNotWriteDataToRWCFile] = "Could not write data to RWC file in writeRWCFile()";
    errorMsgs[canNotCloseTheRealFlowRWCFile] = "Could not close RWC file in writeRWCFile()";

    // Mesh file error messages
    errorMsgs[canNotLockInputsInWriteMeshFile] = "Could not lock input in writeMeshFile()";
    errorMsgs[canNotWriteHeaderMeshFile] = "Could not write Mesh file header in writeMeshFile()";
    errorMsgs[canNotOpenMeshFileForWriting] = "Could not open Mesh file for writing in writeMeshFile()";
    errorMsgs[canNotWriteDataToMeshFile] = "Could not write data to Mesh file in writeMeshFile()";
    errorMsgs[canNotWriteNumFacesToMeshFile] = "Could now write num faces in writeMeshFile()";
    errorMsgs[canNotCloseTheRealFlowMeshFile] = "Could not close Mesh file in writeMeshFile()";
    errorMsgs[canNotWriteEOFToMeshFile] = "Could not write EOF to Mesh file in writeMeshFile()";


    errorMsgs[invalidAttrHandle] = "Invalid Attribute handle/reference";

    errorMsgs[invalidAttrHandleVel] = "Invalid Attribute-vel";
    errorMsgs[invalidAttrHandleForce] = "Invalid Attribute-force";
    errorMsgs[invalidAttrHandleNormal] = "Invalid Attribute-normal";
    errorMsgs[invalidAttrHandleVorticity] = "Invalid Attribute-vorticity";
    errorMsgs[invalidAttrHandleUV] = "Invalid Attribute-uv";
    errorMsgs[invalidAttrHandleInfoBits] = "Invalid Attribute-info bit";
    errorMsgs[invalidAttrHandleAge] = "Invalid Attribute-age";
    errorMsgs[invalidAttrHandleIsolation] = "Invalid Attribute-isolation time";
    errorMsgs[invalidAttrHandleViscosity] = "Invalid Attribute-viscosicty";
    errorMsgs[invalidAttrHandlePressure] = "Invalid Attribute-pressure";
    errorMsgs[invalidAttrHandleDensity] = "Invalid Attribute-density";
    errorMsgs[invalidAttrHandleMass] = "Invalid Attribute-mass";
    errorMsgs[invalidAttrHandleTemperature] = "Invalid Attribute-temperature";
    errorMsgs[invalidAttrHandleNumNeighbors] = "Invalid Attribute-num neighbors";
    errorMsgs[invalidAttrHandleID] = "Invalid Attribute-id";

    myRFSDFile = new RealFlow_SD_File();
    myRFBINFile = new RealFlow_Particle_File();
    myRFRWCFile = new RealFlow_RWC_File();
    myRFMeshFile = new RealFlow_Mesh_File();

    disableParms();

}


/* ******************************************************************************
*  Function Name :  ~SOP_RF_Export()
*
*  Description :
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
SOP_RF_Export::~SOP_RF_Export()
{

    if(myRFRWCFile)
        delete(myRFSDFile);
    if(myRFBINFile)
        delete(myRFBINFile);
    if(myRFRWCFile)
        delete(myRFRWCFile);
    if(myRFMeshFile)
        delete(myRFMeshFile);

}



/* ******************************************************************************
*  Function Name : updateFileFormat()
*
*  Description : Update the menu after the user has selected file type
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_RF_Export::updateFileFormat(void * data, int index, float time, const PRM_Template * tplate)
{

    SOP_RF_Export * me = (SOP_RF_Export *) data;

    me->myFileFormat = me->FILE_FORMAT(time);
    me->myupdateMenuStatus = me->disableParms();

#ifdef DEBUG
    std::cout << "updateFileFormat() - myFileFormat = " << me->myFileFormat << endl;
#endif

    return 1;
}


/* ******************************************************************************
*  Function Name :  updateAnim()
*
*  Description : Update the Start/End GUI widget in response to user
*              selecting the "animation/non-animated" menu
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_RF_Export::updateAnim(void * data, int index, float time, const PRM_Template * tplate)
{

#ifdef DEBUG
    std::cout << "Updating Start/End menu" << endl;
#endif

    SOP_RF_Export * me = (SOP_RF_Export *) data;
    me->myStaticAnim = me->ANIM(time);
    me->myupdateAnimStatus  = me->disableParms();

    return 1;
}


/* ******************************************************************************
*  Function Name :  updateMode()
*
*  Description : Update the mode menu
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_RF_Export::updateMode(void * data, int index, float time, const PRM_Template * tplate)
{

#ifdef DEBUG
    std::cout << "Updating Mode menu" << endl;
#endif

    SOP_RF_Export * me = (SOP_RF_Export *) data;
    me->myMode = me->MODE(time);
    me->myupdateModeStatus  = me->disableParms();

    return 1;
}


/* ******************************************************************************
*  Function Name :  updateFluidType()
*
*  Description : Update the fluid type menu
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_RF_Export::updateFluidType(void * data, int index, float time, const PRM_Template * tplate)
{

#ifdef DEBUG
    std::cout << "Updating Fluid Type menu" << endl;
#endif

    SOP_RF_Export * me = (SOP_RF_Export *) data;
    me->myFluidType = me->FLUID_TYPE(time);
    me->myupdateFluidTypeStatus  = me->disableParms();

    return 1;
}


/* ******************************************************************************
*  Function Name :  updateFPS()
*
*  Description : Update the frame/second menu
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_RF_Export::updateFPS(void * data, int index, float time, const PRM_Template * tplate)
{

#ifdef DEBUG
    std::cout << "updateFPS(): Updating FPS menu" << endl;
#endif

    SOP_RF_Export * me = (SOP_RF_Export *) data;
    int selection = me->FPS(time);

#ifdef DEBUG
    std::cout << "updateFPS(): FPS: " << selection << endl;
#endif

    switch(selection) {
    case 0:
        me->myFPS = 30;
        break;
    case 1:
        me->myFPS = 25;
        break;
    case 2:
        me->myFPS = 24;
        break;
    }

    me->myupdateFPSStatus  = me->disableParms();

    return 1;
}



/* ******************************************************************************
*  Function Name :  writeTheFile()
*
*  Description : Handle the "Write the Real Flow file" button event
*
*  Input Arguments : void *data, int index, float time, const PRM_Template *tplate
*
*  Return Value : int
*
***************************************************************************** */
int SOP_RF_Export::writeTheFile(void * data, int index, float time, const PRM_Template * tplate)
{

#ifdef DEBUG
    std::cout << "writeTheFile() - Writing the Real Flow File" << endl;
#endif

    // Set the callback flag true and reset the error flag as well
    SOP_RF_Export * me = (SOP_RF_Export *) data;
    me->calledFromCallback = true;
    me->myCallBackFlags = (enumErrorList)0;

// void  setData (OP_ContextData *data)
    OP_Context myContext(time);
    myContext.setData(static_cast<OP_ContextData *>(data));

    me->myCallBackError = me->cookMe(myContext);

    return 1;
}



/* ******************************************************************************
*  Function Name :  disableParms()
*
*  Description : Disable or enable GUI parms depending what file the user is creating
*
*  Input Arguments : None
*
*  Return Value : unsigned num_of_parms_changed
*
***************************************************************************** */
unsigned SOP_RF_Export::disableParms()
{
    unsigned changed = 0;

    // First turn them all off
    for(int i=0; i <= NUM_GUI_PARMS; i++)
        enableParm(i, 0);

    // Always on
    enableParm(0, 1);                // GUI switcher
    enableParm("file_name", 1);     // fname
    enableParm("file_format", 1);    // SD/BIN/RWC file format
    enableParm("console", 1);          // echo to console
    enableParm("write_file", 1);       // write file

    enableParm("info1", 1);
    enableParm("info2", 1);
    enableParm("info3", 1);

    // If we're writing an SD file
    if(myFileFormat == SD_FILE) {

        changed  += enableParm("anim", 1);           // non-animated/animated
        // change the state of the Start/End GUI widget
        if(myStaticAnim) {
            changed  += enableParm("start_end", 1);
        } else {
            changed  += enableParm("start_end", 0);
        }

//       changed  += enableParm("start_end", 1);
        changed  += enableParm("object_clr", 1);      // obj color
        changed  += enableParm("obj_xform", 1);       // apply object transforms
        changed  += enableParm("mode", 1);            // SD file mode ("vertex" or "matrix")

    }
    // else we're writing a particle BIN2 file
    else if(myFileFormat == BIN_FILE) {

//       changed  += enableParm("anim", 1);           // non-animated/animated
        changed  += enableParm("start_end", 1);         // start/end (particle exports are always animated)
        changed  += enableParm("version", 1);         // RF file version
        changed  += enableParm("fluid_name", 1);     // fluid name
        changed  += enableParm("fluid_type", 1);     // fluid type
        changed  += enableParm("fps", 1);            // FPS
        changed  += enableParm("scene_scale", 1);    // scene scale
        changed  += enableParm("radius", 1);         // radius
        changed  += enableParm("pressure_min", 1);   // pressure min
        changed  += enableParm("pressure_max", 1);   // pressure max
        changed  += enableParm("pressure_avg", 1);   // pressure avg
        changed  += enableParm("speed_min", 1);      // speed min
        changed  += enableParm("speed_max", 1);      // speed max
        changed  += enableParm("speed_avg", 1);      // speed avg
        changed  += enableParm("temp_min", 1);       // temp min
        changed  += enableParm("temp_max", 1);       // temp max
        changed  += enableParm("temp_avg", 1);       // temp avg
        changed  += enableParm("emitter_pos", 1);    // emitter pos
        changed  += enableParm("emitter_rot", 1);    // emitter rot
        changed  += enableParm("emitter_scl", 1);    // emitter scale
    }
    // If we're writing an RWC file
    else if(myFileFormat == RWC_FILE) {

//    changed  += enableParm("anim", 1);           // non-animated/animated
        changed  += enableParm("start_end", 1);      // start/end (particle exports are always animated)
        changed  += enableParm("rwc_num_x", 1);
        changed  += enableParm("rwc_num_z", 1);

    }
    // If we're writing a mesh file
    else if(myFileFormat == MESH_FILE) {

//    changed  += enableParm("anim", 1);           // non-animated/animated
        changed  += enableParm("start_end", 1);      // start/end (particle exports are always animated)
        changed  += enableParm("mesh_vel", 1);
        changed  += enableParm("mesh_tex", 1);

    }

#ifdef DEBUG
    std::cout << "disableParms()-changed: " << changed << endl;
#endif

    return changed;
}



/* ******************************************************************************
*  Function Name :  cookMySop()
*
*  Description :
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_ERROR SOP_RF_Export::cookMySop(OP_Context & context)
{
    UT_Vector3 trans, rot, scale, up, shear;
    char GUI_str[128];
    OP_ERROR myError;

    OP_Node::flags().timeDep = 1;

    float * myBeginEnd_ptr = myBeginEnd;
    float now = context.getTime();

    disableParms();

    // Evaluate the GUI parameters
    myFileFormat = (int)FILE_FORMAT(now);
    myStaticAnim = (bool)ANIM(now);
    myMode = (bool)MODE(now);
    BEGIN_END(myBeginEnd_ptr, now);
    FNAME(myFileName, now);
    myEchoData =   ECHO_CONSOLE(now);


    disableParms();

    sprintf(GUI_str, "%s", "");
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_EXPORT_INFO1, 0, now);
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_EXPORT_INFO2, 0, now);
    sprintf(GUI_str, "Version: %s", SOP_Version.c_str());
    setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_EXPORT_INFO3, 0, now);


    // If there has been a error from the previous callback, report it
    if(myCallBackFlags) {
        return(reportCallBackErrors(myCallBackFlags));
    }


#ifdef DEBUG
    std::cout << "myFileFormat " << myFileFormat << endl;
#endif

    switch(myFileFormat) {
    case SD_FILE:
        myError = writeSDFile(context);
        break;

    case BIN_FILE:
        myError = writeBINFile(context);
        break;

    case RWC_FILE:
        myError = writeRWCFile(context);
        break;

    case MESH_FILE:
        myError = writeMeshFile(context);
        break;

    default:
        return error();
    }

    return myError;
}



/* ******************************************************************************
*  Function Name :  inputLabel()
*
*  Description :  Provides for a readable label when the user middle clicks on the input connector
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
const char * SOP_RF_Export::inputLabel(unsigned) const
{
    return "Geometry to Export to Real Flow";
}



/* ******************************************************************************
*  Function Name :  reportCallBackErrors()
*
*  Description :
*
*  Input Arguments : None
*
*  Return Value :
*
***************************************************************************** */
OP_ERROR SOP_RF_Export::reportCallBackErrors(enum enumErrorList errorCode)
{

#ifdef DEBUG
    std::cout << "reportCallBackErrors() - myCallBackError: " << "\t" << errorCode <<  endl;
#endif


    // TODO: Open the error log and write error message

    // Add error message to SOP message queue
    addMessage(SOP_MESSAGE,errorMsgs[errorCode]);

    return error();
}



/**********************************************************************************/
//  $Log: SOP_RF_Export.C,v $
//  Revision 1.4  2012-08-31 03:00:51  mstory
//  Continued H12 mods.
//
//  Revision 1.3  2012-08-29 03:07:06  mstory
//  Initial changes for H12.
//
//  Revision 1.2  2011-02-11 04:18:00  mstory
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
