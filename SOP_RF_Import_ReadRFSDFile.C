/* ******************************************************************************
*  Read Real Flow SD File
*
* $RCSfile: SOP_RF_Import_ReadRFSDFile.C,v $
*
*  Description : Read Real Flow SD file and create it's geometry
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import_ReadRFSDFile.C,v $
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

#include "SOP_RF_Import_ReadRFSDFileRestGeo.C"
#include "SOP_RF_Import_ReadRFSDFileAnimGeo.C"


/* ******************************************************************************
*  Function Name : ReadRFSDFile()
*
*
*  Input Arguments : OP_Context &context
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR SOP_RF_Import::ReadRFSDFile(OP_Context & context)
{

   UT_Interrupt *  boss;
   int current_frame;
   int SDFileCurrentFrame;
   float now = context.getTime();

   // Get GUI parameter states
   myGUIState.t_sd_tex = SD_TEX(now);
   myGUIState.t_sd_cg = SD_CG(now);
   myGUIState.t_sd_obj_xform = SD_OBJ_XFORM(now);
   myGUIState.t_sd_cg_xform = SD_CG_XFORM(now);


#ifdef DEBUG
   std::cout << "myFileName:" << (const char *)myFileName << std::endl;
#endif


   try {

         // Check to see that there hasn't been a critical error in cooking the SOP.
         if(error() < UT_ERROR_ABORT) {

               boss = UTgetInterrupt();

               gdp->clearAndDestroy();

               // Start the interrupt server
               boss->opStart("Importing Real Flow SD File");

               // Open the RF SD File
               myRFSDFile->myFileName = myFileName;

               if(myRFSDFile->openSDFile(RF_FILE_READ))
                  throw SOP_RF_Import_Exception(canNotOpenRealFlowSDFileForReading, exceptionError);

               // Read the SD file header
               if(ReadRFSDReadHeader(now))
                  throw SOP_RF_Import_Exception(canNotReadTheSDFileHeader, exceptionError);


//#ifdef DEBUG
// INVARIANT:  file position must be equal to sizeof(rf_sd_header)
               {
                  long int current_file_pos = myRFSDFile->SDifstream.tellg();
                  UT_ASSERT(current_file_pos == sdHeaderSize);
               }
//#endif

               // get the current frame and seek to the correct frame data
               current_frame = context.getFrame();

#ifdef DEBUG
               std::cout << std::endl << "current frame: " << current_frame << " start_frame: " <<
                         myRFSDFile->myRF_SD_Header.beg_frame << " end_frame: " <<
                         myRFSDFile->myRF_SD_Header.end_frame << std::endl;
#endif

               // if the current frame is past last frame, throw exception
               if((current_frame > myRFSDFile->myRF_SD_Header.end_frame))
                  throw SOP_RF_Import_Exception(currentFrameGreaterThanEndFrame, exceptionWarning);

               // if the current frame is less than 1 (invalid), throw exception
               if(current_frame < 1)
                  throw SOP_RF_Import_Exception(currentFrameLessThenOne, exceptionWarning);

               // Add point attributes for texture cooridinates
               if(myGUIState.t_sd_tex) {
                     myAttributeRefs.sd_texture_0 = gdp->addFloatTuple(GA_ATTRIB_POINT, "sd_texture_0", 3);
                     myAttributeRefs.sd_texture_1 = gdp->addFloatTuple(GA_ATTRIB_POINT, "sd_texture_1", 3);
                     myAttributeRefs.sd_texture_2 = gdp->addFloatTuple(GA_ATTRIB_POINT, "sd_texture_2", 3);
                  }

               // Add detail attributes for Center of Gravity position, velocity & rotation
               if(myGUIState.t_sd_cg) {
                     myAttributeRefs.sd_CG_pos = gdp->addFloatTuple(GA_ATTRIB_POINT, "sd_CG_pos", 3);
                     myAttributeRefs.sd_CG_pos = gdp->addFloatTuple(GA_ATTRIB_POINT, "sd_CG_vel", 3);
                     myAttributeRefs.sd_CG_pos = gdp->addFloatTuple(GA_ATTRIB_POINT, "sd_CG_rot", 3);
                  }

               // Check for the maximum number of objects
               if(myRFSDFile->myRF_SD_Header.num_objects > maxNumObjects)
                  throw SOP_RF_Import_Exception(tooManyObjectsInSDFile, exceptionError);

               // Build the SD rest geometry
               if(ReadRFSDCreateRestGeo(boss))
                  throw SOP_RF_Import_Exception(canNotCreateSDRestGeometry, exceptionError);

               // if camera data is present, read it and set the camera
               if(myRFSDFile->myRF_SD_Header.cam_data) {

#ifdef DEBUG
                     std::cout << std::endl << "Reading camera header" << std::endl << std::endl;
#endif

                     if(myRFSDFile->readSDCamData())
                        throw SOP_RF_Import_Exception(canNotReadSDCameraData, exceptionError);

#ifdef DEBUG
                     std::cout << "Camera FOV: " << myRFSDFile->myRF_SD_Cam_Header.cam_fov << std::endl;
                     std::cout << "Camera clip near: " << myRFSDFile->myRF_SD_Cam_Header.cam_near << std::endl;
                     std::cout << "Camera clip far: " << myRFSDFile->myRF_SD_Cam_Header.cam_far << std::endl;
#endif

                  }

               // rewind to the beginning of the file
               myRFSDFile->SDifstream.seekg(0, ios::beg);

#ifdef DEBUG
               long int current_file_pos = myRFSDFile->SDifstream.tellg();
               std::cout << "current file position (rewind): " << current_file_pos << std::endl;
#endif

               // position the file pointer to the currect frame
               myRFSDFile->SDifstream.seekg(
                  (myRFSDFile->myRF_SD_Header.header_chk_size +
                   ((current_frame - 1) * myRFSDFile->myRF_SD_Header.frame_chk_size)), ios::beg);

#ifdef DEBUG
               {
// report where the current file pointer is ...
                  long int current_file_pos = myRFSDFile->SDifstream.tellg();
                  std::cout << "header chunk, frame chunk & seek position: "
                            << myRFSDFile->myRF_SD_Header.header_chk_size << "\t"
                            << myRFSDFile->myRF_SD_Header.frame_chk_size << "\t"
                            << (myRFSDFile->myRF_SD_Header.header_chk_size +
                                ((current_frame - 1) * myRFSDFile->myRF_SD_Header.frame_chk_size)) << std::endl;
                  std::cout << "current file position: " << current_file_pos << std::endl;
               }
#endif

//#ifdef DEBUG
// INVARIANT:  file position at frame 1 must be equal to rf_sd_header.header_chk_size
               {
                  if(current_frame == 1) {
                        long int current_file_pos = myRFSDFile->SDifstream.tellg();
                        UT_ASSERT(current_file_pos == (myRFSDFile->myRF_SD_Header.header_chk_size));
                     }
               }
//#endif

               // read the current frame number from the SD file
               if(myRFSDFile->readSDCurrFrame(SDFileCurrentFrame))
                  throw SOP_RF_Import_Exception(canNotReadSDCurrentFrame, exceptionError);

#ifdef DEBUG
               {
                  long int current_file_pos = myRFSDFile->SDifstream.tellg();
                  std::cout << "current file position: " << current_file_pos << std::endl;
               }
#endif

// INVARIANT:  current frame in Houdini must be equal to (RF SD frame + 1)
               {
                  UT_ASSERT(current_frame == (SDFileCurrentFrame + 1));
               }

               // read the current frame number from the SD file
               if(current_frame != (SDFileCurrentFrame + 1))
                  throw SOP_RF_Import_Exception(incorrectSDCurrentFrame, exceptionError);


#ifdef DEBUG
               std::cout << std::endl << "SD File Current Frame: " << SDFileCurrentFrame << std::endl;
#endif

               // Read the camera data
               if(myRFSDFile->myRF_SD_Header.cam_data) {
                     // Read the camera frame data
                     if(myRFSDFile->readSDCamFrameData())
                        throw SOP_RF_Import_Exception(canNotReadSDCameraFrameData, exceptionError);

#ifdef DEBUG
                     std::cout << "Camera Transform: " << std::endl;
                     for(int i = 0; i < 16; i++)
                        std::cout << myRFSDFile->myRF_SD_Cam_Frame_Data.cam_world_xform[i] << "\t";
                     std::cout << std::endl;
                     std::cout << "Camera World Position: " << std::endl;
                     for(int i = 0; i < 3; i++)
                        std::cout << myRFSDFile->myRF_SD_Cam_Frame_Data.cam_world_pos[i] << "\t";
                     std::cout << std::endl;
                     std::cout << "Camera Look At Position: " << std::endl;
                     for(int i = 0; i < 3; i++)
                        std::cout << myRFSDFile->myRF_SD_Cam_Frame_Data.cam_look_at_pos[i] << "\t";
                     std::cout << std::endl;
                     std::cout << "Camera Up Vector: " << std::endl;
                     for(int i = 0; i < 3; i++)
                        std::cout << myRFSDFile->myRF_SD_Cam_Frame_Data.cam_up_vector[i] << "\t";
                     std::cout << std::endl;
#endif

                  }//

               // Build the SD "animated" geometry
               if(ReadRFSDCreateAnimGeo(boss))
                  throw SOP_RF_Import_Exception(canNotCreateSDAnimatedGeometry, exceptionError);

               // Close the RF SD file
               if(myRFSDFile->closeSDFile(RF_FILE_READ))
                  throw SOP_RF_Import_Exception(canNotCloseRealFlowSDFile, exceptionError);

               // We're done
               boss->opEnd();
            }

      }

// Catch exceptions ...
   catch(SOP_RF_Import_Exception e) {
         e.what();

         if(e.getSeverity() == exceptionWarning)
            addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
         else
            if(e.getSeverity() == exceptionError)
               addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

         if(myRFSDFile->SDifstream.is_open()) {
               // Close the RF SD file
               if(myRFSDFile->closeSDFile(RF_FILE_READ)) {
                     addError(SOP_MESSAGE, "Can't close Real Flow SD file after SOP_RF_Import_Exception exception was thrown");
                     std::cerr << "Can't close Real Flow SD file after SOP_RF_Import_Exception exception was thrown" << std::endl;
                  }
            }

         boss->opEnd();
         return error();
      }

   return error();
}





/* ******************************************************************************
*  Function Name : ReadRFSDReadHeader()
*
*
*  Input Arguments : float now (current time)
*
*  Return Value : int (status)
*
***************************************************************************** */
int SOP_RF_Import::ReadRFSDReadHeader(float now)
{

   char GUI_str[128];

   try {

         // Read the SD file header
         if(myRFSDFile->readSDHeader())
            throw SOP_RF_Import_Exception(canNotReadTheSDFileHeader, exceptionError);


#ifdef DEBUG
         std::cout << "file id: ";
         for(int i = 0; i < 30; i++)
            std::cout << myRFSDFile->myRF_SD_Header.file_id[i];
         std::cout << std::endl;
         std::cout << "version: "  << myRFSDFile->myRF_SD_Header.version << std::endl;
         std::cout << "header_chk_size: " << myRFSDFile->myRF_SD_Header.header_chk_size << std::endl;
         std::cout << "frame_chk_size: " << myRFSDFile->myRF_SD_Header.frame_chk_size << std::endl;
         std::cout << "cam_data: " << (int)myRFSDFile->myRF_SD_Header.cam_data << std::endl;
         std::cout << "server: " << myRFSDFile->myRF_SD_Header.server << std::endl;
         std::cout << "internal_use_1: "  << myRFSDFile->myRF_SD_Header.internal_use_1 << std::endl;
         std::cout << "internal_use_2: " << myRFSDFile->myRF_SD_Header.internal_use_2 << std::endl;
         std::cout << "internal_use_3: " << myRFSDFile->myRF_SD_Header.internal_use_3 << std::endl;
         std::cout << "internal_use_4: " << myRFSDFile->myRF_SD_Header.internal_use_4 << std::endl;
         std::cout << "internal_use_5: " << myRFSDFile->myRF_SD_Header.internal_use_5 << std::endl;
         std::cout << "internal_use_6: "  << myRFSDFile->myRF_SD_Header.internal_use_6 << std::endl;
         std::cout << "num_objects: " << myRFSDFile->myRF_SD_Header.num_objects << std::endl;
         std::cout << "beg_frame: " << myRFSDFile->myRF_SD_Header.beg_frame << std::endl;
         std::cout << "end_frame: " << myRFSDFile->myRF_SD_Header.end_frame << std::endl;
#endif


         // Check for the correct version, only version 6 (RF4) or later is supported
         if(myRFSDFile->myRF_SD_Header.version < 6)
            throw SOP_RF_Import_Exception(canNotReadTheSDFileHeader, exceptionError);

         sprintf(GUI_str, "%s%1.0f", "SD File Version #", myRFSDFile->myRF_SD_Header.version);
         setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);

         sprintf(GUI_str, "%s%d", "Number of Objects: ", myRFSDFile->myRF_SD_Header.num_objects);
         setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_INFO1, 0, now);

         sprintf(GUI_str, "%s%d%s%d", "Begin Frame: ", myRFSDFile->myRF_SD_Header.beg_frame,
                 "   End Frame: ", myRFSDFile->myRF_SD_Header.end_frame);
         setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_INFO2, 0, now);

         std::string server_str("Undefined");

         // Server (1=LW, 2=MAX, 3=XSI, 4, 5=MAYA 6=CINEMA4D, 7=HOUDINI)
         switch(myRFSDFile->myRF_SD_Header.server) {

               case 1:
                  server_str = "Lightwave";
                  break;
               case 2:
                  server_str = "3D Max";
                  break;
               case 3:
                  server_str = "XSI";
                  break;
               case 4:
               case 5:
                  server_str = "Maya/Houdini";
                  break;
               case 6:
                  server_str = "Cinema 4D";
                  break;
               case 7:
                  server_str = "Houdini";
                  break;
            }

         // TODO: use std::strstream
         sprintf(GUI_str, "%s%d: %s%s", "Server: ", myRFSDFile->myRF_SD_Header.server, " ", server_str.c_str());
         setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_INFO3, 0, now);

      }

   catch(SOP_RF_Import_Exception e) {
         e.what();

         if(e.getSeverity() == exceptionWarning)
            addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
         else
            if(e.getSeverity() == exceptionError)
               addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);


         if(myRFSDFile->SDifstream.is_open()) {
               myRFSDFile->closeSDFile(RF_FILE_READ);
            }

         return 1;
      }

   return 0;
}




/**********************************************************************************/
//  $Log: SOP_RF_Import_ReadRFSDFile.C,v $
//  Revision 1.3  2012-08-31 03:00:52  mstory
//  Continued H12 mods.
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
