/*

    Real Flow Scene Data (SD) File Dump Utility

*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story

*    Digital Cinema Arts (C) 2005
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License. 
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*

    $Date: 2012-08-23 01:24:10 $
    $Header: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_sd.C,v 1.3 2012-08-23 01:24:10 mstory Exp $
    $Name:  $
    $RCSfile: dump_rf_sd.C,v $
    $State: Exp $
    $Source: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_sd.C,v $
    $Revision: 1.3 $
    $Author: mstory $

*/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>

// #define DEBUG 

#define RAD2DEG 180/3.141592654
#define DEG2RAD 3.141592654/180


#include "real_flow_sd.C"
#include "real_flow_sd.h"

using namespace dca;

RealFlow_SD_File *mySDFile;

void 	read_sd_file(char *file_name);
void	dump_header();
void	dump_objects();
void	dump_cam_header();
void	dump_frames();

#define	MAX_NUM_OBJECTS 128
int     vertices_array[maxNumObjects];



/* ******************************************************************************
*  Function Name : Main()
*
*  Description : 
*
*  Input Arguments : 
*
*  Output Arguments : 
*
*  Return Value :
*
***************************************************************************** */
int main(int argc, char *argv[])
{

// std::cout.precision(12);

std::cout << "dump_rf_sd -  ver. 0.07 -  Digital Cinema Arts (C) 2005" << std::endl;
std::cout << "Dumping Real Flow Scene Data File" << std::endl;

mySDFile = new RealFlow_SD_File();

read_sd_file(argv[1]);

delete(mySDFile);

exit(0);
}



/* ******************************************************************************
*  Function Name : read_sd_file()
*
*  Description : Read the sd file 
*
*  Input Arguments : char *file_name
*
*  Output Arguments : None
*
*  Return Value :
*
***************************************************************************** */
void read_sd_file(char *file_name) 
{

mySDFile->myFileName.clear();
mySDFile->myFileName.assign((const char *)file_name);

if(mySDFile->openSDFile(0)) {
    std::cout << "Error opening file" << std::endl;
    return;
}

    std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

dump_header();
dump_objects();
dump_cam_header();
dump_frames();

if(mySDFile->closeSDFile(0)) {
    std::cout << "Error closing file" << std::endl;
    return;
}

return;
}



/* ******************************************************************************
*  Function Name : dump_header()
*
*  Description : Dump the SD file header 
*
*  Input Arguments : None
*
*  Output Arguments : None
*
*  Return Value :
*
***************************************************************************** */
void dump_header() 
{
    int i;

    std::cout << std::endl << "Dumping header" << std::endl;

    mySDFile->readSDHeader();

std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

    std::cout << "file id: ";
    for(i=0; i < 30; i++) 
        std::cout << mySDFile->myRF_SD_Header.file_id[i];

    std::cout << std::endl;

    std::cout << "version: " << mySDFile->myRF_SD_Header.version << std::endl;

    if(mySDFile->myRF_SD_Header.version >= 5) {
        std::cout << "header_chk_size: " << 
            mySDFile->myRF_SD_Header.header_chk_size << std::endl;
        std::cout << "frame_chk_size: " << 
            mySDFile->myRF_SD_Header.frame_chk_size << std::endl;
    }
    std::cout << "cam_data: " << (int)mySDFile->myRF_SD_Header.cam_data << std::endl;
    std::cout << "server: " << mySDFile->myRF_SD_Header.server << std::endl;
    std::cout << "internal_use_1: " << mySDFile->myRF_SD_Header.internal_use_1 << std::endl;
    std::cout << "internal_use_2: " << mySDFile->myRF_SD_Header.internal_use_2 << std::endl;
    std::cout << "internal_use_3: " << mySDFile->myRF_SD_Header.internal_use_3 << std::endl;
    std::cout << "internal_use_4: " << mySDFile->myRF_SD_Header.internal_use_4 << std::endl;
    std::cout << "internal_use_5: " << mySDFile->myRF_SD_Header.internal_use_5 << std::endl;
    std::cout << "internal_use_6: " << mySDFile->myRF_SD_Header.internal_use_6 << std::endl;
    std::cout << "num_objects: " << mySDFile->myRF_SD_Header.num_objects << std::endl;
    std::cout << "beg_frame: " << mySDFile->myRF_SD_Header.beg_frame << std::endl;
    std::cout << "end_frame: " << mySDFile->myRF_SD_Header.end_frame << std::endl;

	return;
}



