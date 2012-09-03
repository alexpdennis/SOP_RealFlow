/* ******************************************************************************
*  Real Flow Geometry Export SOP Houdini Extension
*
* $RCSfile: SOP_RF_Export.h,v $
*
* Description : This plugin will export Real Flow SD and BIN files from Houdini
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export.h,v $
*
* $Author: mstory $
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

#ifndef __SOP_RF_Export_h__
#define __SOP_RF_Export_h__

//#define DEBUG

#include "real_flow_sd.h"
#include "real_flow_sd.C"
#include "real_flow_part.h"
#include "real_flow_part.C"
#include "real_flow_RWC.h"
#include "real_flow_RWC.C"

using namespace dca;

const std::string SOP_Version = "2.0.0";

#ifdef SOP_VER
//const std::string SOP_Version = SOP_VER;
#endif


#define RAD2DEG 180/M_PI
#define DEG2RAD M_PI/180

#define SD_FILE 0
#define BIN_FILE 1
#define RWC_FILE 2


#define NUM_GUI_PARMS 31

#define ARG_RF_EXPORT_FNAME         (myParmBase + 0)

#define ARG_RF_EXPORT_SD_BIN		   (myParmBase + 1)
#define ARG_RF_EXPORT_ANIM			   (myParmBase + 2)
#define ARG_RF_EXPORT_BEGIN_END   	(myParmBase + 3)
#define ARG_RF_EXPORT_ECHO_CONSOLE	(myParmBase + 4)

// leave open for "Write RF File" button -------->5

#define ARG_RF_EXPORT_INFO1         (myParmBase + 6)
#define ARG_RF_EXPORT_INFO2         (myParmBase + 7)
#define ARG_RF_EXPORT_INFO3         (myParmBase + 8)

// GUI Params for SD files
#define ARG_RF_EXPORT_OBJ_COLOR		(myParmBase + 9)
#define ARG_RF_EXPORT_OBJ_XFORM		(myParmBase + 10)
#define ARG_RF_EXPORT_MODE		      (myParmBase + 11)

// GUI parms for BIN particle files
#define ARG_RF_EXPORT_FLUID_NAME	   (myParmBase + 12)
#define ARG_RF_EXPORT_FLUID_TYPE	   (myParmBase + 13)
#define ARG_RF_EXPORT_FPS		      (myParmBase + 14)
#define ARG_RF_EXPORT_SCENE_SCALE	(myParmBase + 15)
#define ARG_RF_EXPORT_RADIUS		   (myParmBase + 16)
#define ARG_RF_EXPORT_PRESSURE_MIN	(myParmBase + 17)
#define ARG_RF_EXPORT_PRESSURE_MAX	(myParmBase + 18)
#define ARG_RF_EXPORT_PRESSURE_AVG	(myParmBase + 19)
#define ARG_RF_EXPORT_SPEED_MIN		(myParmBase + 20)
#define ARG_RF_EXPORT_SPEED_MAX		(myParmBase + 21)
#define ARG_RF_EXPORT_SPEED_AVG		(myParmBase + 22)
#define ARG_RF_EXPORT_TEMP_MIN		(myParmBase + 23)
#define ARG_RF_EXPORT_TEMP_MAX		(myParmBase + 24)
#define ARG_RF_EXPORT_TEMP_AVG		(myParmBase + 25)
#define ARG_RF_EXPORT_EMITTER_POS	(myParmBase + 26)
#define ARG_RF_EXPORT_EMITTER_ROT	(myParmBase + 27)
#define ARG_RF_EXPORT_EMITTER_SCALE	(myParmBase + 28)

#define ARG_RF_EXPORT_RWC_NUM_X	(myParmBase + 29)
#define ARG_RF_EXPORT_RWC_NUM_Y	(myParmBase + 30)


enum enumExceptionSeverity {
   exceptionNone = 0,
   exceptionWarning,
   exceptionError,
   exceptionCritical
   };


// An enum for all the error messages
enum enumErrorList {

    export_success = 0,
    export_fail,
    endFrameMustBeGreaterThanBeginningFrame,
    cookInterrupted,

    // SD file error messages
    canNotCalculateChunkSizes,
    canNotOpenRealFlowSDFileForWriting,
    canNotWriteTheSDFileHeader,
    couldNotLockInputInWriteSDFile,
    canNotWriteRealFlowSDObjectHeader,
    canNotWriteFaceCoordinates,
    notTriangularPolygons,
    notAPolygon,
    couldNotCopyGeo,
    canNotWriteFaceIndex,
    canNotWriteFaceTextures,
    canNotWriteFaceVisiblityValues,
    canNotWriteFaceMaterialIndex,
    canNotWriteFrameNumber,
    canNotWriteObjectFrameHeader,
    canNotCloseTheRealFlowSDFile,
    canNotGetTextureFnameString,
    canNotWriteRestGeo,
    canNotWriteAnimGeo,

    // BIN2 particle file error messages
    canNotLockInputsInWriteBINFile,
    writeBINFileOpenRFFileErr,
    canNotBINFileWriteHeaderFrame0,
    canNotWriteTheAdditionalDataSectionFrame0,
    canNotCloseTheRealFlowBINFileFrame0,
    couldNotLockInputInWriteBINFile,
    canNotOpenTheRealFlowParticleBINFile,
    canNotWriteHeaderParticleBINFile,
    canNotWriteParticleDataToBINFile,
    canNotWriteTheAdditionalDataSection,
    canNotCloseTheRealFlowBINFile,

    // RWC file errror messages
   canNotLockInputsInWriteRWCFile,
   couldNotLockInputInWriteRWCFile,
   canNotWriteHeaderRWCFile,
   canNotOpenRWCFileForWriting,
   canNotWriteDataToRWCFile,
   canNotCloseTheRealFlowRWCFile,

   invalidAttrHandleVel,
   invalidAttrHandleForce,
   invalidAttrHandleNormal,
   invalidAttrHandleVorticity,
   invalidAttrHandleUV,
   invalidAttrHandleInfoBits,
   invalidAttrHandleAge,
   invalidAttrHandleIsolation,
   invalidAttrHandleViscosity,
   invalidAttrHandlePressure,
   invalidAttrHandleDensity,
   invalidAttrHandleMass,
   invalidAttrHandleTemperature,
   invalidAttrHandleNumNeighbors,
   invalidAttrHandleID,

   invalidAttrHandle,

   numRFExportErrors

};

   static UT_String errorMsgs[numRFExportErrors];


/* ******************************************************************************
*  Class Name : OP_RF_Export_Operator()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

class OP_RF_Export_Operator : public OP_Operator {
public:
	OP_RF_Export_Operator();
    virtual ~OP_RF_Export_Operator();
//    virtual bool getHDKHelp(UT_String &str) const;

};



/* ******************************************************************************
*  Class Name : SOP_RF_Export_Exception()
*
*  Description :  Exception class for Real Flow Export SOP
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

class SOP_RF_Export_Exception {
   std::string e_msg;
   enumErrorList e_code;
   enumExceptionSeverity severity;

public:
   SOP_RF_Export_Exception(enumErrorList code, enumExceptionSeverity severity);
//   ~SOP_RF_Export_Exception();

   void what() {  std::cout << "SOP_RF_Export_Exception::what() - Real Flow Export exception:  " << e_msg << endl; }
   enumErrorList getErrorCode() { return e_code; }
   enumExceptionSeverity getSeverity() { return severity; }
   };



/* ******************************************************************************
*  Class Name : SOP_RF_Export()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

class SOP_RF_Export : public SOP_Node
{
public:
	SOP_RF_Export(OP_Network *net, const char *name, OP_Operator *op);

    virtual ~SOP_RF_Export();

    static PRM_Template	 	myTemplateList[];
    static OP_Node			*myConstructor(OP_Network*, const char *, OP_Operator *);

protected:
    virtual unsigned      disableParms();
    virtual const char    *inputLabel(unsigned idx) const;

    virtual OP_ERROR cookMySop(OP_Context &context);

private:

    // Functions for GUI widgets (Setup page)
    void    FNAME(UT_String &label, float t)
            { evalString(label, ARG_RF_EXPORT_FNAME, 0, t); }
    int     FILE_FORMAT(float t)
            { return evalInt(ARG_RF_EXPORT_SD_BIN, 0, t); }
    int     ANIM(float t)
            { return evalInt(ARG_RF_EXPORT_ANIM, 0, t); }
    void    BEGIN_END (float *val, float t)
            { evalFloats(ARG_RF_EXPORT_BEGIN_END, val, t); }
    int     ECHO_CONSOLE(float t)
            { return evalInt(ARG_RF_EXPORT_ECHO_CONSOLE, 0, t); }

    // SD file parms
    int	   FPS(float t)
            { return evalInt(ARG_RF_EXPORT_FPS, 0, t); }
    void    OBJ_COLOR(float *val, float t)
            { evalFloats(ARG_RF_EXPORT_OBJ_COLOR, val, t); }
    int     OBJ_XFORM(float t)
            { return evalInt(ARG_RF_EXPORT_OBJ_XFORM, 0, t); }
    int     MODE(float t)
            { return evalInt(ARG_RF_EXPORT_MODE, 0, t); }


    // Particle BIN2 file parms
    void    FLUID_NAME(UT_String &label, float t)
            { evalString(label, ARG_RF_EXPORT_FLUID_NAME, 0, t); }
    int     FLUID_TYPE(float t)
            { return evalInt(ARG_RF_EXPORT_FLUID_TYPE, 0, t); }
    float  SCENE_SCALE(float t)
            { return evalFloat(ARG_RF_EXPORT_SCENE_SCALE, 0, t); }
    float  RADIUS(float t)
            { return evalFloat(ARG_RF_EXPORT_RADIUS, 0, t); }
    float  PRESSURE_MIN(float t)
            { return evalFloat(ARG_RF_EXPORT_PRESSURE_MIN, 0, t); }
    float  PRESSURE_MAX(float t)
            { return evalFloat(ARG_RF_EXPORT_PRESSURE_MAX, 0, t); }
    float  PRESSURE_AVG(float t)
            { return evalFloat(ARG_RF_EXPORT_PRESSURE_AVG, 0, t); }
    float  SPEED_MIN(float t)
            { return evalFloat(ARG_RF_EXPORT_SPEED_MIN, 0, t); }
    float  SPEED_MAX(float t)
            { return evalFloat(ARG_RF_EXPORT_SPEED_MAX, 0, t); }
    float  SPEED_AVG(float t)
            { return evalFloat(ARG_RF_EXPORT_SPEED_AVG, 0, t); }
    float  TEMP_MIN(float t)
            { return evalFloat(ARG_RF_EXPORT_TEMP_MIN, 0, t); }
    float  TEMP_MAX(float t)
            { return evalFloat(ARG_RF_EXPORT_TEMP_MAX, 0, t); }
    float  TEMP_AVG(float t)
            { return evalFloat(ARG_RF_EXPORT_TEMP_AVG, 0, t); }
    void   EMITTER_POS(float *val, float t)
            { evalFloats(ARG_RF_EXPORT_EMITTER_POS, val, t); }
    void   EMITTER_ROT(float *val, float t)
            { evalFloats(ARG_RF_EXPORT_EMITTER_ROT, val, t); }
    void   EMITTER_SCALE(float *val, float t)
            { evalFloats(ARG_RF_EXPORT_EMITTER_SCALE, val, t); }

    // RWC file parms
    int     RWC_NUM_X(float t)
            { return evalInt(ARG_RF_EXPORT_RWC_NUM_X, 0, t); }
    int     RWC_NUM_Y(float t)
            { return evalInt(ARG_RF_EXPORT_RWC_NUM_Y, 0, t); }


    // Callbacks for various GUI widgets
	static int updateAnim(void *data, int index, float time, const PRM_Template *tplate );
	static int updateVersion(void *data, int index, float time, const PRM_Template *tplate );
	static int updateMode(void *data, int index, float time, const PRM_Template *tplate );
    static int updateFileFormat(void *data, int index, float time, const PRM_Template *tplate );
	static int updateFluidType(void *data, int index, float time, const PRM_Template *tplate );
	static int updateFPS(void *data, int index, float time, const PRM_Template *tplate );

	unsigned myupdateMenuStatus;
	unsigned myupdateVersionStatus;
	unsigned myupdateAnimStatus;
	unsigned myupdateModeStatus;
	unsigned myupdateFluidTypeStatus;
	unsigned myupdateFPSStatus;

	static int 	writeTheFile(void *data, int index, float time, const PRM_Template *tplate );

	OP_ERROR writeSDFile(OP_Context& context);
    int calculateChunkSizes(OP_Context& context);
    int writeSDFileRestGeo(OP_Context& context, UT_Interrupt *boss);
    int writeSDFileAnimGeo(OP_Context& context, UT_Interrupt *boss);

	OP_ERROR writeBINFile(OP_Context& context);
    OP_ERROR writeRWCFile(OP_Context& context);

	void doObjectXForm(UT_DMatrix4& xform);

	bool calledFromCallback;
	OP_ERROR myCallBackError;
	enum enumErrorList   myCallBackFlags;

	OP_ERROR reportCallBackErrors(enum enumErrorList errorCode);

    int myNumInputs;
    int myStartFrame;
    int myEndFrame;

	UT_String	myFileName;
	int         myFileFormat;
	bool        myStaticAnim;
  	bool        myObjXform;
    bool        myMode;
	float 		myBeginEnd[2];
	float 		myObjColor[3];

	UT_String 	myFluidName;
	int         myFluidType;
	int         myFPS;
	float       myRadius;
	float       mySceneScale;
	float       myPressureMin;
	float       myPressureMax;
	float       myPressureAvg;
	float       mySpeedMin;
	float       mySpeedMax;
	float       mySpeedAvg;
	float       myTempMin;
	float       myTempMax;
	float       myTempAvg;
	UT_Vector3	myEmitterPos;
	UT_Vector3	myEmitterRot;
	UT_Vector3	myEmitterScale;

   UT_StringArray objectNames;
   UT_StringArray objectTextureNames;

	bool		myEchoData;

	RealFlow_SD_File        *myRFSDFile;
	RealFlow_Particle_File	*myRFBINFile;
	RealFlow_RWC_File       *myRFRWCFile;

   short myParmBase;
};

#endif

/**********************************************************************************/
//  $Log: SOP_RF_Export.h,v $
//  Revision 1.3  2012-08-29 03:07:07  mstory
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
