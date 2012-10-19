/* ******************************************************************************
*  Real Flow Mesh file exporter for Houdini
*
* $RCSfile: SOP_RF_Export_writeMeshFile.C,v $
*
*  Description : Write a Real Flow Mesh file to disk from the incoming geometry
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export_writeMeshFile.C,v $
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
*  Function Name :  writeMeshFile()
*
*  Description : Write the Real Flow Mesh file(s)
*
*  Input Arguments : OP_Context &context
*
*  Return Value : enumErrorList error_num
*
***************************************************************************** */

OP_ERROR SOP_RF_Export::writeMeshFile(OP_Context & context)
{

   UT_Interrupt * boss;
   GA_ROAttributeRef v_ref;
   UT_Vector4 pos;
   GEO_Point * ppt;
   GEO_Vertex vtx;
   UT_Vector3 v_vec;
   GEO_Primitive * prim;
// UT_Vector3 *vel;
// bool v_found = false;
   int cur_frame = 0;
   GEO_AttributeHandle attrHandleVelocity;
   UT_String fileName, export_stat_str = "";
   long int frame_offset, rf_start_frame, rf_end_frame = 0;

   float now = context.getTime();
   long int save_frame = context.getFrame();

   // If this cook was not inititiated by the user pressing the
   // "Write the File" button (the display flag was set), do not write the file.
   if(!calledFromCallback) {
         if(lockInputs(context) >= UT_ERROR_ABORT)
            throw SOP_RF_Export_Exception(canNotLockInputsInWriteMeshFile, exceptionError);

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
         myRFMeshFile->mesh_header.ID_code = 0xDADADADA;
         // Set version
         myRFMeshFile->mesh_header.version = 4;
         // Set code
         myRFMeshFile->mesh_header.code = 0xCCCCCCCC;

         boss = UTgetInterrupt();
         boss->opStart("Exporting Geometry To Mesh File");

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
                     if(myEchoData)
                        std::cout << "Real Flow Export Mesh file-cur_frame: " << cur_frame << "\tend_frame: " << myEndFrame
                                  << "\tFileName: " << (const char *) myFileName << std::endl;

                     if(lockInputs(context) >= UT_ERROR_ABORT)
                        throw SOP_RF_Export_Exception(canNotLockInputsInWriteMeshFile, exceptionError);

                     if(boss->opInterrupt())
                        throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                     // duplicate the incoming geometry
                     SOP_Node::duplicateSource(0, context);


                     gdp->GU_Detail::uniquePoints();

//                     GA_IndexMap myIndexMap(gdp, GA_ATTRIB_POINT);
//                     gdp->GU_Detail::sortPoints(myIndexMap, 7);

//void GU_Detail::uniquePoints   (  const GA_ElementGroup *    group = 0,
//    int   useMark = 0
// )
//
//void GU_Detail::sortPoints  (  GA_IndexMap &     array,
//    int   seed
// )
//
//void GU_Detail::sortPoints  (  GA_IndexMap &     array,
//    const UT_Vector3 &   o,
//    const UT_Vector3 &   d
// )
//
//void GU_Detail::sortPoints  (  GA_OffsetArray &     array,
//    GU_AxisType    axis = GU_XAXIS
// )
//
//void GU_Detail::sortPoints  (  GA_IndexMap &     array,
//    GU_AxisType    axis = GU_XAXIS
// )
//

//from GB_Vertex
//   GA_Index             getPointIndex() const;
//


                     // Get the number of primitives
                     myRFMeshFile->mesh_face_data.num_faces = (int)gdp->primitives().entries();

                     // Calculate number of vertices (assuming triangle polys which will be checked)
                     myRFMeshFile->mesh_vertex_data.num_vertices = (myRFMeshFile->mesh_face_data.num_faces * 3);

                     // Open the Real Flow Mesh file for writing
                     if(myRFMeshFile->openMeshFile((const char *) myFileName, RF_FILE_WRITE))
                        throw SOP_RF_Export_Exception(canNotOpenMeshFileForWriting, exceptionError);

                     // Write the header to the file
                     if(myRFMeshFile->writeMeshFileHeader())
                        throw SOP_RF_Export_Exception(canNotWriteHeaderMeshFile, exceptionError);


                     // For all the primitives in the geomtery, write out position
                     GA_FOR_ALL_PRIMITIVES(gdp, prim) {

                        if(boss->opInterrupt())
                           throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                        // Get the number of vertices
                        int numv = prim->getVertexCount();

                        // Check for triangular polygons ... if not, triangular, throw exeption, unlock inputs and return error
                        if(numv != 3)
                           throw SOP_RF_Export_Exception(notTriangularPolygons, exceptionError);

                        // If this primitive is a polygon
                        if(prim->getTypeId().get() == GEO_PRIMPOLY) {

                              for(int i = 0; i < 3; i++) {
                                    vtx = prim->getVertexElement(i);
                                    pos = vtx.getPos();

                                    // Set the mesh position data structure
                                    myRFMeshFile->mesh_vertex_data.X = pos.x();
                                    myRFMeshFile->mesh_vertex_data.Y = pos.y();
                                    myRFMeshFile->mesh_vertex_data.Z = pos.z();

                                    if(myEchoData)
                                       std::cout << "mesh_vertex_data: "
                                                 << myRFMeshFile->mesh_vertex_data.X << "\t"
                                                 << myRFMeshFile->mesh_vertex_data.Y
                                                 << "\t" << myRFMeshFile->mesh_vertex_data.Z << std::endl;

                                    // Write mesh position data to disk
                                    if(myRFMeshFile->writeMeshVertexData())
                                       throw SOP_RF_Export_Exception(canNotWriteDataToMeshFile, exceptionError);
                                 }
                           }
                        // Not a polygon, throw exception, unlock inputs and return error
                        else {
                              throw SOP_RF_Export_Exception(notAPolygon, exceptionError);
                           }
                     }


                     // write the number of faces
                     if(myRFMeshFile->writeMeshNumFaces())
                        throw SOP_RF_Export_Exception(canNotWriteNumFacesToMeshFile, exceptionError);

                     if(myEchoData)
                        cout << std::endl << "Processing vertex indices" << std::endl << std::endl;


                     long int vert_index = 0;

                     // Now for all the primitives in this object, retrieve the geometry face data and write to the mesh file
                     GA_FOR_ALL_PRIMITIVES(gdp, prim) {

                        if(boss->opInterrupt())
                           throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                        myRFMeshFile->mesh_face_data.vertex[0] = vert_index++;
                        myRFMeshFile->mesh_face_data.vertex[1] = vert_index++;
                        myRFMeshFile->mesh_face_data.vertex[2] = vert_index++;

                        if(myEchoData)
                           std::cout << "mesh_face_data.vertex: "
                                     << myRFMeshFile->mesh_face_data.vertex[0] << "\t"
                                     << myRFMeshFile->mesh_face_data.vertex[1]  << "\t"
                                     << myRFMeshFile->mesh_face_data.vertex[2] << std::endl;

                        myRFMeshFile->writeMeshFaceData();
                     }

// GA_Index GA_Detail::primitiveIndex  (  GA_Offset   offset    )    const [inline]
//
//Given a primitive's data offset, return its index.
//
//Definition at line 306 of file GA_Detail.h.
//
//GA_Offset GA_Detail::primitiveOffset    (  GA_Index    index     )    const [inline]
//
//Given a primitive's index (in append order), return its data offset.
//
//Definition at line 303 of file GA_Detail.h.
//
//static int64 GA_Detail::printMemory  (  UT_WorkBuffer &   buffer,
//    bool     include_total = true,
//    int   indent = 3,
//    const char *   title = "Geometry Memory Tracking"
// )        [static]


                     // Check to see if the velocity and uv attributes are present
                     GA_ROAttributeRef v_ref = gdp->findPointAttribute("v");
                     GA_ROAttributeRef uv_ref = gdp->findPointAttribute("uv");
                     UT_Vector3   uv_vec(0.0, 0.0, 0.0);
                     UT_Vector3   v_vec(0.0, 0.0, 0.0);

                     // If there's a texture attribute (uv), write it to the mesh file.
                     if(uv_ref.isValid()) {
                           if(myEchoData)
                              cout << std::endl << "Processing texture attribute" << std::endl << std::endl;

                           myRFMeshFile->mesh_tex_data.code = 0xCCCCCC00;
                           myRFMeshFile->mesh_tex_data.num_fluids = 1;
                           myRFMeshFile->mesh_tex_data.texture_weight[0] = 1.0;

                           myRFMeshFile->writeMeshTextureChunkCode();
                           myRFMeshFile->writeMeshNumFluids();

                           // Now for all the primitives in this object, retrieve the geometry texture data and write to the mesh file
                           GA_FOR_ALL_PRIMITIVES(gdp, prim) {

                              if(boss->opInterrupt())
                                 throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                              for(int i = 0; i < 3; i++) {
                                    ppt = prim->getVertexElement(i).getPt();
                                    uv_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(uv_ref));
                                    myRFMeshFile->mesh_tex_data.U = static_cast<float>(uv_vec.x());
                                    myRFMeshFile->mesh_tex_data.V = static_cast<float>(uv_vec.y());
                                    myRFMeshFile->mesh_tex_data.W = static_cast<float>(uv_vec.z());

                                    if(myEchoData)
                                       std::cout << "mesh_tex_data: "
                                                 << myRFMeshFile->mesh_tex_data.U << "\t"
                                                 << myRFMeshFile->mesh_tex_data.V  << "\t"
                                                 << myRFMeshFile->mesh_tex_data.W << std::endl;

                                    myRFMeshFile->writeMeshTextureData();
                                 }
                           }

                        }

                     if(v_ref.isValid()) {
                           if(myEchoData)
                              cout << std::endl << "Processing velocity attribute" << std::endl << std::endl;

                           myRFMeshFile->mesh_vel_data.code = 0xCCCCCC11;

                           myRFMeshFile->writeMeshVelocityChunkCode();

                           // Now for all the primitives in this object, retrieve the velocity data and write to the mesh file
                           GA_FOR_ALL_PRIMITIVES(gdp, prim) {

                              if(boss->opInterrupt())
                                 throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                              for(int i = 0; i < 3; i++) {
                                    ppt = prim->getVertexElement(i).getPt();
                                    v_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(v_ref));
                                    myRFMeshFile->mesh_vel_data.X = static_cast<float>(v_vec.x());
                                    myRFMeshFile->mesh_vel_data.Y = static_cast<float>(v_vec.y());
                                    myRFMeshFile->mesh_vel_data.Z = static_cast<float>(v_vec.z());

                                    if(myEchoData)
                                       std::cout << "mesh_vel_data: "
                                                 << myRFMeshFile->mesh_vel_data.X << "\t"
                                                 << myRFMeshFile->mesh_vel_data.Y  << "\t"
                                                 << myRFMeshFile->mesh_vel_data.Z << std::endl;

                                    myRFMeshFile->writeMeshVelocityData();
                                 }
                           }
                        }


                     // Write EOF to mesh file
                     if(myRFMeshFile->writeMeshFileEOF())
                        throw SOP_RF_Export_Exception(canNotWriteEOFToMeshFile, exceptionError);
                     if(myEchoData)
                        std::cout << std::endl << "Wrote EOF to mesh file " << std::endl;

                     // We're done, close the file
                     if(myRFMeshFile->closeMeshFile(RF_FILE_WRITE))
                        throw SOP_RF_Export_Exception(canNotCloseTheRealFlowMeshFile, exceptionError);
                     if(myEchoData)
                        std::cout << std::endl << "Closed mesh file " << std::endl;

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

         if(myRFMeshFile->RFMeshofstream.is_open()) {
               // Close the RF Mesh file
               if(myRFMeshFile->closeMeshFile(RF_FILE_WRITE)) {
                     addError(SOP_MESSAGE, "Can't close Real Flow Mesh file after SOP_RF_Export_Exception exception was thrown");
                     return error();
                  }
            }
         return error();
      }


// Restore the frame
   context.setFrame((long) save_frame);

   return error();
}


