/*

    Real Flow Mesh File Dump Utility

*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story
*  Digital Cinema Arts (C) 2005
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*


    $Date: 2011-02-11 04:18:01 $
    $Header: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_mesh.C,v 1.2 2011-02-11 04:18:01 mstory Exp $
    $Name:  $
    $RCSfile: dump_rf_mesh.C,v $
    $State: Exp $
    $Source: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_mesh.C,v $
    $Revision: 1.2 $
    $Author: mstory $

*/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#define DEBUG

#include "real_flow_mesh.h"
#include "real_flow_mesh.C"

using namespace dca;


FILE *RFMeshFile;
RealFlow_Mesh_File *myRFMeshFile;
int myCurrPoint = 0;


void handleError(char *msg)
{
    std::cout << msg << std::endl << std::endl;
    exit(1);
}


// Read the mesh file
void read_mesh_file(char *myFileName)
{

    int i, hdr_status;
    int tex_flag, vel_flag, eof_flag = 0;


// Open the Real Flow mesh file
    if (myRFMeshFile->openMeshFile((char *)myFileName))
        handleError("Can't open Real Flow Mesh file for reading");

// Read the header
    if (myRFMeshFile->readMeshFileHeader(&hdr_status))
        if (hdr_status == -1)
            handleError("Not A Real Flow Mesh File!");


// Get the vertice point data
    for (i = 0; i < myRFMeshFile->mesh_vertex_data.num_vertices; i++) {

        myCurrPoint = i;

        // Read the mesh data from the file
        if (myRFMeshFile->readMeshVertexData())
            handleError("Can't read from Real Flow Mesh file: vertex data");
    }


// Read the the number of faces
    if (myRFMeshFile->readMeshNumFaces())
        handleError("Can't read Real Flow mesh file: num_faces");


// For each face, read the face data
    for (i = 0; i < myRFMeshFile->mesh_face_data.num_faces; i++) {

        myCurrPoint = i;

        // Read the mesh data from the file
        if (myRFMeshFile->readMeshFaceData())
            handleError("Can't read from Real Flow Mesh file: face data");

    } // for number of faces


// Read the next chunk code to see if texture data is present
    if (myRFMeshFile->readMeshTextureChunkCode())
        handleError("Can't read texture chunk data");

// if texture chunk present, read the data
    if ((myRFMeshFile->mesh_tex_data.code == 0xCCCCCC00)) {

        // Read the number of fluids contributing to this mesh
        if (myRFMeshFile->readMeshNumFluids())
            handleError("Can't read Real Flow mesh file: num_fluids");

        // For all the vertices in the geometry, get the texture data
        for (i = 0; i < myRFMeshFile->mesh_vertex_data.num_vertices; i++) {

            if (myRFMeshFile->readMeshTextureData())
                handleError("Can't read Real Flow mesh file: texture data");

        } // for each vertex

    } // if (texture chunk present)



// Read the next chunk code to see if velocity data is present
    if (myRFMeshFile->readMeshVelocityChunkCode())
        handleError("Can't read velocity chunk data");


// If the velocity chunk is present, read the velocity data
    if (myRFMeshFile->mesh_vel_data.code == 0xCCCCCC11) {

        // For all the vertices in the geometry, get the velocity data
        for (i = 0; i < myRFMeshFile->mesh_vertex_data.num_vertices; i++) {

            myCurrPoint = i;

            // Read the velocity data
            if (myRFMeshFile->readMeshVelocityData())
                handleError("Can't read Real Flow mesh file: velocity data");

        } // for each vertex
    } // if (velocity chunk present)


// Read the eof marker
    if (myRFMeshFile->readMeshFileEOF())
        handleError("Can't read Real Flow mesh file: eof marker");

// Close the RF mesh file
    if (myRFMeshFile->closeMeshFile())
        handleError("Can't close Real Flow mesh file");


    return;
}



int main(int argc, char *argv[])
{

    std::cout << "dump_rf_mesh -  ver. 0.05 -  Digital Cinema Arts (C) 2003" << std::endl;
    std::cout << "Dumping Real Flow Mesh File" << std::endl << std::endl;

    myRFMeshFile = new RealFlow_Mesh_File();

    read_mesh_file(argv[1]);

    delete(myRFMeshFile);

    exit(0);

}

/*

$Log: dump_rf_mesh.C,v $
Revision 1.2  2011-02-11 04:18:01  mstory
Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...


--mstory

Revision 1.1.1.1  2009-01-30 02:24:05  mstory
Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.




*/