/* ******************************************************************************
*  Function Name : dump_objects()
*
*  Description : Read the SD file objects 
*
*  Input Arguments : None
*
*  Output Arguments : None
*
*  Return Value :
*
***************************************************************************** */

void dump_objects()
{
    int 		obj_num;
    int			i,j;

    std::cout << std::endl << "Dumping objects " << std::endl << std::endl;

    for(obj_num=0; obj_num < mySDFile->myRF_SD_Header.num_objects; obj_num++) {
        std::cout << std::endl << "Object Number: " << obj_num + 1 << std::endl;

        // Read the SD file obj header
        mySDFile->readSDObjHdr();
std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

        std::cout << std::endl << "Object Header: " << std::endl;

        if(mySDFile->myRF_SD_Obj_Header.obj_mode == 0) 
            std::cout << "obj_mode: matrix mode " << std::endl;
        else 
            std::cout << "obj_mode: vertex mode " << std::endl;

        std::cout << "obj_color: " << mySDFile->myRF_SD_Obj_Header.obj_color[0] << 
            "\t" << mySDFile->myRF_SD_Obj_Header.obj_color[1] << 
            "\t" << mySDFile->myRF_SD_Obj_Header.obj_color[2] << std::endl;
        std::cout << "obj_name_len: " << 
            mySDFile->myRF_SD_Obj_Header.obj_name_len << std::endl;

        std::cout << "obj_name: ";
        for(i=0; i < mySDFile->myRF_SD_Obj_Header.obj_name_len; i++)
            std::cout << mySDFile->myRF_SD_Obj_Header.obj_name[i];
        std::cout << std::endl;

        if(mySDFile->myRF_SD_Header.version >= 3) {
            std::cout << "obj_tex_len: " << 
                mySDFile->myRF_SD_Obj_Header.obj_tex_len << std::endl;

            std::cout << "obj_tex_name: ";
            for(i=0; i < mySDFile->myRF_SD_Obj_Header.obj_tex_len; i++)
                std::cout << mySDFile->myRF_SD_Obj_Header.obj_tex_name[i];
            std::cout << std::endl;
        }

        vertices_array[obj_num] = mySDFile->myRF_SD_Obj_Header.num_vertices;

        std::cout << "num_vertices: " << mySDFile->myRF_SD_Obj_Header.num_vertices << std::endl;
        std::cout << "num_faces " << mySDFile->myRF_SD_Obj_Header.num_faces << std::endl;

        std::cout << std::endl << "Vertex Data: " << std::endl;

        // For each vertex, read it's coordinates	
        for(i=0; i < mySDFile->myRF_SD_Obj_Header.num_vertices; i++) {

            // Read the face coordinates
            mySDFile->readSDFaceCoord();
//std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

            std::cout << "X Y Z " << "\t" << mySDFile->myRF_SD_Face_Data.vertex[0] 
                << "\t" << mySDFile->myRF_SD_Face_Data.vertex[1] 
                << "\t" << mySDFile->myRF_SD_Face_Data.vertex[2] << std::endl;
        }


        std::cout << std::endl << "Face Data: " << std::endl;

        // For each face, read it's index 
        for(int cur_face=0; cur_face < mySDFile->myRF_SD_Obj_Header.num_faces; cur_face++) {

                // Read the face vertex index
                mySDFile->readSDFaceIndex();
//std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

                std::cout << std::endl << "face number: " << cur_face + 1 << " of " 
                    << mySDFile->myRF_SD_Obj_Header.num_faces << " faces " 
                    << std::endl;

                std::cout << "vertex indices:  " << "\t" 
                    << mySDFile->myRF_SD_Face_Data.vertex_idx[0] << "\t" 
                    << mySDFile->myRF_SD_Face_Data.vertex_idx[1] << "\t" 
                    << mySDFile->myRF_SD_Face_Data.vertex_idx[2] << std::endl;

                // Read the UVW values (version >= 3)
                if(mySDFile->myRF_SD_Header.version >= 3) {
                    mySDFile->readSDFaceTexture();
//std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

                    for(i = 0; i < 3; i++) {
                        std::cout << "vertex texture coords: "; 
                        for(j=0; j < 3; j++)
                            std::cout << mySDFile->myRF_SD_Face_Data.vertex_tex[i][j] 
                            << "\t";
                            std::cout << std::endl;
                    }
                }

                // Read the visibility values
                mySDFile->readSDFaceVis();
//std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

                std::cout << "vertex visibility flags: ";
                for(i = 0; i < 3; i++)
                    std::cout << mySDFile->myRF_SD_Face_Data.visible[i] << " "; 
                std::cout << std::endl;

                // Read  the material index (not used)
                mySDFile->readSDFaceMat();
//std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

                std::cout << "face material indices: "; 
                for(i = 0; i < 3; i++)
                    std::cout << mySDFile->myRF_SD_Face_Data.mat_idx << " "; 
                std::cout << std::endl;
            }
    }

return;
}


