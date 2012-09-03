/*

    Real Flow Particle File Dump Utility

*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story

*    Digital Cinema Arts (C) 2004
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*

    $Date: 2012-08-29 03:07:07 $
    $Header: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_part.C,v 1.3 2012-08-29 03:07:07 mstory Exp $
    $Name:  $
    $RCSfile: dump_rf_part.C,v $
    $State: Exp $
    $Source: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_part.C,v $
    $Revision: 1.3 $
    $Author: mstory $

*/


// #define DEBUG

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "real_flow_part.h"
#include "real_flow_part.C"

using namespace dca;
using namespace std;


#define RF_FILE_READ 0

// Dump the particle file header
void dump_header(RealFlow_Particle_File * myRFParticleFile)
{

    printf("verify_code: \t%X\n", myRFParticleFile->part_header.verify_code);
    printf("fluid_name: \t%s\n", myRFParticleFile->part_header.fluid_name);
    std::cout << "version: " <<  myRFParticleFile->part_header.version << endl;
    printf("scene_scale: \t%f\n", myRFParticleFile->part_header.scene_scale);
    printf("fluid_type: \t%d\n", myRFParticleFile->part_header.fluid_type);
    printf("elapsed_time: \t%f\n",  myRFParticleFile->part_header.elapsed_time);
    printf("frame_number: \t%d\n", myRFParticleFile->part_header.frame_number);
    printf("fps: \t\t%d\n", myRFParticleFile->part_header.fps);
    printf("num_particles: \t%d\n", myRFParticleFile->part_header.num_particles);
    printf("radius: \t%f\n", myRFParticleFile->part_header.radius);

    printf("max press: \t%f\n", myRFParticleFile->part_header.pressure[0]);
    printf("min press: \t%f\n", myRFParticleFile->part_header.pressure[1]);
    printf("avg. press: \t%f\n", myRFParticleFile->part_header.pressure[2]);

    printf("max speed: \t%f\n", myRFParticleFile->part_header.speed[0]);
    printf("min speed: \t%f\n", myRFParticleFile->part_header.speed[1]);
    printf("avg. speed: \t%f\n", myRFParticleFile->part_header.speed[2]);

    printf("max temp: \t%f\n", myRFParticleFile->part_header.temperature[0]);
    printf("min temp: \t%f\n", myRFParticleFile->part_header.temperature[1]);
    printf("avg. temp: \t%f\n", myRFParticleFile->part_header.temperature[2]);

	printf("emitter position: \t%f\t%f\t%f\n",
	       myRFParticleFile->part_header.emitter_pos[0],
	       myRFParticleFile->part_header.emitter_pos[1],
	       myRFParticleFile->part_header.emitter_pos[2]);
	printf("emitter rotation: \t%f\t%f\t%f\n",
	       myRFParticleFile->part_header.emitter_rot[0],
	       myRFParticleFile->part_header.emitter_rot[1],
	       myRFParticleFile->part_header.emitter_rot[2]);
	printf("emitter scale: \t%f\t%f\t%f\n",
	       myRFParticleFile->part_header.emitter_scale[0],
	       myRFParticleFile->part_header.emitter_scale[1],
	       myRFParticleFile->part_header.emitter_scale[2]);

    return;
}


