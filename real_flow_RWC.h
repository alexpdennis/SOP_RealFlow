/* ******************************************************************************
* Real Flow RWC Geometry File
*
* $RCSfile: real_flow_RWC.h,v $
*
* Description : Interface for the RealFlow_RWC_File class
*
* $Revision: 1.2 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_RWC.h,v $
*
* $Author: mstory $
*
*
* See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2008
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */

#ifndef __real_flow_RWC_h__
#define __real_flow_RWC_h__

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

   class RF_RWC_Exception
   {
         std::string e_msg;

      public:
         RF_RWC_Exception(std::string msg);

         void what() {
            std::cout << "RF_RWC_Exception: " << e_msg << endl;
         }
   };


   class RealFlow_RWC_File
   {
      public:
         RealFlow_RWC_File();
         ~RealFlow_RWC_File();

         struct rf_RWC_header {
            unsigned int version;    // version = 3
            bool            use_magic_num;
            unsigned int ID_code;    // ID code = 0xFAFAFAFA
            double         RW_pos_X;
            double         RW_pos_Y;
            double         RW_pos_Z;
            double         RW_rot_X;
            double         RW_rot_Y;
            double         RW_rot_Z;
            long int        num_X_vtx;
            long int        num_Z_vtx;
         } RWC_header;

         struct rf_RWC_vertex_data {
            double   X;
            double   Y;
            double   Z;
         } RWC_vtx_data;

         struct rf_RWC_vel_data {
            double   X;
            double   Y;
            double   Z;
         } RWC_vel_data;

         int     openRWCFile(const char * file_name, int mode);
         int     readRWCFileHeader();
         int     readRWCData();
         int     writeRWCFileHeader();
         int     writeRWCData();
         int     closeRWCFile(int mode);

         std::ifstream RWCifstream;
         std::ofstream RWCofstream;
   };


}

#endif



/**********************************************************************************/
//  $Log: real_flow_RWC.h,v $
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
