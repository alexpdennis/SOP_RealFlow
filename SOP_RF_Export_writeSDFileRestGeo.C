/* ******************************************************************************
*   Real Flow
*
* $RCSfile: SOP_RF_Export_writeSDFileRestGeo.C,v $
*
* Description : Write the Real Flow SD rest geomtery to disk
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export_writeSDFileRestGeo.C,v $
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
*  Function Name :  writeSDFileRestGeo()
*
*  Description : Write the Real Flow SD rest geomtery
*
*  Input Arguments : OP_Context & context
*
*  Return Value : int return status
*
***************************************************************************** */
int SOP_RF_Export::writeSDFileRestGeo(OP_Context & context, UT_Interrupt * boss)
{

   long int numv;
   int vtx_index_num = 0;
   int current_obj;
   long int num_prim = 0;
   UT_Vector3 normal, p, uv_vec, clr_vec;
   UT_Vector4 pos;
   GEO_Primitive * prim;
   GEO_Vertex vtx;
   GEO_AttributeHandle attrHandle;
   GA_ROAttributeRef uv_ref;
   UT_String  tex_fname_str;

   UT_Vector3 trans, rot, scale, up, shear, pivot;
   UT_XformOrder xformOrder(UT_XformOrder::SRT);
   UT_Vector3 CG_pos;
   UT_Vector3 CG_vel;
   UT_Vector3 CG_rot;
   UT_String my_cwd, my_full_path, my_parent_full_path, my_name;
   UT_String current_input_path;

#ifdef DEBUG
   std::cout << "SOP_RF_Export::writeSDFileRestGeo()" << std::endl;
#endif

   try {

         // For each object (input)
         for(current_obj = 0; current_obj < myNumInputs; current_obj++) {

               // Set the the object's name length and name
               myRFSDFile->myRF_SD_Obj_Header.obj_name_len = objectNames(current_obj).length();
               myRFSDFile->myRF_SD_Obj_Header.obj_name = objectNames(current_obj);

               // If the "sd_obj_color" attribute is in the geometry, assign it to the object header
               UT_Vector3 sd_obj_color_vec;
               attrHandle = gdp->getDetailAttribute("sd_obj_color");
               if(attrHandle.isAttributeValid()) {
                     sd_obj_color_vec = attrHandle.getV3();

                     // Assign object color
                     myRFSDFile->myRF_SD_Obj_Header.obj_color[0] = sd_obj_color_vec.x();
                     myRFSDFile->myRF_SD_Obj_Header.obj_color[1] = sd_obj_color_vec.y();
                     myRFSDFile->myRF_SD_Obj_Header.obj_color[2] = sd_obj_color_vec.z();

#ifdef DEBUG
                     std::cout << "sd_obj_color_vec: " << sd_obj_color_vec.x() << std::endl;
                     std::cout << "sd_obj_color_vec: " << sd_obj_color_vec.y() << std::endl;
                     std::cout << "sd_obj_color_vec: " << sd_obj_color_vec.z() << std::endl;
#endif

                  }
               // If not found, assign the GUI's color value
               else {
                     myRFSDFile->myRF_SD_Obj_Header.obj_color[0] = myObjColor[0];
                     myRFSDFile->myRF_SD_Obj_Header.obj_color[1] = myObjColor[1];
                     myRFSDFile->myRF_SD_Obj_Header.obj_color[2] = myObjColor[2];
                  }

               if(objectTextureNames.entries() > 0) {
                     myRFSDFile->myRF_SD_Obj_Header.obj_tex_len = objectTextureNames(current_obj).length();
                     myRFSDFile->myRF_SD_Obj_Header.obj_tex_name = objectTextureNames(current_obj);
                  }

               // Lock this input, if it fails, return
               if(lockInput(current_obj, context) >= UT_ERROR_ABORT)
                  throw SOP_RF_Export_Exception(couldNotLockInputInWriteSDFile, exceptionError);

               // Duplicate the geometry from this input
               duplicateSource(current_obj, context, gdp);

               // Set the number of faces and vertices for this object
               myRFSDFile->myRF_SD_Obj_Header.num_faces = (long int) gdp->primitives().entries();
               myRFSDFile->myRF_SD_Obj_Header.num_vertices = (myRFSDFile->myRF_SD_Obj_Header.num_faces * 3);

#ifdef DEBUG
               std::cout << "in object loop: num_vertices: " << myRFSDFile->myRF_SD_Obj_Header.num_vertices << std::endl;
               std::cout << "in object loop: num_faces: " << myRFSDFile->myRF_SD_Obj_Header.num_faces << std::endl;
#endif

               // Write the SD file object header  for this object
               if(myRFSDFile->writeSDObjHdr())
                  throw SOP_RF_Export_Exception(canNotWriteRealFlowSDObjectHeader, exceptionError);

               if(myEchoData)
                  std::cout << std::endl << "Processing vertex coordinates for object #"  <<
                            current_obj << std::endl << std::endl;

               // Now for all the primitives in this object, retrieve the geometry face data and write to the SD file
               num_prim = 0;
               GA_FOR_ALL_PRIMITIVES(gdp, prim) {

                  if(boss->opInterrupt())
                     throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                  // Get the number of vertices
                  numv = prim->getVertexCount();

                  if(myEchoData) {
                        std::cout << "Primitive num: " << num_prim;
                        std::cout << "\tNumber of vertices: " << numv << std::endl;
                     }

                  // Check for triangular polygons ... if not, triangular, throw exeption, unlock inputs and return error
                  if(numv != 3)
                     throw SOP_RF_Export_Exception(notTriangularPolygons, exceptionError);

                  // If this primitive is a polygon
                  if(prim->getTypeId().get() == GEO_PRIMPOLY) {

                        // For each vertex, get it's position and write the face coordinates to the SD file
                        for(int i = 0; i < 3; i++) {
                              vtx = prim->getVertexElement(i);
                              pos = vtx.getPos();

                              if(myRFSDFile->writeSDFaceCoord(pos.x(), pos.y(), pos.z()))
                                 throw SOP_RF_Export_Exception(canNotWriteFaceCoordinates, exceptionError);
                           }
                     }
                  // Not a polygon, throw exception, unlock inputs and return error
                  else {
                        throw SOP_RF_Export_Exception(notAPolygon, exceptionError);
                     }
                  num_prim++;
               }

               if(myEchoData)
                  cout << std::endl << "Processing vertex indices" << std::endl << std::endl;


               // Try to find if there's a uv attribute in the geometry ...
               uv_ref = gdp->findTextureAttribute(GEO_VERTEX_DICT);

               num_prim = 0;
               vtx_index_num = 0;

               // Now for all the primitives in this object, retrieve the geometry data and write to the SD file
               GA_FOR_ALL_PRIMITIVES(gdp, prim) {
                  if(boss->opInterrupt())
                     throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

                  numv = prim->getVertexCount();

                  if(myEchoData) {
                        std::cout << "Primitive num: " << num_prim;
                        std::cout << "\tNumber of vertices: " << numv << std::endl;
                     }

                  // for each vertex in the polygon, try to get the "uv" primitive attribute, if found assign it in the SD file
                  for(int i = 0; i < 3; i++) {

                        // Get a vertex
                        vtx = prim->getVertexElement(i);

                        if(uv_ref.isValid()) {
                              attrHandle = gdp->getVertexAttribute("uv");
                              if(attrHandle.isAttributeValid())
                                 uv_vec = attrHandle.getV3();

                              myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][0] = static_cast<float>(uv_vec.x());
                              myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][1] = static_cast<float>(uv_vec.y());
                              myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][2] = static_cast<float>(uv_vec.z());
                           }

#ifdef DEBUG
                        std::cout << "uv:" << std::endl;
                        std::cout << myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][0] << std::endl;
                        std::cout << myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][1] << std::endl;
                        std::cout << myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][2] << std::endl;
