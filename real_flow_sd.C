/* ******************************************************************************
* Real Flow SD  File
*
* $RCSfile: real_flow_sd.C,v $
*
* Description : Implementation for the RealFlow_SD_File class
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_sd.C,v $
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

#ifndef __real_flow_sd_C__
#define __real_flow_sd_C__

#include "real_flow_sd.h"

namespace dca {

/* ******************************************************************************
*  Function Name : RF_SD_Exception()
*
*  Description : Exception class for the Real Flow SD File Object
*
*
*  Input Arguments : std::string msg
*
*  Return Value : None
*
***************************************************************************** */

RF_SD_Exception::RF_SD_Exception(std::string msg) {

   std::cout << "RF_SD_Exception: in constructor ... " << std::endl;

   e_msg = msg;
};



/* ******************************************************************************
*  Function Name : RealFlow_SD_File()
*
*  Description : Constructor for the Real Flow SD File Object
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */

RealFlow_SD_File::RealFlow_SD_File() {

   strcat(myRF_SD_Header.file_id, "");
   myRF_SD_Header.version = 0;
   myRF_SD_Header.header_chk_size = 0;
   myRF_SD_Header.frame_chk_size = 0;
   myRF_SD_Header.cam_data = 0;
   myRF_SD_Header.server = 0;
   myRF_SD_Header.internal_use_1 = 0;
   myRF_SD_Header.internal_use_2 = 0;
   myRF_SD_Header.internal_use_3 = 0;
   myRF_SD_Header.internal_use_4 = 0;
   myRF_SD_Header.internal_use_5 = 0;
   myRF_SD_Header.internal_use_6 = 0;
   myRF_SD_Header.internal_use_6 = 0;
   myRF_SD_Header.num_objects = 0;
   myRF_SD_Header.beg_frame = 0;
   myRF_SD_Header.end_frame = 0;

   myRF_SD_Obj_Header.obj_mode = 0;
   for(int i=0; i <3; i++)
      myRF_SD_Obj_Header.obj_color[i] = 0;
   myRF_SD_Obj_Header.obj_name_len = 0;
   myRF_SD_Obj_Header.obj_name = "";
   myRF_SD_Obj_Header.obj_tex_len = 0;
   myRF_SD_Obj_Header.obj_tex_name = "";
   myRF_SD_Obj_Header.num_vertices = 0;
   myRF_SD_Obj_Header.num_faces = 0;

   for(int i=0; i < 3; i++) {
      myRF_SD_Face_Data.vertex[i] = 0;
      myRF_SD_Face_Data.vertex_idx[i] = 0;
      for(int j=0; j < 3; j++)
         myRF_SD_Face_Data.vertex_tex[i][j] = 0;
   }
   for(int i=0; i < 3; i++)
      myRF_SD_Face_Data.visible[i] = 0;
   myRF_SD_Face_Data.mat_idx = 0;

   myRF_SD_Cam_Header.cam_fov = 0;
   myRF_SD_Cam_Header.cam_near = 0;
   myRF_SD_Cam_Header.cam_far = 0;
   for(int i=0; i < 3; i++)
      myRF_SD_Cam_Header.cam_sky[i] = 0;

   myRF_SD_Obj_Frame_Header.obj_name_len = 0;
   myRF_SD_Obj_Frame_Header.obj_name = "";
   for(int i=0; i < 12; i++)
      myRF_SD_Obj_Frame_Header.obj_world_xform[i] = 0;
   for(int i=0; i < 3; i++) {
      myRF_SD_Obj_Frame_Header.obj_trans_vec[i] = 0;
      myRF_SD_Obj_Frame_Header.obj_rot_vec[i] = 0;
      myRF_SD_Obj_Frame_Header.obj_scale_vec[i] = 0;
      myRF_SD_Obj_Frame_Header.obj_pivot_pos[i] = 0;
      myRF_SD_Obj_Frame_Header.obj_CG_pos[i] = 0;
      myRF_SD_Obj_Frame_Header.obj_CG_vel[i] = 0;
      myRF_SD_Obj_Frame_Header.obj_CG_rot[i] = 0;
   }

   for(int i=0; i < 16; i++)
      myRF_SD_Cam_Frame_Data.cam_world_xform[i] = 0;
   for(int i=0; i < 3; i++) {
      myRF_SD_Cam_Frame_Data.cam_world_pos[i] = 0;
      myRF_SD_Cam_Frame_Data.cam_look_at_pos[i] = 0;
      myRF_SD_Cam_Frame_Data.cam_up_vector[i] = 0;
   }
   myRF_SD_Cam_Frame_Data.cam_roll = 0;

   for(int i=0; i < maxNumObjects; i++) {
      obj_detail[i].num_points = 0;
      obj_detail[i].num_faces = 0;
      obj_detail[i].mode = 0;
   }

}



