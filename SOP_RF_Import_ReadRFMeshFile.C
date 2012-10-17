/* ******************************************************************************
*  Real Flow Mesh Geometry File Import
*
* $RCSfile: SOP_RF_Import_ReadRFMeshFile.C,v $
*
* Description : This module implements the function to read a Real Flow mesh file and create it's geometry
*
* $Revision: 1.5 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import_ReadRFMeshFile.C,v $
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
*  Function Name : ReadRFMeshFile()
*
*  Description : Read a Real Flow mesh file and create the geometry including
*                   the attributes the user has selected
*
*  Input Arguments : OP_Context &context
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR
SOP_RF_Import::ReadRFMeshFile(OP_Context & context)
{

    int i, j, hdr_status = 0;
    int vtx_idx = 0;

    GA_RWAttributeRef m_texture_uv, m_velocity_v, m_num_fluids;
    GA_RWAttributeRef m_text_wght[MAX_NUM_FLUIDS];
    GA_RWAttributeRef attrRef;
    GA_RWHandleI attrIntHandle;
    GA_RWHandleF attrFloatHandle;
    GA_RWHandleV3 attrVector3Handle;

    int t_texture, t_velocity = 0;
    float now = 0;
    UT_Interrupt * boss;
    GEO_Point * ppt;
    GU_PrimPoly * poly;
//   UT_Vector3 *vel, *tex_map;
//   float *text_wght[MAX_NUM_FLUIDS];  // TODO: replace this array with a STL vector object
    char GUI_str[128];  // std::string instead?

    now = context.getTime();

    //Get GUI parms for texture and velocity attributes
    t_texture = MESH_TEX(now);
    t_velocity = MESH_VEL(now);

#ifdef DEBUG
    std::cout << "myFileName:" << (const char *) myFileName << endl;
    std::cout << "t_texture:" << t_texture << endl;
    std::cout << "t_velocity:" << t_velocity << endl;
#endif

    // Enter try block for the mesh reader
    try {

        // Check to see that there hasn't been a critical error in cooking the SOP.
        if(error() < UT_ERROR_ABORT) {

            boss = UTgetInterrupt();
            gdp->clearAndDestroy();

            // Start the interrupt server
            boss->opStart("Importing Real Flow Mesh");

            // Open the Real Flow Mesh File
            if(myRFMeshFile->openMeshFile((char *)myFileName))
                throw SOP_RF_Import_Exception(canNotOpenRealFlowMeshFileForReading, exceptionError);

            // Read the header
            if(myRFMeshFile->readMeshFileHeader(&hdr_status))
                throw SOP_RF_Import_Exception(canNotReadTheMeshFileHeader, exceptionError);

            if(hdr_status == -1)
                throw SOP_RF_Import_Exception(notARealFlowMeshFile, exceptionError);

            if(!(myRFMeshFile->mesh_vertex_data.num_vertices > 0))
                throw SOP_RF_Import_Exception(numRealFlowMeshVerticesInvalid, exceptionError);


            // TODO: Set Object location to set these coordinates
            // X = myRFMeshFile->mesh_vertex_data.X;
            // Y = myRFMeshFile->mesh_vertex_data.Y;
            // Z = myRFMeshFile->mesh_vertex_data.Z;



            // Update the GUI with the mesh version number
            sprintf(GUI_str, "%s%d", "Mesh File Version #", myRFMeshFile->mesh_header.version);

            setString((UT_String) GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);

            // Assign the total number of points for this SOP
            myTotalPoints = myRFMeshFile->mesh_vertex_data.num_vertices;

            // Get the vertices point data
            for(i = 0; i < myRFMeshFile->mesh_vertex_data.num_vertices; i++) {

                // Check to see if the user has interrupted us...
                if(boss->opInterrupt())
                    throw SOP_RF_Import_Exception(cookInterrupted, exceptionWarning);

                myCurrPoint = i;

                // Read the mesh data from the file
                if(myRFMeshFile->readMeshVertexData())
                    throw SOP_RF_Import_Exception(canNotReadTheMeshFileVertextData, exceptionError);

                // Append a point to the geometry detail and set it's position
                ppt = gdp->appendPointElement();

                // Assign the position to the point
                gdp->points()[i]->setPos((float)myRFMeshFile->mesh_vertex_data.X,
                                         (float)myRFMeshFile->mesh_vertex_data.Y,
                                         (float)myRFMeshFile->mesh_vertex_data.Z, 1.0);
            }


            // Read the the number of faces
            if(myRFMeshFile->readMeshNumFaces())
                throw SOP_RF_Import_Exception(canNotReadTheMeshFileNumFaces, exceptionError);


            // For each face, read the face data, build a polygon and add
            // 3 vertices to it and referencing it's points.
            for(i = 0; i < myRFMeshFile->mesh_face_data.num_faces; i++) {
                // Check to see if the user has interrupted us...
                if(boss->opInterrupt())
                    throw SOP_RF_Import_Exception(cookInterrupted, exceptionWarning);

                // Read the mesh data from the file
                if(myRFMeshFile->readMeshFaceData())
                    throw SOP_RF_Import_Exception(canNotReadTheMeshFileFaceData, exceptionError);

                // Build a polygon (0 vertices)
                poly = GU_PrimPoly::build(gdp, 0, GU_POLY_CLOSED);

                // Loop 3 times and append a vertex with it's point.
                for(j = 0; j < 3; j++) {
                    ppt = gdp->points().entry(myRFMeshFile->mesh_face_data.vertex[j]);
                    vtx_idx = poly->appendVertex(ppt);

#ifdef DEBUG
                    std::cout << "vertex index: " << vtx_idx << endl;
                    std::cout << "vertex[j]:" << myRFMeshFile->mesh_face_data.vertex[j] << endl;
#endif
                }
            }     // for number of faces


            // If a file error occured when trying to read the texture chunk, return with error
            if(myRFMeshFile->readMeshTextureChunkCode())
                throw SOP_RF_Import_Exception(canNotReadTheMeshFileTextureChunkCode, exceptionError);

            // if texture chunk present, read the data and assign it to the point attribute
            if((myRFMeshFile->mesh_tex_data.code == 0xCCCCCC00)) {

                // Read the number of fluids contributing to this mesh
                if(myRFMeshFile->readMeshNumFluids())
                    throw SOP_RF_Import_Exception(canNotReadTheMeshFileNumFluids, exceptionError);


                // If the user wants to import the texture data, add the uv and texture weight attributes,
                // and add number of fluids to the geo detail
                if(t_texture) {
                    char attr_str[12];

                    // Set the number of fluids attribute for this mesh in the geometry detail
//             m_num_fluids = gdp->addAttrib ("num_fluids", sizeof (float),GB_ATTRIB_FLOAT, 0);
//             *gdp->attribs().castAttribData<float>(m_num_fluids) = (float)myRFMeshFile->mesh_tex_data.num_fluids;

                    attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "num_fluids", 1);
                    if(attrRef.isValid()) {
                        attrFloatHandle.bind(attrRef.getAttribute());
                        attrFloatHandle.set(0, (float)myRFMeshFile->mesh_tex_data.num_fluids);
                    }


                    // Add the point texture "uv" attribute
//          m_texture_uv = gdp->addTextureAttribute (GEO_POINT_DICT);
                    m_texture_uv = gdp->addFloatTuple(GA_ATTRIB_POINT, "uv", 3);

                    // TODO: Get rid of this sprintf()!
                    // Loop for number of fluids, build the attribute name string, then create the point attribute with those names
                    for(int i = 0; i < myRFMeshFile->mesh_tex_data.num_fluids; i++) {
                        sprintf(attr_str, "text_wght%i", i);
//                m_text_wght[i] = gdp->addPointAttrib(attr_str,sizeof(float), GB_ATTRIB_FLOAT, 0);
                        m_text_wght[i]  = gdp->addFloatTuple(GA_ATTRIB_POINT, attr_str, 1);
                    }
                }

#ifdef DEBUG
                std::cout << "ReadRFMeshFile(): Set num of fluids in geo detail" << endl;
#endif

                // For all the vertices in the geometry, get the texture data
                for(i = 0; i < myRFMeshFile->mesh_vertex_data.num_vertices; i++) {
                    // Check to see if the user has interrupted us ...
                    if(boss->opInterrupt())
                        throw SOP_RF_Import_Exception(cookInterrupted, exceptionWarning);

                    myCurrPoint = i;

                    if(myRFMeshFile->readMeshTextureData())
                        throw SOP_RF_Import_Exception(canNotReadTheMeshFileTextureData, exceptionError);

                    // If the user want's texture data, assign it to the
                    // vertex's point attribute.
                    if(t_texture) {
                        // Get the point and the pointer to the texure attribute
                        ppt = gdp->points().entry(i);
//                    tex_map = ppt->castAttribData<UT_Vector3> (m_texture_uv);

                        // Assign the texture value to the point attribute
//                tex_map->assign ((float) myRFMeshFile->mesh_tex_data.U,
//                         (float) myRFMeshFile->mesh_tex_data.V,
//                         (float) myRFMeshFile->mesh_tex_data.W);

                        attrVector3Handle.bind(m_texture_uv.getAttribute());
                        attrVector3Handle.set(gdp->pointOffset(i), UT_Vector3(
                                                  (float)myRFMeshFile->mesh_tex_data.U,
                                                  (float) myRFMeshFile->mesh_tex_data.V,
                                                  (float) myRFMeshFile->mesh_tex_data.W));


                        // Set the texture weight values
                        for(int j = 0; j < (float) myRFMeshFile->mesh_tex_data.num_fluids; j++) {
//                   text_wght[i] = ppt->castAttribData<float>(m_text_wght[i]);
//                   *text_wght[i] = (float) myRFMeshFile->mesh_tex_data.texture_weight[i];
                            attrFloatHandle.bind(m_text_wght[j].getAttribute());
                            attrFloatHandle.set(gdp->pointOffset(i), (float) myRFMeshFile->mesh_tex_data.texture_weight[j]);
                        }
                    }
                } // for each vertice

#ifdef DEBUG
                std::cout << "ReadRFMeshFile(): Finished setting texture data in point attribute" << endl;
#endif
            }     // if (texture chunk present)


            // Read the next chunk code to see if velocity data is present
            if(myRFMeshFile->readMeshVelocityChunkCode())
                // If a file error occured when trying to read the next chunk
                throw SOP_RF_Import_Exception(canNotReadTheMeshFileVelocityChunk, exceptionError);


            // If the user wants to import the velocity data, add the velocity point attibute
            if(t_velocity)
                m_velocity_v = gdp->addVelocityAttribute(GEO_POINT_DICT);


            // If the next chunk is velocity data, read it ...
            if(myRFMeshFile->mesh_vel_data.code == 0xCCCCCC11) {
                // For all the vertices in the geometry, get the velocity data
                for(i = 0; i < myRFMeshFile->mesh_vertex_data.num_vertices; i++) {

                    // Check to see if the user has interrupted us...
                    if(boss->opInterrupt())
                        throw SOP_RF_Import_Exception(cookInterrupted, exceptionWarning);

                    myCurrPoint = i;

                    // Read the velocity data
                    if(myRFMeshFile->readMeshVelocityData())
                        throw SOP_RF_Import_Exception(canNotReadTheMeshFileVelocityData, exceptionError);

                    // If the user wants velocity data, assign it the
                    // verttex point.
                    if(t_velocity) {
//                ppt = gdp->points ().entry (i);
//                vel = ppt->castAttribData<UT_Vector3> (m_velocity_v);
//                vel->assign ((float) myRFMeshFile->mesh_vel_data.X,
//                      (float) myRFMeshFile->mesh_vel_data.Y,
//                      (float) myRFMeshFile->mesh_vel_data.Z);
                        attrVector3Handle.bind(m_velocity_v.getAttribute());
                        attrVector3Handle.set(gdp->pointOffset(i), UT_Vector3(
                                                  (float)myRFMeshFile->mesh_vel_data.X,
                                                  (float) myRFMeshFile->mesh_vel_data.Y,
                                                  (float) myRFMeshFile->mesh_vel_data.Z));
                    }
                } // for each vertice
            }     // if (velocity chunk present)


            // Read the eof marker
            if(myRFMeshFile->readMeshFileEOF())
                throw SOP_RF_Import_Exception(canNotReadTheMeshFileEOF, exceptionError);

            // Close the RF mesh file
            if(myRFMeshFile->closeMeshFile(RF_FILE_READ))
                throw SOP_RF_Import_Exception(canNotCloseRealFlowMeshFile, exceptionError);

            // We're done
            boss->opEnd();
        }

    }

    catch(SOP_RF_Import_Exception e) {
        e.what();

        if(e.getSeverity() == exceptionWarning)
            addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
        else if(e.getSeverity() == exceptionError)
            addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

        if(myRFMeshFile->RFMeshifstream.is_open()) {
            myRFMeshFile->closeMeshFile(RF_FILE_READ);
        }

        sprintf(GUI_str, "%s", "Error");
        setString((UT_String) GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);

        boss->opEnd();
        return error();
    }


    return error();
}




/**********************************************************************************/
//  $Log: SOP_RF_Import_ReadRFMeshFile.C,v $
//  Revision 1.5  2012-09-03 15:36:56  mstory
//  Fixed bug when importing mesh files & selecting texture attributes.
//
//  Revision 1.4  2012-08-31 03:00:52  mstory
//  Continued H12 mods.
//
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
