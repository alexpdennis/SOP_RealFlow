/* ******************************************************************************
* Real Flow SD Geometry File
*
* $RCSfile: real_flow_sd.h,v $
*
* Description :  Interface for the RealFlow_SD_File class
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_sd.h,v $
*
* $Author: mstory $
*
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

#ifndef __real_flow_sd_h__
#define __real_flow_sd_h__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

namespace dca
{

#ifndef RF_FILE_READ
#define RF_FILE_READ 0
#define RF_FILE_WRITE 1
#endif

   const int sdHeaderSize = 83;
   const int sdObjectHeaderSize = 29;        // this struct size *does not* include the size of the std::string containers, that data is calculated at run time.
   const int sdFaceDataSize = 64;
   const int sdCamHeaderSize = 24;
   const int sdObjectFrameHeaderSize = 172;  // this struct size *does not* include the size of the std::string container, that data is calculated at run time.
   const int sdCamFrameDataSize = 104;

   const int maxNumObjects = 1024;

   class RF_SD_Exception
   {
         std::string e_msg;

      public:
         RF_SD_Exception(std::string msg);
         ~RF_SD_Exception();

         void what() {
            std::cout << "RF_SD_Exception: " << e_msg << std::endl;
         }
   };


// The Real Flow SD File Class
   class RealFlow_SD_File
   {
      public:
         RealFlow_SD_File();
         ~RealFlow_SD_File();

         struct rf_sd_header {
            char   file_id[30];   // File identifier
            float  version;       // Version (current = 6)

            int header_chk_size;  // header chunk size in bytes version >=5
            int frame_chk_size;   // frame chunk size in bytes version>= 5

            char   cam_data;      // Camera data present (0=no, 1=yes) version >= 2
            int    server;        // Server (1=LW, 2=MAX, 3=XSI, 4, 5=MAYA 6=CINEMA4D, 7=HOUDINI)

            int    internal_use_1;     // Internal use.
            int         internal_use_2;     // Internal use (month)
            int         internal_use_3;     // Internal use (hour).
            int         internal_use_4;     // Internal use (day).
            int         internal_use_5;     // Internal use (min).
            int         internal_use_6;     // Internal use (sec).

            int       num_objects;    // Number of objects
            int       beg_frame;      // Begin frame
            int       end_frame;      // End frame
         } myRF_SD_Header;

         struct rf_sd_obj_header {
            char    obj_mode;        // matrix mode (0) || vertex mode (1)
            float   obj_color[3];    // color of the object
            int     obj_name_len;    // Length of the object's name = L
            std::string    obj_name; // Object's name  (max length = 256 bytes)
            int     obj_tex_len;     // Length of the object's texture file path
            std::string    obj_tex_name; // Object's texture file path (max length = 1024 bytes)
            int     num_vertices;    // Object's number of vertices
            int     num_faces;       // Object's number of faces
         } myRF_SD_Obj_Header;

         struct rf_sd_face_data {
            float   vertex[3];          // Vertex coordinates
            int     vertex_idx[3];      // Vertex indices
            float   vertex_tex[3][3];   // Vertex texture coordinates
            int     visible[3];         // visibility flags
            int     mat_idx;            // material index
         } myRF_SD_Face_Data;

         struct sd_cam_header {
            float   cam_fov;
            float   cam_near;
            float   cam_far;
            float   cam_sky[3];      // Camera sky vector (version => 6)
         } myRF_SD_Cam_Header;

         struct rf_sd_obj_frame_header {
            int     obj_name_len;          // Length of the object's name = L
            std::string    obj_name;       // Object's name (max length = 256 bytes)
            float   obj_world_xform[12];   // 3X4 world transformation matrix
            float   obj_trans_vec[3];      // Object's translation vector
            float   obj_rot_vec[3];        // Object's rotation vector
            float   obj_scale_vec[3];      // Object's scale vector
            float   obj_pivot_pos[3];      // Object's pivot position
            double  obj_CG_pos[3];         // Object's CG position
            double  obj_CG_vel[3];         // Object's CG velocity
            double  obj_CG_rot[3];         // Object's CG angular rotation
         } myRF_SD_Obj_Frame_Header;

         struct rf_sd_cam_frame_data {
            float   cam_world_xform[16];    // Camera's world transformation matrix
            float   cam_world_pos[3];       // Camera's world transformation matrix
            float   cam_look_at_pos[3];     // Camera's look at position
            float   cam_up_vector[3];       // Camera's up vector
            float   cam_roll;               // Camera's roll (version => 6)
         } myRF_SD_Cam_Frame_Data;

         struct obj_detail_struct {
            long    num_points;
            long    num_faces;
            char    mode;
         } obj_detail[maxNumObjects];

         int openSDFile(int mode);
         int closeSDFile(int mode);

         int readSDHeader();
         int readSDObjHdr();
         int readSDFaceCoord();
         int readSDCamData();
         int readSDFaceIndex();
         int readSDFaceTexture();
         int readSDFaceVis();
         int readSDFaceMat();
         int readSDCamFrameData();
         int readSDCurrFrame(int & cur_frame);
         int readSDObjFrameHdr();

         int writeSDHeader();
         int writeSDObjHdr();
         int writeSDObjFrameHdr();
         int writeSDFaceCoord(float & x, float & y, float & z);
         int writeSDFaceIndex(int & vtx_index_num);
         int writeSDCurrFrame(int cur_frame);
         int writeSDCamData(int & camera_present);
         int writeSDCamFrameData();
         int writeSDFaceTexture();
         int writeSDFaceVis(int & visible);
         int writeSDFaceMat(int & material);

         std::string myFileName;
         std::ifstream SDifstream;
         std::ofstream SDofstream;
   };


}


#endif


/**********************************************************************************/
//  $Log: real_flow_sd.h,v $
//  Revision 1.3  2012-08-31 03:00:52  mstory
//  Continued H12 mods.
//
//  Revision 1.2  2011-02-11 04:18:02  mstory
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
