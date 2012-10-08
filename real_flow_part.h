/* ******************************************************************************
* Real Flow Particle Geometry File
*
* $RCSfile: real_flow_part.h,v $
*
* Description :  Interface for the RealFlow_Particle_File class
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_part.h,v $
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

#ifndef __real_flow_part_h__
#define __real_flow_part_h__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

namespace dca {

#ifndef RF_FILE_READ
#define RF_FILE_READ 0
#define RF_FILE_WRITE 1
#endif

#define RF_PART_HDR_SIZE sizeof(part_header)
#define RF_PART_DATA_SIZE sizeof(part_data)

class RF_Particle_Exception {
    std::string e_msg;

public:
    RF_Particle_Exception(std::string msg);

    void what() {
        std::cout << "RF_Particle_Exception: " << e_msg << std::endl;
    }
};


const int RF_PART_FLUID_NAME_SZ = 250;

class RealFlow_Particle_File {
public:
    RealFlow_Particle_File();
    ~RealFlow_Particle_File();

    // RealFlow particle file header structure
    struct rf_part_header {
        int         verify_code;        // verification code (0xFABADA)                   0
        char        fluid_name[RF_PART_FLUID_NAME_SZ];    // name of the fluid (string)   4
        short int   version;            // version (current = 11)                         254
        float       scene_scale;        // scene scale                                    256
        int         fluid_type;         // fluid type (gas, liquid..)                     260
        float       elapsed_time;       // elapsed simulation time                        264
        int         frame_number;       // frame number                                   268
        int         fps;                // frames per second                              272
        int         num_particles;      // number of particles                            276
        float       radius;             // radius                                         280
        float       pressure[3];        // pressure (max, min, average)                   284
        float       speed[3];           // speed  (max, min, average)                     296
        float       temperature[3];     // temperature  (max, min, average)               308
        float       emitter_pos[3];     // emitter position                               320
        float       emitter_rot[3];     // emitter rotation                               332
        float       emitter_scale[3];   // emitter scale                                  344
    } part_header;

    // RealFlow particle data structure
    struct rf_part_data {
        float       pos[3];             // (X,Y,Z) particle position                      356
        float       vel[3];             // (X,Y,Z) particle velocity                      368
        float       force[3];           // (X,Y,Z) particle force                         380
        float       vorticity[3];       // (X,Y,Z) particle vorticity                     392
        float       normal[3];          // (X,Y,Z) particle normal                        404
        int         num_neighbors;      // number of neighbors                            416
        float       texture_vector[3];  // texture vector                                 420
        short int   info_bits;          // "information bits"                             432
        float       age;                // elapsed particle time (age)                    434
        float       isolation_time;     // isolation time                                 438
        float       viscosity;          // particle viscosity                             442
        float       density;            // particle density                               446
        float       pressure;           // particle pressure                              450
        float       mass;               // particle mass                                  454
        float       temperature;        // particle temperature                           458
        int         id;                 // particle ID                                    462
    } part_data;

    int open_part_file(char *file_name, int mode);
    int read_part_file_header();
    int write_part_file_header();
    int read_part_data();
    int write_part_data();
    int read_additional_data();
    int write_additional_data();
    int close_part_file(int mode);

    char  *myFileName;
    std::ifstream RFPartifstream;
    std::ofstream RFPartofstream;

};

}

#endif

/**********************************************************************************/
//  $Log: real_flow_part.h,v $
//  Revision 1.3  2012-08-29 03:07:07  mstory
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