/* ******************************************************************************
*  Function Name : ~RealFlow_SD_File()
*
*  Description : Destructor for the Real Flow SD File Object
*
*
*  Input Arguments : None
*
*  Return Value : int-status
*
***************************************************************************** */
RealFlow_SD_File::~RealFlow_SD_File() {

}



/* ******************************************************************************
*  Function Name : openSDFile()
*
*  Description : Open the Real Flow SD File Object
*
*
*  Input Arguments : int mode (RF_FILE_READ, RF_FILE_WRITE)
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_SD_File::openSDFile(int mode) {

    if(mode) {

      try {
         SDofstream.exceptions (std::ofstream::eofbit | std::ofstream::failbit | std::ofstream::badbit);
         SDofstream.open ((const char *)myFileName.c_str(), std::ios::out | std::ios::binary);
#ifdef DEBUG
std::cout << "RealFlow_SD_File::openSDFile(): Opened Real Flow SD file for writing" << std::endl;
#endif
      }

      catch (std::ios_base::failure& e) {
         std::cerr << "RealFlow_RWC_File::openSDFile(): EXCEPTION: " << e.what () << std::endl;
         SDofstream.clear();
         return 1;
		   }
    }
    else {
      try {
         SDifstream.exceptions (std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
         SDifstream.open ((const char *)myFileName.c_str(), std::ios::in | std::ios::binary);
#ifdef DEBUG
std::cout << "RealFlow_SD_File::openSDFile(): Opened Real Flow SD file for reading" << std::endl;
#endif
      }

      catch (std::ios_base::failure& e) {
         std::cerr << "RealFlow_SD_File::openSDFile(): EXCEPTION: " << e.what () << std::endl;
         SDifstream.clear();
         return 1;
		   }
    }

    return 0;
}



/* ******************************************************************************
*  Function Name : writeSDHeader()
*
*  Description : Write the SD File Header
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_SD_File::writeSDHeader()
{

	try {
		// Write the header
		SDofstream.write ((const char *) &myRF_SD_Header.file_id, 30);
		SDofstream.write ((const char *) &myRF_SD_Header.version, sizeof(myRF_SD_Header.version));
		SDofstream.write ((const char *) &myRF_SD_Header.header_chk_size, sizeof(myRF_SD_Header.header_chk_size));
		SDofstream.write ((const char *) &myRF_SD_Header.frame_chk_size, sizeof(myRF_SD_Header.frame_chk_size));
		SDofstream.write ((const char *) &myRF_SD_Header.cam_data, sizeof(myRF_SD_Header.cam_data));
		SDofstream.write ((const char *) &myRF_SD_Header.server, sizeof(myRF_SD_Header.server));

		SDofstream.write ((const char *) &myRF_SD_Header.internal_use_1, sizeof(myRF_SD_Header.internal_use_1));
		SDofstream.write ((const char *) &myRF_SD_Header.internal_use_2, sizeof(myRF_SD_Header.internal_use_2));
		SDofstream.write ((const char *) &myRF_SD_Header.internal_use_3, sizeof(myRF_SD_Header.internal_use_3));
		SDofstream.write ((const char *) &myRF_SD_Header.internal_use_4, sizeof(myRF_SD_Header.internal_use_4));
		SDofstream.write ((const char *) &myRF_SD_Header.internal_use_5, sizeof(myRF_SD_Header.internal_use_5));
		SDofstream.write ((const char *) &myRF_SD_Header.internal_use_6, sizeof(myRF_SD_Header.internal_use_6));

		SDofstream.write ((const char *) &myRF_SD_Header.num_objects, sizeof(myRF_SD_Header.num_objects));
		SDofstream.write ((const char *) &myRF_SD_Header.beg_frame, sizeof(myRF_SD_Header.beg_frame));
		SDofstream.write ((const char *) &myRF_SD_Header.end_frame, sizeof(myRF_SD_Header.end_frame));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDHeader(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : readSDHeader()
*
*  Description : Read the SD File Header
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_SD_File::readSDHeader()
{

	try {
		// Read the version number
//		SDifstream.read ((char *) &myRF_SD_Header, sizeof (myRF_SD_Header));

		SDifstream.read ((char *) &myRF_SD_Header.file_id, sizeof (myRF_SD_Header.file_id));
		SDifstream.read ((char *) &myRF_SD_Header.version, sizeof (myRF_SD_Header.version));
		SDifstream.read ((char *) &myRF_SD_Header.header_chk_size, sizeof (myRF_SD_Header.header_chk_size));
		SDifstream.read ((char *) &myRF_SD_Header.frame_chk_size, sizeof (myRF_SD_Header.frame_chk_size));
		SDifstream.read ((char *) &myRF_SD_Header.cam_data, sizeof (char));
		SDifstream.read ((char *) &myRF_SD_Header.server, sizeof (myRF_SD_Header.server));

		SDifstream.read ((char *) &myRF_SD_Header.internal_use_1, sizeof (myRF_SD_Header.internal_use_1));
		SDifstream.read ((char *) &myRF_SD_Header.internal_use_2, sizeof (myRF_SD_Header.internal_use_2));
		SDifstream.read ((char *) &myRF_SD_Header.internal_use_3, sizeof (myRF_SD_Header.internal_use_3));
		SDifstream.read ((char *) &myRF_SD_Header.internal_use_4, sizeof (myRF_SD_Header.internal_use_4));
		SDifstream.read ((char *) &myRF_SD_Header.internal_use_5, sizeof (myRF_SD_Header.internal_use_5));
		SDifstream.read ((char *) &myRF_SD_Header.internal_use_6, sizeof (myRF_SD_Header.internal_use_6));

      SDifstream.read ((char *) &myRF_SD_Header.num_objects, sizeof (myRF_SD_Header.num_objects));
      SDifstream.read ((char *) &myRF_SD_Header.beg_frame, sizeof (myRF_SD_Header.beg_frame));
      SDifstream.read ((char *) &myRF_SD_Header.end_frame, sizeof (myRF_SD_Header.end_frame));

#ifdef DEBUG
std::cout << "myRF_SD_Header.file_id = " << myRF_SD_Header.file_id << std::endl;
std::cout << "myRF_SD_Header.version = " << myRF_SD_Header.version << std::endl;
std::cout << "myRF_SD_Header.header_chk_size = " << myRF_SD_Header.header_chk_size << std::endl;
std::cout << "myRF_SD_Header.frame_chk_size = " << myRF_SD_Header.frame_chk_size << std::endl;
std::cout << "myRF_SD_Header.cam_data = " << static_cast<bool>(myRF_SD_Header.cam_data) << std::endl;
std::cout << "myRF_SD_Header.server = " << myRF_SD_Header.server << std::endl;

std::cout << "myRF_SD_Header.internal_use_1 = " << myRF_SD_Header.internal_use_1 << std::endl;
std::cout << "myRF_SD_Header.internal_use_2 = " << myRF_SD_Header.internal_use_2 << std::endl;
std::cout << "myRF_SD_Header.internal_use_3 = " << myRF_SD_Header.internal_use_3 << std::endl;
std::cout << "myRF_SD_Header.internal_use_4 = " << myRF_SD_Header.internal_use_4 << std::endl;
std::cout << "myRF_SD_Header.internal_use_5 = " << myRF_SD_Header.internal_use_5 << std::endl;
std::cout << "myRF_SD_Header.internal_use_6 = " << myRF_SD_Header.internal_use_6 << std::endl;

std::cout << "myRF_SD_Header.num_objects = " << myRF_SD_Header.num_objects << std::endl;
std::cout << "myRF_SD_Header.beg_frame = " << myRF_SD_Header.beg_frame << std::endl;
std::cout << "myRF_SD_Header.end_frame = " << myRF_SD_Header.end_frame << std::endl;
#endif

	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDHeader(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}


return 0;

}



/* ******************************************************************************
*  Function Name : writeSDObjHdr()
*
*  Description : Write the SD file object header
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDObjHdr()
{

#ifdef DEBUG
std::cout << "RealFlow_SD_File::writeSDObjHdr: myRF_SD_Obj_Header.obj_tex_len: "
   << myRF_SD_Obj_Header.obj_tex_len << std::endl;
std::cout << "RealFlow_SD_File::writeSDObjHdr: myRF_SD_Obj_Header.obj_tex_name: "
   << myRF_SD_Obj_Header.obj_tex_name << std::endl;
#endif

#ifdef DEBUG
std::cout << "RealFlow_SD_File::writeSDObjHdr: myRF_SD_Obj_Header.obj_mode: "
   << (int)myRF_SD_Obj_Header.obj_mode << std::endl;
#endif

	try {
		// Write the header
		SDofstream.write ((const char *) &myRF_SD_Obj_Header.obj_mode, sizeof(myRF_SD_Obj_Header.obj_mode));

		SDofstream.write ((const char *) &myRF_SD_Obj_Header.obj_color[0], sizeof(float));
		SDofstream.write ((const char *) &myRF_SD_Obj_Header.obj_color[1], sizeof(float));
		SDofstream.write ((const char *) &myRF_SD_Obj_Header.obj_color[2], sizeof(float));

		SDofstream.write ((const char *) &myRF_SD_Obj_Header.obj_name_len, sizeof(myRF_SD_Obj_Header.obj_name_len));
		SDofstream.write ((const char *) (const char *)myRF_SD_Obj_Header.obj_name.c_str(), myRF_SD_Obj_Header.obj_name_len);

		SDofstream.write ((const char *) &myRF_SD_Obj_Header.obj_tex_len, sizeof(myRF_SD_Obj_Header.obj_tex_len));
		SDofstream.write ((const char *) (const char *)myRF_SD_Obj_Header.obj_tex_name.c_str(), myRF_SD_Obj_Header.obj_tex_len);

		SDofstream.write ((const char *) &myRF_SD_Obj_Header.num_vertices, sizeof(myRF_SD_Obj_Header.num_vertices));
		SDofstream.write ((const char *) &myRF_SD_Obj_Header.num_faces, sizeof(myRF_SD_Obj_Header.num_faces));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDObjHdr(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}


return 0;
}



/* ******************************************************************************
*  Function Name : writeSDObjFrameHdr()
*
*  Description : Write the SD file object frame header
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDObjFrameHdr()
{

	try {
		// Write the header
		SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_name_len, sizeof(myRF_SD_Obj_Frame_Header.obj_name_len));
		SDofstream.write ((const char *) (const char *)myRF_SD_Obj_Frame_Header.obj_name.c_str(), myRF_SD_Obj_Frame_Header.obj_name_len);

      for(int i=0; i < 12; i++)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_world_xform[i], sizeof(float));

      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_trans_vec[i], sizeof(float));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_rot_vec[i], sizeof(float));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_scale_vec[i], sizeof(float));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_pivot_pos[i], sizeof(float));

      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_CG_pos[i], sizeof(double));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_CG_vel[i], sizeof(double));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Obj_Frame_Header.obj_CG_rot[i], sizeof(double));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDObjFrameHdr(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : writeSDFaceCoord()
*
*  Description : Read the particle data
*
*
*  Input Arguments : float xcord, float ycord, float zcord, bool myEchoData
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDFaceCoord(float& x, float& y, float& z)
{
 //   float x = xcord, y = ycord, z = zcord;

   try {
		SDofstream.write ((const char *) &z, sizeof(float));
		SDofstream.write ((const char *) &y, sizeof(float));
		SDofstream.write ((const char *) &x, sizeof(float));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDFaceCoord(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : writeSDFaceIndex()
*
*  Description : Write the SG file face index
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDFaceIndex(int& vtx_index_num)
{

   // write the vertex index
   try {
		SDofstream.write ((const char *) &vtx_index_num, sizeof(int));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDFaceIndex(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : writeSDCurrFrame()
*
*  Description : Write the SD file current frame
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDCurrFrame(int cur_frame)
{

   // Write the frame number
   try {
		SDofstream.write ((const char *) &cur_frame, sizeof(int));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDCurrFrame(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

   return 0;
}



/* ******************************************************************************
*  Function Name : writeSDCamData()
*
*  Description : Write the SD file camera data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDCamData(int& camera_present)
{
int i;

    if(camera_present) {

#ifdef DEBUG
std::cout << "doSingleFrame(): cam_fov = " << myRF_SD_Cam_Header.cam_fov << std::endl;
std::cout << "doSingleFrame(): cam_near = " << myRF_SD_Cam_Header.cam_near << std::endl;
std::cout << "doSingleFrame(): cam_far = " << myRF_SD_Cam_Header.cam_far << std::endl;

std::cout << "doSingleFrame(): cam_sky = ";
for(i=0; i<3; i++)
   std::cout << myRF_SD_Cam_Header.cam_sky[i];
std::cout << std::endl;
#endif

      try {
         SDofstream.write ((const char *) &myRF_SD_Cam_Header.cam_fov, sizeof(float));
         SDofstream.write ((const char *) &myRF_SD_Cam_Header.cam_near, sizeof(float));
         SDofstream.write ((const char *) &myRF_SD_Cam_Header.cam_far, sizeof(float));
         for(i=2; i>=0; i--)
            SDofstream.write ((const char *) &myRF_SD_Cam_Header.cam_sky[i], sizeof(float));
      }

      catch (std::ios_base::failure& e) {
         std::cerr << "RealFlow_SD_File::writeSDCamData(): EXCEPTION: " << e.what () << std::endl;
         SDofstream.clear();
         SDofstream.close ();
         return 1;
      }

    }

return 0;
}



/* ******************************************************************************
*  Function Name : writeSDCamFrameData()
*
*  Description : Write the SD file camera frame data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDCamFrameData()
{

#ifdef DEBUG
for(int i=0; i < 3; i++)
   std::cout << "Camera world position: " <<
      myRF_SD_Cam_Frame_Data.cam_world_pos[i] << std::endl;
#endif

   try {
      for(int i=15; i >= 0; i--)
         SDofstream.write ((const char *) &myRF_SD_Cam_Frame_Data.cam_world_xform[i], sizeof(float));

      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Cam_Frame_Data.cam_world_pos[i], sizeof(float));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Cam_Frame_Data.cam_look_at_pos[i], sizeof(float));
      for(int i=2; i>=0; i--)
         SDofstream.write ((const char *) &myRF_SD_Cam_Frame_Data.cam_up_vector[i], sizeof(float));

      SDofstream.write ((const char *) &myRF_SD_Cam_Frame_Data.cam_roll, sizeof(float));

	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDCamFrameData(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : writeSDFaceTexture()
*
*  Description : Write the SD file face texture data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDFaceTexture()
{

   // Write the UVW values
   try {
      for (int i=0; i < 3; i++)
        for (int j=0; j < 3; j++)
         SDofstream.write ((const char *) &myRF_SD_Face_Data.vertex_tex[i][j], sizeof(float));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDFaceTexture(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : writeSDFaceVis()
*
*  Description : Write the SD file face visibility data
*
*
*  Input Arguments : int visible
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDFaceVis(int& visible)
{

   try {
      for (int i=0; i < 3; i++)
         SDofstream.write ((const char *) &visible, sizeof(int));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDFaceVis(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}

   return 0;
}



/* ******************************************************************************
*  Function Name : writeSDFaceMat()
*
*  Description : Write the SD file face material data
*
*
*  Input Arguments : int &material
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::writeSDFaceMat(int& material)
{

   // Write the material index (not used)
   try {
         SDofstream.write ((const char *) &material, sizeof(int));
	}

	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::writeSDFaceMat(): EXCEPTION: " << e.what () << std::endl;
		SDofstream.clear();
		SDofstream.close ();
		return 1;
	}


return 0;
}



/* ******************************************************************************
*  Function Name : readSDObjHdr()
*
*  Description : Read the SD file object header
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDObjHdr()
{

#ifdef DEBUG
std::cout << "readSDObjHdr" << std::endl;
#endif

char string_data[1024];

	try {
		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_mode, sizeof (myRF_SD_Obj_Header.obj_mode));

		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_color[0], sizeof (myRF_SD_Obj_Header.obj_color[0]));
		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_color[1], sizeof (myRF_SD_Obj_Header.obj_color[1]));
		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_color[2], sizeof (myRF_SD_Obj_Header.obj_color[2]));

		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_name_len, sizeof (myRF_SD_Obj_Header.obj_name_len));
//		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_name, myRF_SD_Obj_Header.obj_name_len);
		SDifstream.read (string_data, myRF_SD_Obj_Header.obj_name_len);
      myRF_SD_Obj_Header.obj_name.assign(string_data);

		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_tex_len, sizeof (myRF_SD_Obj_Header.obj_tex_len));
//		SDifstream.read ((char *) &myRF_SD_Obj_Header.obj_tex_name, myRF_SD_Obj_Header.obj_tex_len);
		SDifstream.read (string_data, myRF_SD_Obj_Header.obj_tex_len);
      myRF_SD_Obj_Header.obj_tex_name.assign(string_data);

		SDifstream.read ((char *) &myRF_SD_Obj_Header.num_vertices, sizeof (myRF_SD_Obj_Header.num_vertices));
		SDifstream.read ((char *) &myRF_SD_Obj_Header.num_faces, sizeof (myRF_SD_Obj_Header.num_faces));

#ifdef DEBUG
std::cout << "myRF_SD_Obj_Header.obj_mode = " << static_cast<bool>(myRF_SD_Obj_Header.obj_mode) << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_color[0] = " << myRF_SD_Obj_Header.obj_color[0] << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_color[1] = " << myRF_SD_Obj_Header.obj_color[1] << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_color[2] = " << myRF_SD_Obj_Header.obj_color[2] << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_name_len = " << myRF_SD_Obj_Header.obj_name_len << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_name = " << myRF_SD_Obj_Header.obj_name << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_tex_len = " << myRF_SD_Obj_Header.obj_tex_len << std::endl;
std::cout << "myRF_SD_Obj_Header.obj_tex_name = " << myRF_SD_Obj_Header.obj_tex_name << std::endl;
std::cout << "myRF_SD_Obj_Header.num_vertices = " << myRF_SD_Obj_Header.num_vertices << std::endl << std::endl;
std::cout << "myRF_SD_Obj_Header.num_faces = " << myRF_SD_Obj_Header.num_faces << std::endl << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDObjHdr(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}


return 0;
}




/* ******************************************************************************
*  Function Name : readSDFaceCoord()
*
*  Description : Read the SD file face coordinates
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDFaceCoord()
{

	try {
		SDifstream.read ((char *) &myRF_SD_Face_Data.vertex[2], sizeof (myRF_SD_Face_Data.vertex[2]));
		SDifstream.read ((char *) &myRF_SD_Face_Data.vertex[1], sizeof (myRF_SD_Face_Data.vertex[1]));
		SDifstream.read ((char *) &myRF_SD_Face_Data.vertex[0], sizeof (myRF_SD_Face_Data.vertex[0]));

#ifdef DEBUG
std::cout << "myRF_SD_Face_Data.vertex[0] = " << myRF_SD_Face_Data.vertex[0] << std::endl;
std::cout << "myRF_SD_Face_Data.vertex[1] = " << myRF_SD_Face_Data.vertex[1] << std::endl;
std::cout << "myRF_SD_Face_Data.vertex[2] = " << myRF_SD_Face_Data.vertex[2] << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDFaceCoord(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}


return 0;
}



/* ******************************************************************************
*  Function Name : readSDFaceIndex()
*
*  Description : Read the SD file face index
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDFaceIndex()
{

	try {
		SDifstream.read ((char *) &myRF_SD_Face_Data.vertex_idx[0], sizeof (myRF_SD_Face_Data.vertex_idx[0]));
		SDifstream.read ((char *) &myRF_SD_Face_Data.vertex_idx[1], sizeof (myRF_SD_Face_Data.vertex_idx[1]));
		SDifstream.read ((char *) &myRF_SD_Face_Data.vertex_idx[2], sizeof (myRF_SD_Face_Data.vertex_idx[2]));

#ifdef DEBUG
std::cout << "myRF_SD_Face_Data.vertex_idx[0] = " << myRF_SD_Face_Data.vertex_idx[0] << std::endl;
std::cout << "myRF_SD_Face_Data.vertex_idx[1] = " << myRF_SD_Face_Data.vertex_idx[1] << std::endl;
std::cout << "myRF_SD_Face_Data.vertex_idx[2] = " << myRF_SD_Face_Data.vertex_idx[2] << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDFaceIndex(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}


return 0;
}




/* ******************************************************************************
*  Function Name : readSDFaceTexture()
*
*  Description : Read the SD file face texture data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDFaceTexture()
{
    // Read the UVW values
	try {

      // If this is a RF4 SD file, reverse the vectors
      for(int i=2; i >= 0; i--)
         for(int j=2; j >= 0; j--)
            //fread(&myRF_SD_Face_Data.vertex_tex[i][j], sizeof(float), 1, myRFSDFile);
            SDifstream.read ((char *) &myRF_SD_Face_Data.vertex_tex[i][j], sizeof (float));

#ifdef DEBUG
std::cout << "myRF_SD_Face_Data.vertex_tex[0] = " << myRF_SD_Face_Data.vertex_tex[0] << std::endl;
std::cout << "myRF_SD_Face_Data.vertex_tex[1] = " << myRF_SD_Face_Data.vertex_tex[1] << std::endl;
std::cout << "myRF_SD_Face_Data.vertex_tex[2] = " << myRF_SD_Face_Data.vertex_tex[2] << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDFaceTexture(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}


return 0;
}


/* ******************************************************************************
*  Function Name : readSDFaceVis()
*
*  Description : Read the SD file face visibility data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDFaceVis()
{

	try {
      for(int j=0; j < 3; j++)
		   SDifstream.read ((char *) &myRF_SD_Face_Data.visible[j], sizeof (int));

#ifdef DEBUG
std::cout << "myRF_SD_Face_Data.visible[0] = " << myRF_SD_Face_Data.vertex_tex[0] << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDFaceVis(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : readSDFaceMat()
*
*  Description : Read the SD file face material data
*
*
*  Input Arguments : None
*
*  Return Value : int-status
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDFaceMat()
{

	try {
		SDifstream.read ((char *) &myRF_SD_Face_Data.mat_idx, sizeof (myRF_SD_Face_Data.mat_idx));

#ifdef DEBUG
		std::cout << "myRF_SD_Face_Data.mat_idx = " << myRF_SD_Face_Data.mat_idx << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDFaceMat(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : readSDCamData()
*
*  Description : Read the SD file camera data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDCamData()
{

   try {
      SDifstream.read ((char *) &myRF_SD_Cam_Header.cam_fov, sizeof (myRF_SD_Cam_Header.cam_fov));
      SDifstream.read ((char *) &myRF_SD_Cam_Header.cam_near, sizeof (myRF_SD_Cam_Header.cam_near));
      SDifstream.read ((char *) &myRF_SD_Cam_Header.cam_far, sizeof (myRF_SD_Cam_Header.cam_far));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Cam_Header.cam_sky[i], sizeof (float));

#ifdef DEBUG
std::cout << "myRF_SD_Cam_Header.cam_fov = " << myRF_SD_Cam_Header.cam_fov << std::endl;
std::cout << "myRF_SD_Cam_Header.cam_near = " << myRF_SD_Cam_Header.cam_near << std::endl;
std::cout << "myRF_SD_Cam_Header.cam_far = " << myRF_SD_Cam_Header.cam_far << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDCamData(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}

return 0;
}



/* ******************************************************************************
*  Function Name : readSDCamFrameData()
*
*  Description : Read the SD file camera frame data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDCamFrameData()
{

	try {
      for(int i=15; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Cam_Frame_Data.cam_world_xform[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Cam_Frame_Data.cam_world_pos[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Cam_Frame_Data.cam_look_at_pos[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Cam_Frame_Data.cam_up_vector[i], sizeof (float));

      SDifstream.read ((char *) &myRF_SD_Cam_Frame_Data.cam_roll, sizeof (float));

#ifdef DEBUG
for(int i=15; i >= 0; i--)
		std::cout << "myRF_SD_Cam_Frame_Data.cam_world_xform[i] = " << myRF_SD_Cam_Frame_Data.cam_world_xform[i] << std::endl;
for(int i=2; i >= 0; i--) {
		std::cout << "myRF_SD_Cam_Frame_Data.cam_world_pos[i] = " << myRF_SD_Cam_Frame_Data.cam_world_pos[i] << std::endl;
		std::cout << "myRF_SD_Cam_Frame_Data.cam_look_at_pos[i] = " << myRF_SD_Cam_Frame_Data.cam_look_at_pos[i] << std::endl;
		std::cout << "myRF_SD_Cam_Frame_Data.cam_up_vector[i] = " << myRF_SD_Cam_Frame_Data.cam_up_vector[i] << std::endl;
}
std::cout << "myRF_SD_Cam_Frame_Data.cam_roll = " << myRF_SD_Cam_Frame_Data.cam_roll << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDCamFrameData(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}

return 0;
}




/* ******************************************************************************
*  Function Name : readSDCurrFrame()
*
*  Description : Read the SD file current frame data
*
*
*  Input Arguments : int &cur_frame
*
*  Return Value :
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDCurrFrame(int& cur_frame)
{

   int frame = 0;

	try {
		SDifstream.read ((char *) &frame, sizeof (int));

//      std::cout << "RealFlow_SD_File::readSDCurrFrame(): frame: " << frame << std::endl;

      cur_frame = frame;

#ifdef DEBUG
      std::cout << "RealFlow_SD_File::readSDCurrFrame(): cur_frame = " << cur_frame << std::endl;
#endif


	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDCurrFrame(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}


return 0;
}



/* ******************************************************************************
*  Function Name : readSDObjFrameHdr()
*
*  Description : Read the SD file frame header
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
inline int RealFlow_SD_File::readSDObjFrameHdr()
{

char string_data[1024];

	try {
		SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_name_len, sizeof (myRF_SD_Obj_Frame_Header.obj_name_len));
//		SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_name, myRF_SD_Obj_Frame_Header.obj_name_len);
		SDifstream.read ((char *) string_data, myRF_SD_Obj_Frame_Header.obj_name_len);
      myRF_SD_Obj_Frame_Header.obj_name.assign(string_data);

      for(int i=0; i < 12; i++)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_world_xform[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_trans_vec[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_rot_vec[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_scale_vec[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_pivot_pos[i], sizeof (float));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_CG_pos[i], sizeof (double));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_CG_vel[i], sizeof (double));
      for(int i=2; i >= 0; i--)
         SDifstream.read ((char *) &myRF_SD_Obj_Frame_Header.obj_CG_rot[i], sizeof (double));

#ifdef DEBUG
std::cout << "myRF_SD_Obj_Frame_Header.obj_name_len = " << myRF_SD_Obj_Frame_Header.obj_name_len << std::endl;
std::cout << "myRF_SD_Obj_Frame_Header.obj_name = " << myRF_SD_Obj_Frame_Header.obj_name << std::endl;

//std::cout << "myRF_SD_Obj_Frame_Header.obj_trans_vec[i] = " << myRF_SD_Obj_Frame_Header.obj_trans_vec[i] << std::endl;
//std::cout << "myRF_SD_Obj_Frame_Header.obj_rot_vec[i] = " << myRF_SD_Obj_Frame_Header.obj_rot_vec[i] << std::endl;
//std::cout << "myRF_SD_Obj_Frame_Header.obj_scale_vec[i] = " << myRF_SD_Obj_Frame_Header.obj_scale_vec[i] << std::endl;
//std::cout << "myRF_SD_Obj_Frame_Header.obj_pivot_pos[i] = " << myRF_SD_Obj_Frame_Header.obj_pivot_pos[i] << std::endl;
#endif

	}
	catch (std::ios_base::failure& e) {
		std::cerr << "RealFlow_SD_File::readSDObjFrameHdr(): EXCEPTION: " << e.what () << std::endl;
		SDifstream.clear();
		SDifstream.close ();
		return 1;
	}

    return 0;
}



/* ******************************************************************************
*  Function Name : closeSDFile()
*
*  Description : Close the SD file
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_SD_File::closeSDFile(int mode)
{

	if (mode == 0) {
      try {
         SDifstream.close ();

#ifdef DEBUG
cout << "RealFlow_SD_File::closeSDFile(): Closed Real Flow SD intput stream" << std::endl;
#endif

		}
		catch (std::ios_base::failure& e) {
         std::cerr << "RealFlow_SD_File::closeSDFile() - EXCEPTION: " << e.what () << std::endl;
         SDifstream.clear();
			return 1;
		}
	}

	else {
		try {
         SDofstream.close ();

#ifdef DEBUG
cout << "RealFlow_SD_File::closeSDFile(): Closed Real Flow SD output stream" << std::endl;
#endif

		}
		catch (std::ios_base::failure& e) {
			std::cerr << "RealFlow_SD_File::closeSDFile() - EXCEPTION: " << e.what () << std::endl;
         SDofstream.clear();
			return 1;
		}

	}


   return 0;
}



}


#endif

/*

$Log: real_flow_sd.C,v $
Revision 1.3  2012-08-31 03:00:52  mstory
Continued H12 mods.

Revision 1.2  2011-02-11 04:18:01  mstory
Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...


--mstory

Revision 1.1.1.1  2009-01-30 02:24:05  mstory
Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.



Revision 1.10  2004/03/03 21:10:41  mstory
Finished the modifications for exporting SD ver. 7 files, and importing SD
files.

--mstory

Revision 1.9  2004/02/18 22:47:25  mstory
.

Revision 1.8  2004/01/21 17:50:31  mstory
completed most of the geometry creation for the SD file import.
file seeking is now working too.

Revision 1.7  2004/01/15 12:56:13  mstory
more work on inporting/exporting SD files correctly ...


*/