#endif

                        // write it's vertex number to the file
                        if(myRFSDFile->writeSDFaceIndex(vtx_index_num))
                           throw SOP_RF_Export_Exception(canNotWriteFaceIndex, exceptionError);

                        // Increment our vertex count
                        vtx_index_num++;
                     }

                  // Write the UVW values
                  if(myRFSDFile->writeSDFaceTexture())
                     throw SOP_RF_Export_Exception(canNotWriteFaceTextures, exceptionError);

                  // Write the visibility values (always on)
                  int vis = 1;
                  if(myRFSDFile->writeSDFaceVis(vis))
                     throw SOP_RF_Export_Exception(canNotWriteFaceVisiblityValues, exceptionError);

                  // Write the material index (not used)
                  int mat = 1;
                  if(myRFSDFile->writeSDFaceMat(mat))
                     throw SOP_RF_Export_Exception(canNotWriteFaceMaterialIndex, exceptionError);

                  num_prim++;
               }

               unlockInput(current_obj);
            } // End-For each object (input)

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
//  $Log: SOP_RF_Export_writeSDFileRestGeo.C,v $
//  Revision 1.4  2012-08-31 03:00:51  mstory
//  Continued H12 mods.
//
//  Revision 1.3  2012-08-29 03:07:06  mstory
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
