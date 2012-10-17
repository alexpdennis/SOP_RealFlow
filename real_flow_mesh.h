/* ******************************************************************************
* Real Flow Mesh Geometry File
*
* $RCSfile: real_flow_mesh.h,v $
*
* Description : Interface for the RealFlow_Mesh_File class
*
* $Revision: 1.2 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_mesh.h,v $
*
* $Author: mstory $
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

#ifndef __real_flow_mesh_h__
#define __real_flow_mesh_h__

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

#define MAX_NUM_FLUIDS 64

class RF_Mesh_Exception
{
    std::string e_msg;

public:
    RF_Mesh_Exception(std::string msg);

    void what() {
        std::cout << "RF_Mesh_Exception: " << e_msg << std::endl;
    }
};


class RealFlow_Mesh_File
{
public:
    RealFlow_Mesh_File();
    ~RealFlow_Mesh_File();

    struct rf_mesh_header {
        unsigned int    ID_code;    // ID code = 0xDADADADA
        unsigned int    version;    // version = 4
        unsigned int    code;       // geometry chunk code = 0xCCCCCCCC
    } mesh_header;

    struct rf_mesh_vertex_data {
        int     num_vertices;
        float   X;
        float   Y;
        float   Z;
    } mesh_vertex_data;

    struct rf_mesh_face_data {
        int num_faces;
        int vertex[3];
    } mesh_face_data;

    struct rf_mesh_texture_data {
        unsigned int    code;   // Texture chunk code = 0xCCCCCC00
        int     num_fluids;
        float   texture_weight[MAX_NUM_FLUIDS];
        float   U;
        float   V;
        float   W;
    } mesh_tex_data;

    struct rf_mesh_velocity_data {
        unsigned int    code;   // ID code = 0xCCCCCC11
        float   X;
        float   Y;
        float   Z;
    } mesh_vel_data;

    int     openMeshFile(const char * file_name, int mode);
    int     readMeshFileHeader(int * hdr_status);
    int     readMeshVertexData();
    int     readMeshNumFaces();
    int     readMeshFaceData();
    int     readMeshChunkCode();
    int     readMeshNumFluids();
    int     readMeshTextureData();
    int     readMeshVelocityData();
    int     readMeshFileEOF();
    int     writeMeshFileHeader();
    int     writeMeshVertexData();
    int     writeMeshNumFaces();
    int     writeMeshFaceData();
    int     writeMeshTextureChunkCode();
    int     writeMeshVelocityChunkCode();
    int     writeMeshNumFluids();
    int     writeMeshTextureData();
    int     writeMeshVelocityData();
    int     writeMeshFileEOF();
    int     closeMeshFile(int mode);
    std::ifstream RFMeshifstream;
    std::ofstream RFMeshofstream;
};

#endif

}



/**********************************************************************************/
//  $Log: real_flow_mesh.h,v $
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
