/* ******************************************************************************
* Real Flow Houdini Import SOP
*
* Description : This plugin will import Real Flow data into Houdini
*
* $RCSfile: SOP_RF_Import.h,v $
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import.h,v $
*
* $Author: mstory $
*
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

#ifndef __SOP_RF_Import_h__
#define __SOP_RF_Import_h__

#include <SOP/SOP_Node.h>

// #define DEBUG

#define RAD2DEG 180/M_PI
#define DEG2RAD M_PI/180

#include "real_flow_sd.h"
#include "real_flow_sd.C"
#include "real_flow_mesh.h"
#include "real_flow_mesh.C"
#include "real_flow_part.h"
#include "real_flow_part.C"
#include "real_flow_RWC.h"
#include "real_flow_RWC.C"

using namespace dca;

const std::string SOP_Version = "2.0.1";

#define NUM_GUI_PARMS 28


//Tab 1 - Real Flow file name and type (particles, mesh or SD) and version
#define ARG_RF_IMPORT_FNAME     (myParmBase + 0)
#define ARG_RF_IMPORT_FTYPE     (myParmBase + 1)
#define ARG_RF_IMPORT_VER        (myParmBase + 2)
#define ARG_RF_IMPORT_INFO1        (myParmBase + 3)
#define ARG_RF_IMPORT_INFO2        (myParmBase + 4)
#define ARG_RF_IMPORT_INFO3        (myParmBase + 5)

//Tab 2 - Import options for Real Flow particles
#define ARG_RF_IMPORT_VELOCITY  (myParmBase + 6)
#define ARG_RF_IMPORT_FORCE     (myParmBase + 7)
#define ARG_RF_IMPORT_VORTICITY (myParmBase + 8)
#define ARG_RF_IMPORT_NORMAL    (myParmBase + 9)
#define ARG_RF_IMPORT_NUM_NEIGHBORS (myParmBase + 10)
#define ARG_RF_IMPORT_TEX_VECTOR    (myParmBase + 11)
#define ARG_RF_IMPORT_INFO_BITS (myParmBase + 12)
#define ARG_RF_IMPORT_AGE       (myParmBase + 13)
#define ARG_RF_IMPORT_ISO_TIME  (myParmBase + 14)
#define ARG_RF_IMPORT_VISCOSITY (myParmBase + 15)
#define ARG_RF_IMPORT_DENSITY   (myParmBase + 16)
#define ARG_RF_IMPORT_PRESSURE  (myParmBase + 17)
#define ARG_RF_IMPORT_MASS      (myParmBase + 18)
#define ARG_RF_IMPORT_TEMPERATURE   (myParmBase + 19)
#define ARG_RF_IMPORT_PARTICLE_ID   (myParmBase + 20)

//Tab 3 - Import options for Real Flow mesh
#define ARG_RF_IMPORT_MESH_TEX  (myParmBase + 21)
#define ARG_RF_IMPORT_MESH_VEL  (myParmBase + 22)

//Tab 4- Import options for Real Flow SD files
#define ARG_RF_IMPORT_SD_TEX    (myParmBase + 23)
#define ARG_RF_IMPORT_SD_CG     (myParmBase + 24)
#define ARG_RF_IMPORT_SD_OBJ_XFORM  (myParmBase + 25)
#define ARG_RF_IMPORT_SD_CG_XFORM   (myParmBase + 26)

//Tab 5 - Import options for Real Flow RWC files
#define ARG_RF_IMPORT_RWC_VELOCITY   (myParmBase + 27)


enum enumExceptionSeverity {
   exceptionNone = 0,
   exceptionWarning,
   exceptionError,
   exceptionCritical
};

// An enum for all the error messages
enum enumErrorList {

   import_success = 0,
   import_fail,

   endFrameMustBeGreaterThanBeginningFrame,
   currentFrameGreaterThanEndFrame,
   currentFrameLessThenZero,
   currentFrameLessThenOne,
   cookInterrupted,

   canNotOpenRealFlowParticleFileForReading,
   canNotReadTheParticleFileHeader,
   canNotReadRealFlowParticleData,
   canNotCloseRealFlowParticleFile,

   canNotOpenRealFlowMeshFileForReading,
   canNotReadTheMeshFileHeader,
   notARealFlowMeshFile,
   numRealFlowMeshVerticesInvalid,
   canNotReadRealFlowMeshData,
   canNotReadTheMeshFileNumFaces,
   canNotReadTheMeshFileFaceData,
   canNotReadTheMeshFileChunkCode,
   canNotReadTheMeshFileTextureChunkCode,
   canNotReadTheMeshFileNumFluids,
   canNotReadTheMeshFileTextureData,
   canNotReadTheMeshFileVelocityChunk,
   canNotReadTheMeshFileVelocityData,
   canNotReadTheMeshFileVertextData,
   canNotReadTheMeshFileEOF,
   canNotCloseRealFlowMeshFile,

   canNotOpenRealFlowRWCFileForReading,
   canNotReadTheRWCFileHeader,
   canNotReadRealFlowRWCData,
   canNotCloseRealFlowRWCFile,

   canNotOpenRealFlowSDFileForReading,
   canNotReadTheSDFileHeader,
   versionNotSupportedSDFile,
   canNotReadTheSDObjectHeader,
   canNotReadTheSDFaceCoords,
   canNotReadRealFlowSDData,
   theSDPointCreationInterrupt,
   theSDPolygonCreationInterrupt,
   canNotReadTheSDFaceVertexIndex,
   canNotReadTheSDFaceTexture,
   canNotReadTheSDFaceVisibility,
   canNotReadTheSDFaceMaterialIndex,
   tooManyObjectsInSDFile,
   canNotReadSDCameraData,
   incorrectSDCurrentFrame,
   canNotReadSDCameraFrameData,
   canNotCreateSDRestGeometry,
   canNotCreateSDAnimatedGeometry,
   canNotReadSDCurrentFrame,
   canNotReadSDObjectFrameHeader,
   theSDAnimGeoCreationInterrupt,
   canNotReadSDAnimFaceData,
   theSDAnimGeoCreationPointNULL,
   canNotCloseRealFlowSDFile,

   numRFImportErrors
};

static UT_String errorMsgs[numRFImportErrors];


/* ******************************************************************************
*  Class Name : OP_RF_Import_Operator()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

class OP_RF_Import_Operator : public OP_Operator
{
   public:
      OP_RF_Import_Operator();
      virtual ~OP_RF_Import_Operator();
};



/* ******************************************************************************
*  Class Name : SOP_RF_Import_Exception()
*
*  Description :  Exception class for Real Flow Import SOP
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

class SOP_RF_Import_Exception
{
      std::string e_msg;
      enumErrorList e_code;
      enumExceptionSeverity severity;

   public:
      SOP_RF_Import_Exception(enumErrorList code, enumExceptionSeverity severity);
//   ~SOP_RF_Import_Exception();

      void what() {
         std::cout << "SOP_RF_Import_Exception::what() - Real Flow import exception:  " << e_msg << endl;
      }
      enumErrorList getErrorCode() {
         return e_code;
      }
      enumExceptionSeverity getSeverity() {
         return severity;
      }
};



/* ******************************************************************************
*  Class Name : SOP_RF_Import()
*
*  Description :
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

class SOP_RF_Import : public SOP_Node
{
   public:
      static OP_Node   *   myConstructor(OP_Network*, const char *, OP_Operator *);

      static PRM_Template      myTemplateList[];
      static CH_LocalVariable  myVariables[];

   protected:
      SOP_RF_Import(OP_Network * net, const char * name, OP_Operator * op);
      virtual ~SOP_RF_Import();

      virtual unsigned    disableParms();
      virtual OP_ERROR    cookMySop(OP_Context & context);
      virtual float       getVariableValue(int index, int thread);

   private:

// Real Flow file name and type (particles, mesh or SD)
      void    FNAME(UT_String & label, float t) {
         evalString(label, ARG_RF_IMPORT_FNAME, 0, t);
      }
      int     FTYPE(float t) {
         return evalInt(ARG_RF_IMPORT_FTYPE, 0, t);
      }

// Import options for Real Flow particles
      int     VELOCITY(float t) {
         return evalInt(ARG_RF_IMPORT_VELOCITY, 0, t);
      }
      int     FORCE(float t) {
         return evalInt(ARG_RF_IMPORT_FORCE, 0, t);
      }
      int     VORTICITY(float t) {
         return evalInt(ARG_RF_IMPORT_VORTICITY, 0, t);
      }
      int     NORMAL(float t) {
         return evalInt(ARG_RF_IMPORT_NORMAL, 0, t);
      }
      int     NUM_NEIGHBORS(float t) {
         return evalInt(ARG_RF_IMPORT_NUM_NEIGHBORS, 0, t);
      }
      int     TEX_VECTOR(float t) {
         return evalInt(ARG_RF_IMPORT_TEX_VECTOR, 0, t);
      }
      int     INFO_BITS(float t) {
         return evalInt(ARG_RF_IMPORT_INFO_BITS, 0, t);
      }
      int     AGE(float t) {
         return evalInt(ARG_RF_IMPORT_AGE, 0, t);
      }
      int     ISO_TIME(float t) {
         return evalInt(ARG_RF_IMPORT_ISO_TIME, 0, t);
      }
      int     VISCOSITY(float t) {
         return evalInt(ARG_RF_IMPORT_VISCOSITY, 0, t);
      }
      int     DENSITY(float t) {
         return evalInt(ARG_RF_IMPORT_DENSITY, 0, t);
      }
      int     PRESSURE(float t) {
         return evalInt(ARG_RF_IMPORT_PRESSURE, 0, t);
      }
      int     MASS(float t) {
         return evalInt(ARG_RF_IMPORT_MASS, 0, t);
      }
      int     TEMPERATURE(float t) {
         return evalInt(ARG_RF_IMPORT_TEMPERATURE, 0, t);
      }
      int     PARTICLE_ID(float t) {
         return evalInt(ARG_RF_IMPORT_PARTICLE_ID, 0, t);
      }

// Import options for Real Flow mesh
      int     MESH_TEX(float t) {
         return evalInt(ARG_RF_IMPORT_MESH_TEX, 0, t);
      }
      int     MESH_VEL(float t) {
         return evalInt(ARG_RF_IMPORT_MESH_VEL, 0, t);
      }

// Import options for Real Flow SD files
      int     SD_TEX(float t) {
         return evalInt(ARG_RF_IMPORT_SD_TEX, 0, t);
      }
      int     SD_CG(float t) {
         return evalInt(ARG_RF_IMPORT_SD_CG, 0, t);
      }
      int     SD_OBJ_XFORM(float t) {
         return evalInt(ARG_RF_IMPORT_SD_OBJ_XFORM, 0, t);
      }
      int     SD_CG_XFORM(float t) {
         return evalInt(ARG_RF_IMPORT_SD_CG_XFORM, 0, t);
      }

// Import options for Real Flow RWC files
      int     RWC_VELOCITY(float t) {
         return evalInt(ARG_RF_IMPORT_RWC_VELOCITY, 0, t);
      }

      static int  updateTheMenu(void * data, int index, float time, const PRM_Template * tplate);
//   static int  updateVersion(void *data, int index, float time, const PRM_Template *tplate );

      struct GUI_state {
         bool
         t_velocity,
         t_force,
         t_vorticity,
         t_normal,
         t_num_neighbors,
         t_texture_vector,
         t_info_bits,
         t_age,
         t_isolation_time,
         t_viscosity,
         t_density,
         t_pressure,
         t_mass,
         t_temperature,
         t_id,

         t_mesh_tex,
         t_mesh_vel,

         t_sd_tex,
         t_sd_cg,
         t_sd_obj_xform,
         t_sd_cg_xform;

      } myGUIState;


      struct Attribute_offsets {
         GA_RWAttributeRef

         // Particle file references
         p_velocity,
         p_force,
         p_vorticity,
         p_normal_N,
         p_num_neighbors,
         p_texture_vector_uv,
         p_info_bits,
         p_age,
         p_isolation_time,
         p_viscosity,
         p_density,
         p_pressure,
         p_mass,
         p_temperature,
         p_id,
         p_min_pressure, p_max_pressure, p_avg_pressure,
         p_min_speed, p_max_speed, p_avg_speed,
         p_min_temp, p_max_temp, p_avg_temp,
         p_scene_scale,
         p_fluid_type,
         p_elapsed_time,
         p_frame_number,
         p_fps,
         p_num_particles,
         p_radius,
         p_emit_pos, p_emit_rot, p_emit_scale,

         //SD file references
         sd_CG_pos, sd_CG_vel, sd_CG_rot,
         sd_texture_0, sd_texture_1, sd_texture_2;

      } myAttributeRefs;

      GA_PointGroup    *    objPointGrpList[maxNumObjects];
      GA_PrimitiveGroup  *  objPrimitiveGrpList[maxNumObjects];

      OP_ERROR ReadRFParticleFile(OP_Context & context);
      int ReadRFParticleFileCreateAttrs();
      int ReadRFParticleFileSetAttrs(long int pt_num);

      OP_ERROR ReadRFMeshFile(OP_Context & context);

      OP_ERROR ReadRFSDFile(OP_Context & context);
      int ReadRFSDReadHeader(float now);
      int ReadRFSDCreateRestGeo(UT_Interrupt * boss);
      int ReadRFSDCreateAnimGeo(UT_Interrupt * boss);

      OP_ERROR ReadRFRWCFile(OP_Context & context);
      void ReadRFRWCFileAddPoint(GEO_Point * ppt, int t_velocity, GA_RWAttributeRef p_velocity_v, int pt_num);

      short   myParmBase; // parameter offsets

      RealFlow_Particle_File * myRFParticleFile;
      RealFlow_Mesh_File  *   myRFMeshFile;
      RealFlow_SD_File   *    myRFSDFile;
      RealFlow_RWC_File   *   myRFRWCFile;

      UT_String  myFileName;

      int     myFileType;
      int     myParticleVersion;
      int     myCurrPoint;
      int     myTotalPoints;
};

#endif


/**********************************************************************************/
//  $Log: SOP_RF_Import.h,v $
//  Revision 1.4  2012-08-31 03:00:51  mstory
//  Continued H12 mods.
//
//  Revision 1.3  2012-08-29 03:07:06  mstory
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
