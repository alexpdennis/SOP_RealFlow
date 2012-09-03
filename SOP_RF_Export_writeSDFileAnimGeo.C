/* ******************************************************************************
*   Real Flow
*
* $RCSfile: SOP_RF_Export_writeSDFileAnimGeo.C,v $
*
* Description : Write the Real Flow SD animated geometry to disk
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export_writeSDFileAnimGeo.C,v $
*
* $Author: mstory $
*
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

/* ******************************************************************************
*  Function Name :  writeSDFileAnimGeo()
*
*  Description : Write the Real Flow SD animated geometry
*
*  Input Arguments : OP_Context & context
*
*  Return Value : int return status
*
***************************************************************************** */
int SOP_RF_Export::writeSDFileAnimGeo(OP_Context & context, UT_Interrupt *boss)
{

#ifdef DEBUG
std::cout << "SOP_RF_Export::writeSDFileAnimGeo()  "  << std::endl;
#endif

    long int num_prim, numv;
    UT_Vector3 normal, p, uv_vec, clr_vec;
    UT_Vector4 pos;
    GEO_Primitive *prim;
    GEO_Vertex vtx;
    GEO_AttributeHandle attrHandle;
    UT_String  tex_fname_str;

    UT_Vector3 trans, rot, scale, up, shear, pivot;
    UT_XformOrder xformOrder(UT_XformOrder::SRT);

    UT_Vector3 CG_pos;
    UT_Vector3 CG_vel;
    UT_Vector3 CG_rot;

	GA_ROAttributeRef sd_CG_pos, sd_CG_vel, sd_CG_rot;

    UT_String current_input_path;
    OP_Node *current_input_node;
    UT_String my_cwd, my_full_path, my_parent_full_path, my_name;
    GU_Detail blank_gdp;
    UT_Matrix4 work_matrix;

    float now = context.getTime();


try {


	// For each frame in our animation ...
	for (int cur_frame = myStartFrame; cur_frame <= myEndFrame; cur_frame++) {

	    if (myStaticAnim && !(myStartFrame == myEndFrame)) {
         // Set the current frame
         context.setFrame((long) cur_frame);
         now = context.getTime();
	    }
       else {
         // Set the current frame
         context.setFrame((long) myStartFrame);
         now = context.getTime();
	    }

       // Write out progress to console
        std::cout << "Real Flow Export Scene File-current frame: " << cur_frame << "\tend frame: " << myEndFrame << std::endl;

#ifdef DEBUG
std::cout << "Writing frames to disk: myStartFrame: " << myStartFrame;
std::cout << "  myEndFrame: " << myEndFrame;
std::cout << "  cur_frame: " << cur_frame;
std::cout << "  now: " << now << std::endl;
#endif

	    // Write the frame number
	    if (myRFSDFile->writeSDCurrFrame(cur_frame))
         throw SOP_RF_Export_Exception(canNotWriteFrameNumber, exceptionError);

	    // For each object (input)
	    for (int current_obj = 0; current_obj < myNumInputs; current_obj++) {

         // Get the object's node
		   current_input_node = getInput(current_obj);
		   current_input_node->getFullPath(current_input_path);

#ifdef DEBUG
std::cout << "current_input_path: " << (const char *)current_input_path << std::endl;
#endif

		   // Lock this input while we're working with it.
		   if (lockInput(current_obj, context) >= UT_ERROR_ABORT)
            throw SOP_RF_Export_Exception(couldNotLockInputInWriteSDFile, exceptionError);

		   // Duplicate the geometry from this input
		   duplicateSource(current_obj, context, gdp);

         // Set the the object's name length and name
         myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name_len = objectNames(current_obj).length();
         myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name = objectNames(current_obj);

#ifdef DEBUG
std::cout << "myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name_len: " << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name_len << std::endl;
std::cout << "myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name: " << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name.c_str() << std::endl;
#endif

		   // Get object's transform and assign to the world transform matrix
		  UT_DMatrix4 current_input_xform;
		  if(!current_input_node->getWorldTransform(current_input_xform, context))
		    addTransformError(*current_input_node, "current_input_node");

		   current_input_xform.explode(xformOrder, rot, scale, trans, pivot);

		   // If the user wants to apply the "object" (/obj) level transform
		   if (myObjXform) {
		      doObjectXForm(current_input_xform);

		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[0] = trans.x();
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[1] = trans.y();
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[2] = trans.z();

		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[0] = rot.x();
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[1] = rot.y();
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[2] = rot.z();

		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[0] = scale.x();
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[1] = scale.y();
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[2] = scale.z();

		      //  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[0] = pivot.x();
		      //  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[1] = pivot.y();
		      //  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[2] = pivot.z();

		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[0] = 0;
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[1] = 0;
		      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[2] = 0;

#ifdef DEBUG
std::cout << "trans.x: " << trans.x() << "\ttrans.y: " << trans.y() << "\ttrans.z: " << trans.z() << std::endl;
std::cout << "rot.x: " << rot.x() * RAD2DEG << "\trot.y: " << rot.y() * RAD2DEG << "\trot.z: " << rot.z() * RAD2DEG << std::endl;
std::cout << "scale.x: " << scale.x() << "\tscale.y: " << scale.y() << "\tscale.z: " << scale.z() << std::endl;
std::cout << "pivot.x: " << pivot.x() << "\tpivot.y: " << pivot.y() << "\tpivot.z: " << pivot.z() << std::endl;
#endif


		      // Transform the geometry
//            gdp->transformPoints(current_input_xform);
		   }
         else {
		      for (int i = 0; i < 3; i++) {
			      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[i] = 0;
			      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[i]   = 0;
			      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[i] = 1;
			      myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[i] = 0;
		      }
		      current_input_xform.identity();
		      doObjectXForm(current_input_xform);
		   }

         // Look for CG attr's in the geo detail, is present, store them in the SD file
         sd_CG_pos = gdp->findIndexAttribute("sd_CG_pos", GA_ATTRIB_DETAIL);
         sd_CG_vel = gdp->findIndexAttribute("sd_CG_vel", GA_ATTRIB_DETAIL);
         sd_CG_rot = gdp->findIndexAttribute("sd_CG_rot", GA_ATTRIB_DETAIL);

         attrHandle = gdp->getDetailAttribute("sd_CG_pos");
         if (attrHandle.isAttributeValid()) {
            CG_pos = attrHandle.getV3();
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[0] = static_cast<float>(CG_pos.x());
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[1] = static_cast<float>(CG_pos.y());
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[2] = static_cast<float>(CG_pos.z());
         }
         else {
             for (int i = 0; i < 3; i++)
            myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[i] = 0;
         }

         attrHandle = gdp->getDetailAttribute("sd_CG_vel");
         if (attrHandle.isAttributeValid()) {
            CG_vel = attrHandle.getV3();
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[0] = static_cast<float>(CG_vel.x());
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[1] = static_cast<float>(CG_vel.y());
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[2] = static_cast<float>(CG_vel.z());
         }
         else {
             for (int i = 0; i < 3; i++)
               myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[i] = 0;
         }

         attrHandle = gdp->getDetailAttribute("sd_CG_rot");
         if (attrHandle.isAttributeValid()) {
            CG_rot = attrHandle.getV3();
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[0] = static_cast<float>(CG_rot.x());
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[1] = static_cast<float>(CG_rot.y());
             myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[2] = static_cast<float>(CG_rot.z());
         }
         else {
             for (int i = 0; i < 3; i++)
               myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[i] = 0;
         }


		if (myEchoData)
		    std::cout << std::endl << "Processing frame vertices:" << std::endl << "Time = " << now << std::endl
             << "Frame num: " << cur_frame << " Current Object: " << current_obj << std::endl << std::endl;

         // Write the SD file's object frame header
         if (myRFSDFile->writeSDObjFrameHdr()) {
            throw SOP_RF_Export_Exception(canNotWriteObjectFrameHeader, exceptionError);
         }


         // If vertex mode, go through geometry and write to disk
         if (myMode) {

            if (myEchoData)
               std::cout << std::endl <<  "Processing animated vertex coordinates" << std::endl << std::endl;

             num_prim = 0;
             GA_FOR_ALL_PRIMITIVES(gdp, prim) {

               if (boss->opInterrupt())
                  throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

               numv = prim->getVertexCount();

               // For each vertex of this polygon
               for (int i = 0; i < 3; i++) {
                  vtx = prim->getVertexElement(i);
                  pos = vtx.getPos();

                  // Write the face
                  if (myRFSDFile->writeSDFaceCoord(pos.x(), pos.y(), pos.z())) {
                     throw SOP_RF_Export_Exception(canNotWriteFaceCoordinates, exceptionError);
                  }
               }

            num_prim++;
            }
         }

		unlockInput(current_obj);

   }			// End-For each object (input)

	    /*
	     * Camera data isn't available at the SOP level
	     * if(camera_present) {
	     * myRFSDFile->writeSDCamData();
	     * }
	     */

   } // for (current frame ..)

}

catch (SOP_RF_Export_Exception e) {
   e.what();

   if(e.getSeverity() == exceptionWarning)
      addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
   else if(e.getSeverity() == exceptionError)
      addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

   boss->opEnd();
 	unlockInputs();

   if(myRFSDFile->SDofstream.is_open()) {
      // Close the RF SD file
      if(myRFSDFile->closeSDFile(RF_FILE_WRITE)) {
         addError(SOP_MESSAGE, "Can't close Real Flow SD file after SOP_RF_Export_Exception exception was thrown");
         return 1;
         }
   }
   return 1;
}


return 0;

}


/**********************************************************************************/
//  $Log: SOP_RF_Export_writeSDFileAnimGeo.C,v $
//  Revision 1.4  2012-08-31 03:00:52  mstory
//  Continued H12 mods.
//
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
