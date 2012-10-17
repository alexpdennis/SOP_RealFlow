/*

    Real Flow Real Wave Cache Dump Utility

*  Version 1.1.0
*  Date: February 17, 2011
*  Author: Mark Story

*    Digital Cinema Arts (C) 2008
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*

    $Date: 2011-02-11 04:18:01 $
    $Header: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_rwc.C,v 1.2 2011-02-11 04:18:01 mstory Exp $
    $Name:  $
    $RCSfile: dump_rf_rwc.C,v $
    $State: Exp $
    $Source: /dca/cvsroot/houdini/SOP_RealFlow/dump_rf_rwc.C,v $
    $Revision: 1.2 $
    $Author: mstory $

*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// #define DEBUG

using namespace std;

#include "real_flow_RWC.h"
#include "real_flow_RWC.C"

using namespace dca;



void print_RWC_data(RealFlow_RWC_File * myRWCFile)
{

    std::cout << "RWC_vertex_data = " <<
              myRWCFile->RWC_vtx_data.X << "\t" << myRWCFile->RWC_vtx_data.Y << "\t" << myRWCFile->RWC_vtx_data.Z << std::endl;
    std::cout << "RWC_vel_data = " << myRWCFile->RWC_vel_data.X
              << "\t" << myRWCFile->RWC_vel_data.Y << "\t" << myRWCFile->RWC_vel_data.Z << std::endl;

}



// Dump the RWC file header
void dump_header(RealFlow_RWC_File * myRWCFile)
{
    myRWCFile->readRWCFileHeader();

    std::cout << "version = " << myRWCFile->RWC_header.version << std::endl;
    std::cout << "use_magic_num = " << ((myRWCFile->RWC_header.use_magic_num)?"true":"false") << std::endl;

    std::cout << std::hex << "ID_code = " << myRWCFile->RWC_header.ID_code << std::dec << std::endl;

    std::cout << "RW_pos_X = " << myRWCFile->RWC_header.RW_pos_X << std::endl;
    std::cout << "RW_pos_Y = " << myRWCFile->RWC_header.RW_pos_Y << std::endl;
    std::cout << "RW_pos_Z = " << myRWCFile->RWC_header.RW_pos_Z << std::endl;

    std::cout << "RW_rot_X = " << myRWCFile->RWC_header.RW_rot_X << std::endl;
    std::cout << "RW_rot_Y = " << myRWCFile->RWC_header.RW_rot_Y << std::endl;
    std::cout << "RW_rot_Z = " << myRWCFile->RWC_header.RW_rot_Z << std::endl;

    std::cout << "num_X_vtx = " << myRWCFile->RWC_header.num_X_vtx << std::endl;
    std::cout << "num_Z_vtx = " << myRWCFile->RWC_header.num_Z_vtx << std::endl << std::endl;

    return;
}




// Dump the RWC  data
void dump_data(RealFlow_RWC_File * myRWCFile)
{

    long int row_count = 0;

    for(long int i = 0; i < myRWCFile->RWC_header.num_X_vtx; i++) {
//   cout << "i: " << i << endl;
        std::cout << "RWC row num: " << row_count << std::endl;

        if(!(i%2))
            for(long int j = 0; j < myRWCFile->RWC_header.num_Z_vtx; j++) {
                myRWCFile->readRWCData();
                print_RWC_data(myRWCFile);
            }
        else
            for(long int k = 0; k < myRWCFile->RWC_header.num_Z_vtx + 1; k++) {
                myRWCFile->readRWCData();
                print_RWC_data(myRWCFile);
            }
        row_count++;
    }

    myRWCFile->readRWCData();
    print_RWC_data(myRWCFile);

    return;

}


// Read the RWC file
int read_RWC_file(char * file_name)
{
    int i;

    RealFlow_RWC_File * myRWCFile = new RealFlow_RWC_File();

    if(myRWCFile->openRWCFile(file_name, 0)) {
        cout << "Can't open Real Flow RWC file for reading: " << file_name << endl;
        return 1;
    }

// Dump the header data
    dump_header(myRWCFile);

// Dump the RWC data
    dump_data(myRWCFile);

// Close the file
    myRWCFile->closeRWCFile(0);


    delete(myRWCFile);

    return 0;
}



//
// Dump the contents of the RWC file
//
int main(int argc, char * argv[])
{

    cout << "dump_rf_rwc -  ver. 0.05 -  Digital Cinema Arts (C) 2008" << endl;
    cout << "Dumping Real Flow Real Wave Cache File" << endl << endl;

    if(read_RWC_file(argv[1]))
        exit(1);

    exit(0);
}


/*

$Log: dump_rf_rwc.C,v $
Revision 1.2  2011-02-11 04:18:01  mstory
Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...


--mstory

Revision 1.1.1.1  2009-01-30 02:24:05  mstory
Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.




*/