/* ******************************************************************************
*  Function Name : dump_cam_header()
*
*  Description : Read the sd file camera header
*
*  Input Arguments : None
*
*  Output Arguments : None
*
*  Return Value :
*
***************************************************************************** */
void dump_cam_header()
{

    if(mySDFile->myRF_SD_Header.cam_data) {
        std::cout << std::endl << "Dumping camera header" << std::endl << std::endl;

        mySDFile->readSDCamData();

std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

        std::cout << "Camera FOV: " << mySDFile->myRF_SD_Cam_Header.cam_fov << std::endl;
        std::cout << "Camera clip near: " 
            << mySDFile->myRF_SD_Cam_Header.cam_near << std::endl;
        std::cout << "Camera clip far: " 
            << mySDFile->myRF_SD_Cam_Header.cam_far << std::endl;
    }

return;
}



/* ******************************************************************************
*  Function Name : dump_frames()
*
*  Description : Read the sd file frame data 
*
*  Input Arguments : None
*
*  Output Arguments : None
*
*  Return Value :
*
***************************************************************************** */
void dump_frames()
{
    int i, cur_frame = 0, obj_num, CurrentFrame;

    if(mySDFile->myRF_SD_Header.end_frame >= mySDFile->myRF_SD_Header.beg_frame) {

        std::cout << std::endl << std::endl << "Dumping frames" << std::endl;

        for(cur_frame = mySDFile->myRF_SD_Header.beg_frame; 
           cur_frame <= (mySDFile->myRF_SD_Header.end_frame - mySDFile->myRF_SD_Header.beg_frame); 
           cur_frame ++) 
        {

            std::cout << std::endl << "Dumping frame num: " << cur_frame << std::endl;

            mySDFile->readSDCurrFrame(CurrentFrame);
std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

            std::cout << std::endl << "Current SD File frame number: " << CurrentFrame << std::endl;

            for(obj_num = 0; obj_num < mySDFile->myRF_SD_Header.num_objects; obj_num++) {

               std::cout << std::endl << "Object Number: " << obj_num + 1 << std::endl;

               mySDFile->readSDObjFrameHdr();
std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

               if(mySDFile->myRF_SD_Header.version >= 5) {
                  std::cout << "obj_name_len: " << mySDFile->myRF_SD_Obj_Frame_Header.obj_name_len << std::endl;

                  std::cout << "obj_name: ";
                  for(i=0; i < mySDFile->myRF_SD_Obj_Frame_Header.obj_name_len; i++)
                        std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_name[i];
                  std::cout << std::endl;
               }

               std::cout << "Object Transform:" << std::endl;
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[0] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[1] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[2] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[3] << "\t";
               std::cout << std::endl;
            
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[4] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[5] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[6] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[7] << "\t";
               std::cout << std::endl;
            
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[8] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[9] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[10] << "\t";
               std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[11] << "\t";
               std::cout << std::endl;
            
               std::cout << std::endl;
            
               std::cout << "Translation Vector: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[i] << "\t";
               std::cout << std::endl;

               std::cout << "Rotation Vector: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[i] * RAD2DEG << "\t";
               std::cout << std::endl;

               std::cout << "Scale Vector: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[i] << "\t";
               std::cout << std::endl;

               std::cout << "Pivot Position: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[i] << "\t";
               std::cout << std::endl << std::endl;


               // Dump CG information (version >=4)	
               if(mySDFile->myRF_SD_Header.version >= 4) {

               std::cout << "CG Position: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[i] << "\t";
               std::cout << std::endl;

               std::cout << "CG Velocity: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[i] << "\t";
               std::cout << std::endl;

               std::cout << "CG Rotation: " << std::endl;
               for(i=0; i < 3; i++)
                  std::cout << mySDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[i] << "\t";
               std::cout << std::endl << std::endl;
             }


             // If vertex mode, dump coordinates
             if(mySDFile->myRF_SD_Obj_Header.obj_mode) {

               for(i=0; i < vertices_array[obj_num]; i++) {

                  std::cout << "vertex  number: " << i + 1 << " of " << vertices_array[obj_num] << " vertices " << std::endl;

                 // Read the face coordinates
                 mySDFile->readSDFaceCoord();

//std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

                 std::cout << "Vertex Coordinates: " << std::endl;
                 std::cout << mySDFile->myRF_SD_Face_Data.vertex[0] << "\t";
                 std::cout << mySDFile->myRF_SD_Face_Data.vertex[1] << "\t";
                 std::cout << mySDFile->myRF_SD_Face_Data.vertex[2] << "\t";
                 std::cout << std::endl;
               }
            }
         }

            // Read the camera data
            if(mySDFile->myRF_SD_Header.cam_data) {

                mySDFile->readSDCamFrameData();
std::cout << "->>>>>> file pos: " << mySDFile->SDifstream.tellg() << std::endl;

                std::cout << "Camera Transform: " << std::endl;
                for(i= 0; i < 16; i++)
                    std::cout << mySDFile->myRF_SD_Cam_Frame_Data.cam_world_xform[i] << "\t";
                std::cout << std::endl;

                std::cout << "Camera World Position: " << std::endl;
                for(i= 0; i < 3; i++)
                    std::cout << mySDFile->myRF_SD_Cam_Frame_Data.cam_world_pos[i] << "\t";
                std::cout << std::endl;

                std::cout << "Camera Look At Position: " << std::endl;
                for(i= 0; i < 3; i++)
                    std::cout << mySDFile->myRF_SD_Cam_Frame_Data.cam_look_at_pos[i] << "\t";
                std::cout << std::endl;

                std::cout << "Camera Up Vector: " << std::endl;
                for(i= 0; i < 3; i++)
                    std::cout << mySDFile->myRF_SD_Cam_Frame_Data.cam_up_vector[i] << "\t";
                std::cout << std::endl;

            }
        }
    }

return;
}



/*

$Log: dump_rf_sd.C,v $
Revision 1.3  2012-08-23 01:24:10  mstory
.

Revision 1.2  2011-02-11 04:18:01  mstory
Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...


--mstory

Revision 1.1.1.1  2009-01-30 02:24:05  mstory
Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.



Revision 1.8  2004/03/03 21:10:40  mstory
Finished the modifications for exporting SD ver. 7 files, and importing SD
files.

--mstory

Revision 1.7  2004/01/21 17:50:31  mstory
completed most of the geometry creation for the SD file import.
file seeking is now working too.

Revision 1.6  2004/01/15 12:56:13  mstory
more work on inporting/exporting SD files correctly ...


*/
