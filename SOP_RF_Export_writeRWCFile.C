/* ******************************************************************************
*  Real Flow RWC file exporter for Houdini
*
* $RCSfile: SOP_RF_Export_writeRWCFile.C,v $
*
*  Description : Write a Real Flow RWC file to disk from the incoming geometry
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export_writeRWCFile.C,v $
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


/* ******************************************************************************
*  Function Name :  writeRWCFile()
*
*  Description : Write the Real Flow RWC file(s)
*
*  Input Arguments : OP_Context &context
*
*  Return Value : enumErrorList error_num
*
***************************************************************************** */

OP_ERROR SOP_RF_Export::writeRWCFile(OP_Context & context)
{

   GEO_Point * ppt;
   UT_Interrupt * boss;
   GA_ROAttributeRef v_ref;

   UT_Vector3 v_vec;
// UT_Vector3 *vel;
// bool v_found = false;
   int cur_frame = 0;
   GEO_AttributeHandle attrHandleVelocity;
   UT_Vector4 pos;
   UT_String fileName, export_stat_str = "";
   long int frame_offset, rf_start_frame, rf_end_frame = 0;
   long int grid_num_X, grid_num_Z = 0;

   float now = context.getTime();
   long int save_frame = context.getFrame();

   // If this cook was not inititiated by the user pressing the
   // "Write the File" button (the display flag was set), do not write the file.
   if(!calledFromCallback) {
         if(lockInputs(context) >= UT_ERROR_ABORT)
            throw SOP_RF_Export_Exception(canNotLockInputsInWriteRWCFile, exceptionError);

         // Duplicate the geometry from the first input
         duplicateSource(0, context);
         unlockInputs();
         // Restore the frame
         context.setFrame((long) save_frame);
         return error();
      }


   try {

         // Reset the flag for the next button pressed event
         calledFromCallback = false;

         myEchoData = ECHO_CONSOLE(now);
         grid_num_X = RWC_NUM_X(now);
         grid_num_Z = RWC_NUM_Y(now);
         myStartFrame = int (myBeginEnd[0]);
         myEndFrame =   int (myBeginEnd[1]);

         // Determine frame range
         frame_offset = abs(int (myStartFrame));

         // If the start frame is less than zero
         if(int (myStartFrame) < 0) {
               rf_start_frame = 0;
               rf_end_frame = int (myEndFrame) + frame_offset + 1;
            }
         // If the start frame is greater than zero
         else {
               rf_start_frame = 0;
               rf_end_frame = int (myEndFrame - frame_offset);
            }


#ifdef DEBUG
         std::cout << "Begin/End " << myBeginEnd[0] << "\t" << myBeginEnd[1] << endl;
         std::cout << "myFileName " << myFileName << endl;
         std::cout << "rf_start_frame: " << rf_start_frame << endl;
         std::cout << "rf_end_frame: " << rf_end_frame << endl;
         std::cout << "frame_offset: " << frame_offset << endl;
#endif


         // Set the file type id
         myRFRWCFile->RWC_header.ID_code = 0xFAFAFAFA;
         // Set "use magic number"
         myRFRWCFile->RWC_header.use_magic_num = false;
         // Set version
         myRFRWCFile->RWC_header.version = 3;

         myRFRWCFile->RWC_header.RW_pos_X = 0.0;
         myRFRWCFile->RWC_header.RW_pos_Y = 0.0;
         myRFRWCFile->RWC_header.RW_pos_Z = 0.0;
         myRFRWCFile->RWC_header.RW_rot_X = 0.0;
         myRFRWCFile->RWC_header.RW_rot_Y = 0.0;
         myRFRWCFile->RWC_header.RW_rot_Z = 0.0;

         boss = UTgetInterrupt();
         boss->opStart("Exporting Geometry To RWC File");

         // Check to see that there hasn't been a critical error in cooking the SOP.
         if(error() < UT_ERROR_ABORT) {

               // For each frame in our animation ...
               for(cur_frame = myStartFrame; cur_frame <= myEndFrame; cur_frame++) {
                     // Set the current frame
                     context.setFrame((long) cur_frame);
                     // Get current time
                     now = context.getTime();;

                     // Evaluate the filename for this frame
                     FNAME(myFileName, now);

                     // Write progress of the write to the console
                     std::cout << "Real Flow Export RWC cache file-cur_frame: " << cur_frame << "\tend_frame: " << myEndFrame
                               << "\tFileName: " << (const char *) myFileName << std::endl;

                     if(lockInputs(context) >= UT_ERROR_ABORT)
                        throw SOP_RF_Export_Exception(canNotLockInputsInWriteRWCFile, exceptionError);

                     if(boss->opInterrupt())
                        throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                     // duplicate the incoming geometry
                     duplicateSource(0, context);


//   std::cout << "num prims: " << (long int) gdp->primitives ().entries () << endl;
//         FOR_MASK_PRIMITIVES(gdp, prim, GEOHULL) {
//            GEO_Hull *hull_prim = dynamic_cast<GEO_Hull *>(prim);
//            UT_ASSERT(hull_prim);
//
//            grid_num_X = hull_prim->getNumCols();
//            grid_num_Z = hull_prim->getNumRows();
//#ifdef DEBUG
//std::cout << "grid_num_X: " <<grid_num_X << " grid_num_Z: " << grid_num_Z << endl;
//#endif
//          bool is_nurb = ((prim->getPrimitiveId() & GEOTPSURF) != 0);
//        }


                     // Check to see if the velocity attribute is oresent
                     v_ref = gdp->findPointAttribute("v");

                     // Set the number of X & Y values
                     myRFRWCFile->RWC_header.num_X_vtx = grid_num_X;
                     myRFRWCFile->RWC_header.num_Z_vtx = grid_num_Z;

                     // Open the Real Flow RWC file for writing
                     if(myRFRWCFile->openRWCFile((const char *) myFileName, RF_FILE_WRITE))
                        throw SOP_RF_Export_Exception(canNotOpenRWCFileForWriting, exceptionError);


                     // Write the header to the file
                     if(myRFRWCFile->writeRWCFileHeader())
                        throw SOP_RF_Export_Exception(canNotWriteHeaderRWCFile, exceptionError);


//         long int point_num = 0;

                     // For all the points in the geomtery, write out position and velocity
                     GA_FOR_ALL_GPOINTS(gdp, ppt) {
                        if(boss->opInterrupt())
                           throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                        // Set the particle data structure
                        pos = ppt->getPos();
                        myRFRWCFile->RWC_vtx_data.X = pos.x();
                        myRFRWCFile->RWC_vtx_data.Y = pos.y();
                        myRFRWCFile->RWC_vtx_data.Z = pos.z();

// std::cout << "point num: " << point_num++ << " pos: " << myRFRWCFile->RWC_vtx_data.X << " "
//   << myRFRWCFile->RWC_vtx_data.Y << " " << myRFRWCFile->RWC_vtx_data.Z << endl;


                        /*
                        uv_vec = *vtx->castAttribData<UT_Vector3>(uv_off);
                        GEO_AttributeHandle   h = gdp->getVertexAttribute("uv");
                        if (h.isAttributeValid())
                        {
                            h.setElement(vtx);
                            uv_vec = h.getV3();
                        }
                        */
                        // If velocity attribute is present, get the values and set them in the RWC record
                        if(v_ref.isValid()) {
                              v_vec = ppt->getValue<UT_Vector3>(v_ref, 0);
                              myRFRWCFile->RWC_vel_data.X = static_cast<float>(v_vec.x());
                              myRFRWCFile->RWC_vel_data.Y = static_cast<float>(v_vec.y());
                              myRFRWCFile->RWC_vel_data.Z = static_cast<float>(v_vec.z());
                           }
                        // Else set the velocity to zero.
                        else {
                              myRFRWCFile->RWC_vel_data.X = 0.0;
                              myRFRWCFile->RWC_vel_data.Y = 0.0;
                              myRFRWCFile->RWC_vel_data.Z = 0.0;
                           }

                        /*
                                       if (v_found) {
                                          v_vec = attrHandleVelocity.getV3();
                                          myRFRWCFile->part_data.vel[0] = v_vec.x();
                                          myRFRWCFile->part_data.vel[1] = v_vec.y();
                                          myRFRWCFile->part_data.vel[2] = v_vec.z();
                                     }
                        */

                        // Write particle data to disk
                        if(myRFRWCFile->writeRWCData())
                           throw SOP_RF_Export_Exception(canNotWriteDataToRWCFile, exceptionError);

                     }


                     // We're done, close the file
                     if(myRFRWCFile->closeRWCFile(RF_FILE_WRITE))
                        throw SOP_RF_Export_Exception(canNotCloseTheRealFlowRWCFile, exceptionError);

                  }

               // We're done with this frame
               boss->opEnd();
               unlockInputs();

            } // for (cur_frame)

      }

   catch(SOP_RF_Export_Exception e) {
         e.what();

         if(e.getSeverity() == exceptionWarning)
            addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
         else
            if(e.getSeverity() == exceptionError)
               addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

         boss->opEnd();
         unlockInputs();
         context.setFrame((long) save_frame);

         if(myRFRWCFile->RWCofstream.is_open()) {
               // Close the RF RWC file
               if(myRFRWCFile->closeRWCFile(RF_FILE_WRITE)) {
                     addError(SOP_MESSAGE, "Can't close Real Flow RWC file after SOP_RF_Export_Exception exception was thrown");
                     return error();
                  }
            }
         return error();
      }


// Restore the frame
   context.setFrame((long) save_frame);

   return error();
}


/**********************************************************************************/
//  $Log: SOP_RF_Export_writeRWCFile.C,v $
//  Revision 1.3  2012-08-29 03:07:07  mstory
//  Initial changes for H12.
//
//  Revision 1.2  2011-02-11 04:18:00  mstory
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