// Dump the particle data
void dump_data(RealFlow_Particle_File * myRFParticleFile, int i)
{

    printf("\n\n### Particle record num: %-8d ###\n", i);

    printf("particle position: \t%f\t%f\t%f\n",
	   myRFParticleFile->part_data.pos[0],
	   myRFParticleFile->part_data.pos[1],
	   myRFParticleFile->part_data.pos[2]);

    printf("particle velocity: \t%f\t%f\t%f\n",
	   myRFParticleFile->part_data.vel[0],
	   myRFParticleFile->part_data.vel[1],
	   myRFParticleFile->part_data.vel[2]);

    printf("particle force: \t%f\t%f\t%f\n",
	   myRFParticleFile->part_data.force[0],
	   myRFParticleFile->part_data.force[1],
	   myRFParticleFile->part_data.force[2]);

    printf("particle vorticity: \t%f\t%f\t%f\n",
	   myRFParticleFile->part_data.vorticity[0],
	   myRFParticleFile->part_data.vorticity[1],
	   myRFParticleFile->part_data.vorticity[2]);

    printf("particle normal: \t%f\t%f\t%f\n",
	   myRFParticleFile->part_data.normal[0],
	   myRFParticleFile->part_data.normal[1],
	   myRFParticleFile->part_data.normal[2]);

    printf("number of neighbors: \t\t%d\n", myRFParticleFile->part_data.num_neighbors);

    printf("texture vector: \t%f\t%f\t%f\n",
	   myRFParticleFile->part_data.texture_vector[0],
	   myRFParticleFile->part_data.texture_vector[1],
	   myRFParticleFile->part_data.texture_vector[2]);

    printf("information bits: \t\t%d\n", myRFParticleFile->part_data.info_bits);

    printf("particle age: \t\t%f\n", myRFParticleFile->part_data.age);
    printf("particle isolation time: \t%f\n", myRFParticleFile->part_data.isolation_time);
    printf("particle viscosity: \t\t%f\n",  myRFParticleFile->part_data.viscosity);
    printf("particle density: \t%f\n", myRFParticleFile->part_data.density);
    printf("particle pressure: \t%f\n", myRFParticleFile->part_data.pressure);
    printf("particle mass: \t\t%f\n", myRFParticleFile->part_data.mass);
    printf("particle temperature: \t\t%f\n", myRFParticleFile->part_data.temperature);
    printf("particle id: \t\t%d\n", myRFParticleFile->part_data.id);

    return;
}



// Read the particle file
int read_part_file(char *file_name)
{
    int i;

    RealFlow_Particle_File *myRFParticleFile = new RealFlow_Particle_File();

    if (myRFParticleFile->open_part_file(file_name, RF_FILE_READ)) {
      std::cerr << "Can't open Real Flow particle file for reading" << std::endl;
      return 1;
    }
    std::cout << "Opened Real Flow particle file: " << file_name << std::endl << std::endl;

    if (myRFParticleFile->read_part_file_header()) {
      std::cerr << "Can't read Real Flow particle file header" << std::endl;
      return 1;
    }

    dump_header(myRFParticleFile);

    for (i = 0; i < myRFParticleFile->part_header.num_particles; i++) {
      //  printf("\nreading particle data record\n");
      if (myRFParticleFile->read_part_data()) {
         std::cerr << "Can't read Real Flow particle data" << std::endl;;
         return 1;
      }
      dump_data(myRFParticleFile, i);
    }

   std::cout << std::endl;
//
//   std::cout << "Reading additional data record" << std::endl;
//   myRFParticleFile->read_additional_data();

   std::cout << "Closing Real Flow particle file" << std::endl;
   myRFParticleFile->close_part_file(RF_FILE_READ);

   delete(myRFParticleFile);

   return 0;
}


int main(int argc, char *argv[])
{

try {

   std::cout << "dump_rf_part -  ver. 0.05 -  Digital Cinema Arts (C) 2008" << std::endl;
   std::cout << "Dumping Real Flow Particle File" << endl << endl;

    if (read_part_file(argv[1]))
	      exit(1);

   }
catch(...) {

   std::cerr << "Unknown error ocurred, exiting program ..." << std::endl;
    exit(1);
   }

exit(0);
}


/*

$Log: dump_rf_part.C,v $
Revision 1.3  2012-08-29 03:07:07  mstory
Initial changes for H12.

Revision 1.2  2011-02-11 04:18:01  mstory
Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...


--mstory

Revision 1.1.1.1  2009-01-30 02:24:05  mstory
Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.




*/
