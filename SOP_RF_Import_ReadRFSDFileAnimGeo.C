/* ******************************************************************************
*  Read Real Flow SD File
*
* $RCSfile: SOP_RF_Import_ReadRFSDFileAnimGeo.C,v $
*
*  Description : Create the animated geometry for the Real Flow SD file.
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import_ReadRFSDFileAnimGeo.C,v $
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
*  Function Name : ReadRFSDCreateAnimGeo()
*
*
*  Input Arguments : OP_Context &context
*
*  Return Value : int
*
***************************************************************************** */
inline int SOP_RF_Import::ReadRFSDCreateAnimGeo(UT_Interrupt *boss)
{

    UT_XformOrder xformOrder(UT_XformOrder::SRT);
    UT_Matrix4 work_matrix;
    GEO_Point *ppt;

    try {

        // For each object in the SD file, read it's frame header and geometry
        for (int cur_object = 0; cur_object < myRFSDFile->myRF_SD_Header.num_objects; cur_object++) {

            // read the object's frame header data
            if (myRFSDFile->readSDObjFrameHdr())
                throw SOP_RF_Import_Exception(canNotReadSDObjectFrameHeader, exceptionError);


#ifdef DEBUG
            std::cout << "obj_name_len: " << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name_len << std::endl;
            std::cout << "Object Name: " << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_name << std::endl;
            std::cout << "Object Transform:" << std::endl;
            for (int i=0; i < 12; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[i] << "\t";
            std::cout << std::endl;
            std::cout << "Translation Vector: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[i] << "\t";
            std::cout << std::endl;
            std::cout << "Rotation Vector: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[i] << "\t";
            std::cout << std::endl;
            std::cout << "Scale Vector: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[i] << "\t";
            std::cout << std::endl;
            std::cout << "Pivot Position: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[i] << "\t";
            std::cout << std::endl << std::endl;
            std::cout << "CG Position: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[i] << "\t";
            std::cout << std::endl;
            std::cout << "CG Velocity: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel[i] << "\t";
            std::cout << std::endl;
            std::cout << "CG Rotation: " << std::endl;
            for (int i=0; i < 3; i++) std::cout << myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[i] << "\t";
            std::cout << std::endl << std::endl;
#endif

            if (myGUIState.t_sd_cg) {
                work_matrix.identity();
                work_matrix.xform(xformOrder,
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[0],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[1],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_trans_vec[2],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[0],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[1],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_rot_vec[2],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[0],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[1],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_scale_vec[2],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[0],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[1],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_pivot_pos[2]);
                // Transform the geometry
                gdp->transformPoints(work_matrix, (const GA_PointGroup *)objPrimitiveGrpList[cur_object]);
            }

            if (myGUIState.t_sd_cg_xform) {
                work_matrix.identity();
                work_matrix.xform(xformOrder,
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[0],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[1],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos[2],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[0],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[1],
                                  myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot[2]);
                // Transform the geometry
                gdp->transformPoints(work_matrix, (const GA_PointGroup *)objPrimitiveGrpList[cur_object]);
            }

#ifdef DEBUG
            std::cout << "Current primitive group: " << objPrimitiveGrpList[cur_object]->getName() << std::endl;
#endif

            GA_RWAttributeRef attrRef;
            GA_RWHandleI attrIntHandle;
            GA_RWHandleF attrFloatHandle;
            GA_RWHandleV3 attrVector3Handle;

            // If the user wants the CG data, add it to the geo detail
            if (myGUIState.t_sd_cg) {

                if (myAttributeRefs.sd_CG_pos.isValid()) {
                    attrVector3Handle.bind(myAttributeRefs.sd_CG_pos.getAttribute());
                    attrVector3Handle.set(0, UT_Vector3(myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_pos));
                }

                if (myAttributeRefs.sd_CG_vel.isValid()) {
                    attrVector3Handle.bind(myAttributeRefs.sd_CG_vel.getAttribute());
                    attrVector3Handle.set(0, UT_Vector3(myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_vel));
                }

                if (myAttributeRefs.sd_CG_rot.isValid()) {
                    attrVector3Handle.bind(myAttributeRefs.sd_CG_rot.getAttribute());
                    attrVector3Handle.set(0, UT_Vector3(myRFSDFile->myRF_SD_Obj_Frame_Header.obj_CG_rot));
                }

            }


            // If vertex mode, update coordinates
            if (myRFSDFile->myRF_SD_Obj_Header.obj_mode) {

                for (int cur_point=0; cur_point < myRFSDFile->obj_detail[cur_object].num_points; cur_point++) {

#ifdef DEBUG
                    std::cout << "vertex number: " << cur_point + 1 << " of "
                              << myRFSDFile->myRF_SD_Obj_Header.num_vertices
                              << " vertices " << std::endl;
#endif

                    if ( myRFSDFile->readSDFaceCoord())
                        throw SOP_RF_Import_Exception(canNotReadSDAnimFaceData, exceptionError);

#ifdef DEBUG
                    std::cout << "Vertex Coordinates: " << std::endl;
                    std::cout << myRFSDFile->myRF_SD_Face_Data.vertex[0] << "\t";
                    std::cout << myRFSDFile->myRF_SD_Face_Data.vertex[1] << "\t";
                    std::cout << myRFSDFile->myRF_SD_Face_Data.vertex[2] << std::endl;
#endif

                    // update points
                    if (boss->opInterrupt())
                        throw SOP_RF_Import_Exception(theSDAnimGeoCreationInterrupt, exceptionWarning);

                    myCurrPoint = cur_point;

                    // Get the current point to either update position
                    ppt = gdp->points().entry(cur_point);

                    if (ppt == NULL)
                        throw SOP_RF_Import_Exception(theSDAnimGeoCreationPointNULL, exceptionError);

#ifdef DEBUG
                    std::cout << "cur_object: " << cur_object << std::endl;
#endif

                    gdp->points()[cur_point]->setPos((float)myRFSDFile->myRF_SD_Face_Data.vertex[0],
                                                     (float)myRFSDFile->myRF_SD_Face_Data.vertex[1],
                                                     (float)myRFSDFile->myRF_SD_Face_Data.vertex[2], 1.0);

#ifdef DEBUG
                    std::cout << "position assigned" << std::endl;
#endif

                } // for(int cur_point=0; ...
            } // if(myRFSDFile->myRF_SD_Obj_Header.obj_mode)
        } // if vertex mode


    } catch (SOP_RF_Import_Exception e) {
        e.what();

        if (e.getSeverity() == exceptionWarning)
            addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
        else if (e.getSeverity() == exceptionError)
            addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

        if (myRFSDFile->SDifstream.is_open()) {
            myRFSDFile->closeSDFile(RF_FILE_READ);
        }

        return 1;
    }

    return 0;
}





/**********************************************************************************/
//  $Log: SOP_RF_Import_ReadRFSDFileAnimGeo.C,v $
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
