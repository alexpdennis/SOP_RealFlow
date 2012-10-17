/*
*
*  Real Flow File Dump Utility
*
*
*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story
*
*  Digital Cinema Arts (C) 2005

*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*

*/


#define DEBUG

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

using std;

#include "real_flow_sd.C"
#include "real_flow_sd.h"
//#include "real_flow_part.C"
//#include "real_flow_part.h"
//#include "real_flow_mesh.C"
//#include "real_flow_mesh.h"
//#include "real_flow_RWC.C"
//#include "real_flow_RWC.h"

using namespace dca;

//
// Main
//
int main(int argc, char * argv[])
{

// cout.precision(12);

    cout << "dump_rf -  ver. 0.05 -  Digital Cinema Arts (C) 2006" << endl;
    cout << "Dumping Real Flow File" << endl;

//RealFlow_Mesh_File *myRFMeshFile;
//myRFMeshFile = new RealFlow_Mesh_File();


    RealFlow_SD_File * mySDFile = new RealFlow_SD_File();
//RealFlow_Particle_File *myRFParticleFile = new RealFlow_Particle_File();

//read_sd_file(argv[1]);

    delete(mySDFile);
//delete(myRFParticleFile);
//delete(myRFMeshFile);

    exit(0);
}



/*

$Log:


*/
