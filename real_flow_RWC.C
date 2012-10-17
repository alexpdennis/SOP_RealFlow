/* ******************************************************************************
* Real Flow RWC Geometry File
*
* $RCSfile: real_flow_RWC.C,v $
*
* Description : Implementation for the RealFlow_RWC_File class
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_RWC.C,v $
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

#ifndef __real_flow_RWC_C__
#define __real_flow_RWC_C__


namespace dca
{

/* ******************************************************************************
*  Function Name : RF_RWC_Exception()
*
*  Description : Exception class for the Real Flow RWC File Object
*
*
*  Input Arguments : std::string msg
*
*  Return Value : None
*
***************************************************************************** */

RF_RWC_Exception::RF_RWC_Exception(std::string msg)
{

    e_msg = msg;
};




/* ******************************************************************************
*  Function Name : RealFlow_RWC_File()
*
*  Description : Constructor for the Real Flow RWC File Object
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
RealFlow_RWC_File::RealFlow_RWC_File()
{

    RWC_header.version = 0;
    RWC_header.use_magic_num = false;
    RWC_header.ID_code = 0;
    RWC_header.num_X_vtx = 0;
    RWC_header.num_Z_vtx = 0;
    RWC_header.RW_pos_X = 0;
    RWC_header.RW_pos_Y = 0;
    RWC_header.RW_pos_Z = 0;
    RWC_header.RW_rot_X = 0;
    RWC_header.RW_rot_Y = 0;
    RWC_header.RW_rot_Z = 0;

    RWC_vtx_data.X = 0;
    RWC_vtx_data.Y = 0;
    RWC_vtx_data.Z = 0;

    RWC_vel_data.X = 0;
    RWC_vel_data.Y = 0;
    RWC_vel_data.Z = 0;
}



/* ******************************************************************************
*  Function Name : ~RealFlow_RWC_File()
*
*  Description : Destructor for the Real Flow RWC File Object
*
*
*  Input Arguments : None
*
*  Return Value : None
*
***************************************************************************** */
RealFlow_RWC_File::~RealFlow_RWC_File()
{

}



