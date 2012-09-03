/* ******************************************************************************
*  Read Real Flow SD File
*
* $RCSfile: SOP_RF_Import_ReadRFSDFileRestGeo.C,v $
*
*  Description : Creates "static" geometry for the Real Flow SD file
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import_ReadRFSDFileRestGeo.C,v $
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

/* ******************************************************************************
*  Function Name : ReadRFSDCreateStaticGeo()
*
*
*  Input Arguments : UT_Interrupt   *boss
*
*  Return Value : int
*
***************************************************************************** */
inline int SOP_RF_Import::ReadRFSDCreateRestGeo(UT_Interrupt *boss){

   GEO_Point *ppt;
   GU_PrimPoly *poly;
   long int vtx_idx, point_idx = 0;


try {

   // For each object in the SD file, read it's geometry and create it
   for(int cur_object = 0; cur_object < myRFSDFile->myRF_SD_Header.num_objects; cur_object++) {

      myRFSDFile->myRF_SD_Obj_Header.obj_name = "";

      // Read the object header
      if(myRFSDFile->readSDObjHdr())
         throw SOP_RF_Import_Exception(canNotReadTheSDObjectHeader, exceptionError);

      // set the total number of points for this geometry
      myTotalPoints = myRFSDFile->myRF_SD_Obj_Header.num_vertices;
      myRFSDFile->obj_detail[cur_object].num_points = myRFSDFile->myRF_SD_Obj_Header.num_vertices;
      myRFSDFile->obj_detail[cur_object].num_faces = myRFSDFile->myRF_SD_Obj_Header.num_faces;
      myRFSDFile->obj_detail[cur_object].mode = myRFSDFile->myRF_SD_Obj_Header.obj_mode;

#ifdef DEBUG
std::cout << "obj_name: " << myRFSDFile->myRF_SD_Obj_Header.obj_name << std::endl;
#endif

      // Create a group named the same as the object's name
      UT_String pt_grp_name("pt_");
      pt_grp_name += (myRFSDFile->myRF_SD_Obj_Header.obj_name.substr(0, myRFSDFile->myRF_SD_Obj_Header.obj_name_len)).c_str();
      pt_grp_name.substitute("/","_");
      UT_String prim_grp_name("pr_");
      prim_grp_name += (myRFSDFile->myRF_SD_Obj_Header.obj_name.substr(0, myRFSDFile->myRF_SD_Obj_Header.obj_name_len)).c_str();
      prim_grp_name.substitute("/","_");

      // Create a group named the same as the object's name
      objPointGrpList[cur_object] = gdp->newPointGroup((const char*)pt_grp_name);
      objPointGrpList[cur_object]->clearEntries();
      objPrimitiveGrpList[cur_object] = gdp->newPrimitiveGroup((const char*)prim_grp_name);
      objPrimitiveGrpList[cur_object]->clearEntries();


#ifdef DEBUG
std::cout << std::endl << "Object Header: " << std::endl;

if(myRFSDFile->myRF_SD_Obj_Header.obj_mode == 0)
   std::cout << "obj_mode: matrix mode " << std::endl;
else
   std::cout << "obj_mode: vertex mode " << std::endl;

std::cout << "obj_color: " << myRFSDFile->myRF_SD_Obj_Header.obj_color[0] <<
   "\t" << myRFSDFile->myRF_SD_Obj_Header.obj_color[1] <<
   "\t" << myRFSDFile->myRF_SD_Obj_Header.obj_color[2] << std::endl;
std::cout << "obj_name_len: " <<
   myRFSDFile->myRF_SD_Obj_Header.obj_name_len << std::endl;
std::cout << "obj_name: " << myRFSDFile->myRF_SD_Obj_Header.obj_name << std::endl;

// TODO: Add detail level attribute for the object's texture name
std::cout << "obj_tex_len: " <<
   myRFSDFile->myRF_SD_Obj_Header.obj_tex_len << std::endl;
std::cout << "obj_tex_name: " <<
   myRFSDFile->myRF_SD_Obj_Header.obj_tex_name << std::endl;

std::cout << "num_vertices: " << myRFSDFile->myRF_SD_Obj_Header.num_vertices << std::endl;
std::cout << "num_faces " << myRFSDFile->myRF_SD_Obj_Header.num_faces << std::endl;

std::cout << std::endl << "Vertex Data: " << std::endl;
#endif


      // For each vertex, read it's coordinates, add a point, and set it's coordinates.
      for(int cur_point=0; cur_point < myRFSDFile->myRF_SD_Obj_Header.num_vertices; cur_point++) {

         // Read the face coordinates
         if(myRFSDFile->readSDFaceCoord())
            throw SOP_RF_Import_Exception(canNotReadTheSDFaceCoords, exceptionError);


#ifdef DEBUG
std::cout << "X Y Z "
   << "\t" << myRFSDFile->myRF_SD_Face_Data.vertex[0]
   << "\t" << myRFSDFile->myRF_SD_Face_Data.vertex[1]
   << "\t" << myRFSDFile->myRF_SD_Face_Data.vertex[2] << std::endl;
#endif

         // Check to see if we've been interrupted
         if (boss->opInterrupt())
            throw SOP_RF_Import_Exception(theSDPointCreationInterrupt, exceptionError);

         // Set the current point (for local vars)
         myCurrPoint = cur_point;

         // Append a point to the geometry detail and set it's position and add it to it's group
         ppt = gdp->appendPointElement();
         objPointGrpList[cur_object]->add(ppt);
         // Assign the position to the point
         gdp->points()[cur_point]->setPos((float)myRFSDFile->myRF_SD_Face_Data.vertex[0],
                              (float)myRFSDFile->myRF_SD_Face_Data.vertex[1],
                              (float)myRFSDFile->myRF_SD_Face_Data.vertex[2], 1.0);

//         ppt->getPos().assign((float)myRFSDFile->myRF_SD_Face_Data.vertex[0],
//            (float)myRFSDFile->myRF_SD_Face_Data.vertex[1],
//            (float)myRFSDFile->myRF_SD_Face_Data.vertex[2], 1);
      }


#ifdef DEBUG
std::cout << std::endl << "Face Data: " << std::endl;
#endif

      // For each face, create a polygon, read it's index and assign the points to it's vertices.
      for(int cur_face=0; cur_face < myRFSDFile->myRF_SD_Obj_Header.num_faces; cur_face++)
      {
         if (boss->opInterrupt())
            throw SOP_RF_Import_Exception(theSDPolygonCreationInterrupt, exceptionWarning);

         // Read the face vertex index
         if(myRFSDFile->readSDFaceIndex())
            throw SOP_RF_Import_Exception(canNotReadTheSDFaceVertexIndex, exceptionError);

#ifdef DEBUG
std::cout << std::endl << "face number: " << cur_face + 1 << " of "
<< myRFSDFile->myRF_SD_Obj_Header.num_faces << " faces " << std::endl;

std::cout << "vertex indices:  " << "\t"
<< myRFSDFile->myRF_SD_Face_Data.vertex_idx[0] << "\t"
<< myRFSDFile->myRF_SD_Face_Data.vertex_idx[1] << "\t"
<< myRFSDFile->myRF_SD_Face_Data.vertex_idx[2] << std::endl;
#endif


         // Build a polygon (0 vertices)
         poly = GU_PrimPoly::build(gdp, 0, GU_POLY_CLOSED);
         objPrimitiveGrpList[cur_object]->add(poly);

         // append a vertex with it's point.
         ppt = gdp->points().entry(myRFSDFile->myRF_SD_Face_Data.vertex_idx[0] + point_idx);
         vtx_idx = poly->appendVertex(ppt);

         ppt = gdp->points().entry(myRFSDFile->myRF_SD_Face_Data.vertex_idx[1] + point_idx);
         vtx_idx = poly->appendVertex(ppt);

         ppt = gdp->points().entry(myRFSDFile->myRF_SD_Face_Data.vertex_idx[2] + point_idx);
         vtx_idx = poly->appendVertex(ppt);

         // Read the UVW values
         if(myRFSDFile->readSDFaceTexture())
            throw SOP_RF_Import_Exception(canNotReadTheSDFaceTexture, exceptionError);


#ifdef DEBUG
for(int i = 0; i < 3; i++)
   for(int j=0; j < 3; j++)
      std::cout << "vertex texture coords: " << myRFSDFile->myRF_SD_Face_Data.vertex_tex[i][j] << std::endl;
#endif

         GA_RWHandleV3 attrVector3Handle;

         // assign texture data
//         if(myGUIState.t_sd_tex) {
//            if (myAttributeRefs.sd_texture_0.isValid()) {
//                attrVector3Handle.bind(myAttributeRefs.sd_texture_0.getAttribute());
//                attrVector3Handle.set(gdp->pointOffset(cur_face * 3), UT_Vector3(myRFSDFile->myRF_SD_Face_Data.vertex_tex[0]));
//            }
//            if (myAttributeRefs.sd_texture_1.isValid()) {
//                attrVector3Handle.bind(myAttributeRefs.sd_texture_0.getAttribute());
//                attrVector3Handle.set(gdp->pointOffset(cur_face * 3), UT_Vector3(myRFSDFile->myRF_SD_Face_Data.vertex_tex[1]));
//            }
//            if (myAttributeRefs.sd_texture_2.isValid()) {
//                attrVector3Handle.bind(myAttributeRefs.sd_texture_0.getAttribute());
//                attrVector3Handle.set(gdp->pointOffset(cur_face * 3), UT_Vector3(myRFSDFile->myRF_SD_Face_Data.vertex_tex[2]));
//            }
//         }

         // Read the visibility values
         // TODO: add visibility primitive attribute
         if(myRFSDFile->readSDFaceVis())
            throw SOP_RF_Import_Exception(canNotReadTheSDFaceVisibility, exceptionError);


#ifdef DEBUG
std::cout << "vertex visibility flags: ";
for(int i = 0; i < 3; i++)
   std::cout << myRFSDFile->myRF_SD_Face_Data.visible[i] << " ";
std::cout << std::endl;
#endif

         // Read the material index (not used)
         // TODO: add material index primitive attribute
         if(myRFSDFile->readSDFaceMat())
            throw SOP_RF_Import_Exception(canNotReadTheSDFaceMaterialIndex, exceptionError);

#ifdef DEBUG
std::cout << "face material indices: ";
for(int i = 0; i < 3; i++)
   std::cout << myRFSDFile->myRF_SD_Face_Data.mat_idx << " ";
   std::cout << std::endl;
#endif

      } // for each face

//      objPointGrpList[cur_object]->setEntries();
//       objPrimitiveGrpList[cur_object]->setEntries();


#ifdef DEBUG
std::cout << "cur_object: " << cur_object << std::endl;
std::cout << "number of entries in this point group: " << objPointGrpList[cur_object]->entries() << std::endl;
std::cout << "number of entries in this prim group: " << objPrimitiveGrpList[cur_object]->entries() << std::endl;
#endif

   point_idx += myRFSDFile->myRF_SD_Obj_Header.num_vertices;

#ifdef DEBUG
std::cout << "point index: " << point_idx << std::endl;
#endif

} // for each object



}
catch (SOP_RF_Import_Exception e) {
   e.what();

   if(e.getSeverity() == exceptionWarning)
      addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
   else if(e.getSeverity() == exceptionError)
      addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

   if(myRFSDFile->SDifstream.is_open()) {
         myRFSDFile->closeSDFile(RF_FILE_READ);
      }

   return 1;

   }

return 0;

}



/**********************************************************************************/
//  $Log: SOP_RF_Import_ReadRFSDFileRestGeo.C,v $
//  Revision 1.3  2012-08-31 03:00:51  mstory
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