/* ******************************************************************************
*  Function Name : openRWCFile()
*
*  Description : Open the Real Flow RWC file
*
*
*  Input Arguments : const char *file_name, int mode
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_RWC_File::openRWCFile(const char * file_name, int mode)
{

#ifdef DEBUG
    std::cout << "openRWCFile(): Opening Real Flow RWC file: " << file_name << " mode: " << mode << std::endl;
#endif


    // Open the RWC file to read.
    if(mode == RF_FILE_READ) {

        try {
//       RWCifstream.exceptions (ifstream::failbit | ifstream::badbit);
//       RWCifstream.exceptions (ifstream::eofbit | ifstream::failbit | ifstream::badbit);
            RWCifstream.open((const char *) file_name, ios::in | ios::binary);
            if(RWCifstream.good()) {
#ifdef DEBUG
                std::cerr << "RealFlow_RWC_File::openRWCFile(): Opened Real Flow RWC file for reading" << std::endl;
#endif
                return 0;
            } else {
#ifdef DEBUG
                std::cerr << "RealFlow_RWC_File::openRWCFile(): Failed to open Real Flow RWC file for reading" << std::endl;
#endif
                return 1;
            }
        } catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_RWC_File::openRWCFile(): EXCEPTION: " << e.what() << std::endl;
//       RWCifstream.clear();
            return 1;
        }
    }

    // Open the RWC file for writing
    else {

        try {
//       RWCofstream.exceptions (ofstream::eofbit | ofstream::failbit | ofstream::badbit);
            RWCofstream.exceptions(ofstream::failbit | ofstream::badbit);
            RWCofstream.open((const char *) file_name, ios::out | ios::binary);
#ifdef DEBUG
            std::cerr << "RealFlow_RWC_File::openRWCFile(): Opened Real Flow RWC file for writing" << std::endl;
#endif
            return 0;
        } catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_RWC_File::openRWCFile(): EXCEPTION: " << e.what() << std::endl;
//       RWCofstream.clear();
            return 1;
        }

    }



#ifdef DEBUG
    std::cout << "RealFlow_RWC_File::openRWCFile(): Opened Real Flow RWC file: " << std::endl;
#endif

    return 0;
}




/* ******************************************************************************
*  Function Name : readRWCFileHeader()
*
*  Description : Read the Real Flow RWC file header
*
*
*  Input Arguments :
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_RWC_File::readRWCFileHeader()
{

#ifdef DEBUG
    std::cout << "RealFlow_RWC_File::readRWCFileHeader(): Reading Real Flow RWC file header" <<
              std::endl << std::endl;
#endif


    try {
        // Read the version number
        RWCifstream.read((char *) &RWC_header.version, sizeof(unsigned int));

        // If this is not a Real Flow RWC file version 2, return
        if(RWC_header.version != 3) {
            std::cerr << "readRWCFileHeader(): readRWCFileHeader(): Real Flow version number incorrect = "
                      << RWC_header.ID_code << std::endl;
            return 1;
        }

        // read the use RWC "magic number" flag
        RWCifstream.read((char *) &RWC_header.use_magic_num, sizeof(bool));

        // Read the RWC ID code
        RWCifstream.read((char *) &RWC_header.ID_code, sizeof(unsigned int));

        // If this is not a Real Flow RWC file ID code return
        if(RWC_header.use_magic_num == 0 && RWC_header.ID_code != 0xFAFAFAFA) {
            std::cerr << "RealFlow_RWC_File::readRWCFileHeader(): Real Flow RWC ID code incorrect = "
                      << std::hex << RWC_header.ID_code << std::dec << std::endl;
            return 1;
        }

        // Read the RW center position
        RWCifstream.read((char *) &RWC_header.RW_pos_Z, sizeof(double));
        RWCifstream.read((char *) &RWC_header.RW_pos_Y, sizeof(double));
        RWCifstream.read((char *) &RWC_header.RW_pos_X, sizeof(double));

        // Read the RW center rotation
        RWCifstream.read((char *) &RWC_header.RW_rot_Z, sizeof(double));
        RWCifstream.read((char *) &RWC_header.RW_rot_Y, sizeof(double));
        RWCifstream.read((char *) &RWC_header.RW_rot_X, sizeof(double));

        // Read the number of vertices in "Z"
        RWCifstream.read((char *) &RWC_header.num_Z_vtx, sizeof(long int));
        // Read the number of vertices in "X"
        RWCifstream.read((char *) &RWC_header.num_X_vtx, sizeof(long int));


#ifdef DEBUG
        std::cout << "version = " << RWC_header.version << std::endl;
        std::cout << "use_magic_num = " << RWC_header.use_magic_num << std::endl;
        std::cout << std::hex << "ID_code = " << RWC_header.ID_code << std::dec << std::endl;
        std::cout << "num_X_vtx = " << RWC_header.num_X_vtx << std::endl;
        std::cout << "num_Z_vtx = " << RWC_header.num_Z_vtx << std::endl << std::endl;
#endif

    } catch(std::ios_base::failure & e) {
        std::cerr << "RealFlow_RWC_File::readRWCFileHeader(): EXCEPTION: " << e.what() << std::endl;
//    RWCifstream.clear();
        RWCifstream.close();
        return 1;
    }

    return 0;
}



/* ******************************************************************************
*  Function Name : readRWCData()
*
*  Description : Read the Real Flow RWC file data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_RWC_File::readRWCData()
{

    if(!RWCifstream.eof()) {

        // Read the RWC vertex and velocity data
        try {
            RWCifstream.read((char *) &RWC_vtx_data.Z, sizeof(double));
            RWCifstream.read((char *) &RWC_vtx_data.Y, sizeof(double));
            RWCifstream.read((char *) &RWC_vtx_data.X, sizeof(double));
            RWCifstream.read((char *) &RWC_vel_data.Z, sizeof(double));
            RWCifstream.read((char *) &RWC_vel_data.Y, sizeof(double));
            RWCifstream.read((char *) &RWC_vel_data.X, sizeof(double));

#ifdef DEBUG
            std::cout << "RealFlow_RWC_File::readRWCData(): RWC_vertex_data = " <<
                      RWC_vtx_data.X << "\t" << RWC_vtx_data.Y << "\t" << RWC_vtx_data.Z << std::endl;
            std::cout << "RealFlow_RWC_File::readRWCData(): RWC_vel_data = " << RWC_vel_data.X
                      << "\t" << RWC_vel_data.Y << "\t" << RWC_vel_data.Z << std::endl;
#endif

        }

        catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_RWC_File::readRWCData(): EXCEPTION: " << e.what() << std::endl;
            RWCifstream.clear();
            std::cerr << "Closing file: " << std::endl;
            RWCifstream.close();
            return 1;
        }
    } else {

#ifdef DEBUG
        std::cout << "EOF reached, closing file" << std::endl;
#endif

//       RWCifstream.clear();
        RWCifstream.close();
        return 1;
    }

    return 0;
}




/* ******************************************************************************
*  Function Name : writeRWCFileHeader()
*
*  Description : Write the Real Flow RWC file header
*
*
*  Input Arguments :
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_RWC_File::writeRWCFileHeader()
{

#ifdef DEBUG
    std::cout << "RealFlow_RWC_File::writeRWCFileHeader(): Write Real Flow RWC file header" << std::endl << std::endl;
#endif

    try {
        // Write the version, magic number, ID code and X/Y resolution
        RWCofstream.write((const char *) &RWC_header.version, sizeof(unsigned int));
        RWCofstream.write((const char *) &RWC_header.use_magic_num, sizeof(bool));
        RWCofstream.write((const char *) &RWC_header.ID_code, sizeof(unsigned int));

        // Write the RW center position
        RWCofstream.write((const char *) &RWC_header.RW_pos_Z, sizeof(double));
        RWCofstream.write((const char *) &RWC_header.RW_pos_Y, sizeof(double));
        RWCofstream.write((const char *) &RWC_header.RW_pos_X, sizeof(double));

        // Write the RW center rotation
        RWCofstream.write((const char *) &RWC_header.RW_rot_Z, sizeof(double));
        RWCofstream.write((const char *) &RWC_header.RW_rot_Y, sizeof(double));
        RWCofstream.write((const char *) &RWC_header.RW_rot_X, sizeof(double));

        // Write the X & Z vertex nums
        RWCofstream.write((const char *) &RWC_header.num_Z_vtx, sizeof(long int));
        RWCofstream.write((const char *) &RWC_header.num_X_vtx, sizeof(long int));
    }

    catch(std::ios_base::failure & e) {
        std::cerr << "RealFlow_RWC_File::writeRWCFileHeader(): EXCEPTION: " << e.what() << std::endl;
//    RWCifstream.clear();
        RWCifstream.close();
        return 1;
    }


#ifdef DEBUG
    std::cout << "version = " << RWC_header.version << std::endl;
    std::cout << "use_magic_num = " << RWC_header.use_magic_num << std::endl;
    std::cout << std::hex << "ID_code = " << RWC_header.ID_code << std::dec << std::endl;
    std::cout << "num_X_vtx = " << RWC_header.num_X_vtx << std::endl;
    std::cout << "num_Z_vtx = " << RWC_header.num_Z_vtx << std::endl << std::endl;
#endif


    return 0;
}



/* ******************************************************************************
*  Function Name : writeRWCData()
*
*  Description : Write the Real Flow RWC file data
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_RWC_File::writeRWCData()
{

    try {
        // Write the RWC vertex and velocity data
        RWCofstream.write((const char *) &RWC_vtx_data.Z, sizeof(double));
        RWCofstream.write((const char *) &RWC_vtx_data.Y, sizeof(double));
        RWCofstream.write((const char *) &RWC_vtx_data.X, sizeof(double));
        RWCofstream.write((const char *) &RWC_vel_data.Z, sizeof(double));
        RWCofstream.write((const char *) &RWC_vel_data.Y, sizeof(double));
        RWCofstream.write((const char *) &RWC_vel_data.X, sizeof(double));
    }

    catch(std::ios_base::failure & e) {
        std::cerr << "RealFlow_RWC_File::writeRWCData(): EXCEPTION: " << e.what() << std::endl;
//      RWCifstream.clear();
        RWCifstream.close();
        return 1;
    }

#ifdef DEBUG
    std::cout << "RealFlow_RWC_File::writeRWCData(): RWC_vertex_data = " << RWC_vtx_data.X << "\t"
              << RWC_vtx_data.Y << "\t" << RWC_vtx_data.Z << "\t" << std::endl;
    std::cout << "RealFlow_RWC_File::writeRWCData(): RWC_vel_data = " << RWC_vel_data.X << "\t"
              << RWC_vel_data.Y << "\t" << RWC_vel_data.Z << std::endl;
#endif

    return 0;
}




/* ******************************************************************************
*  Function Name : closeRWCFile()
*
*  Description :  Close the Real Flow RWC file
*
*
*  Input Arguments : None
*
*  Return Value : int
*
***************************************************************************** */
int RealFlow_RWC_File::closeRWCFile(int mode)
{

#ifdef DEBUG
    std::cout << "RealFlow_RWC_File::closeRWCFile(): Closing Real Flow RWC file" << std::endl;
#endif

    if(mode == RF_FILE_READ) {

        try {
            RWCifstream.close();
#ifdef DEBUG
            std::cout << "RealFlow_RWC_File::closeRWCFile(): Closed Real Flow RWC input stream" << std::endl;
#endif

        } catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_RWC_File::closeRWCFile - EXCEPTION: " << e.what() << std::endl;
//         RWCifstream.clear();
            return 1;
        }
    }

    else {

        try {
            RWCofstream.close();
#ifdef DEBUG
            std::cout << "RealFlow_RWC_File::closeRWCFile(): Closed Real Flow RWC output stream" << std::endl;
#endif
        } catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_RWC_File::closeRWCFile - EXCEPTION: " << e.what() << std::endl;
//         RWCifstream.clear();
            return 1;
        }
    }


    return 0;

}


}


#endif



/**********************************************************************************/
//  $Log: real_flow_RWC.C,v $
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
