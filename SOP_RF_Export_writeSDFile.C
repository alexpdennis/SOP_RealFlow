/* ******************************************************************************
*   Real Flow
*
* $RCSfile: SOP_RF_Export_writeSDFile.C,v $
*
* Description : Write the Real Flow SD file to disk
*
* $Revision: 1.3 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export_writeSDFile.C,v $
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

#include "SOP_RF_Export_writeSDFileRestGeo.C"
#include "SOP_RF_Export_writeSDFileAnimGeo.C"


/* ******************************************************************************
*  Function Name :  writeSDFile()
*
*  Description : Write the Real Flow SD file
*
*  Input Arguments : OP_Context &context
*
*  Return Value : enumErrorList error_num
*
***************************************************************************** */
OP_ERROR SOP_RF_Export::writeSDFile(OP_Context& context)
{

    float now = context.getTime();
    char file_id_str[] = "Digital Cinema Arts (C)2008   ";
    long int rf_start_frame, rf_end_frame;
    GEO_AttributeHandle attrHandle;
    UT_String current_input_path;
    UT_Interrupt *boss;
    UT_String my_cwd, my_full_path, my_parent_full_path, my_name;
    GU_Detail *src;
    GU_Detail blank_gdp;
    UT_Matrix4 work_matrix;


    long int save_frame = context.getFrame();

    const char *date_format = "DCA v1.1.0  %y%m%d-%H:%M:%S";
    time_t datetime;
    const tm *my_tm;

    datetime = time(NULL);
    my_tm = localtime(&datetime);
    strftime(file_id_str, 32, date_format, my_tm);

#ifdef DEBUG
    std::cout << "SOP_RF_Export::writeSDFile()"  << std::endl;
    std::cout << "file_id_str: " << file_id_str << std::endl;
#endif

    // Get GUI state
    OBJ_COLOR(myObjColor, now);
    myObjXform =   OBJ_XFORM(now);


    // Set the SD filename
    myRFSDFile->myFileName = myFileName;

    // Get the number of inputs
    myNumInputs = nConnectedInputs();

#ifdef DEBUG
    std::cout << "Number of connected inputs: " << myNumInputs << std::endl;
    std::cout << "myStaticAnim " << myStaticAnim << std::endl;
    std::cout << "myMode " << myMode << std::endl;
    std::cout << "BEGIN_END " << myBeginEnd[0] << "\t" << myBeginEnd[1] << std::endl;
    std::cout << "myFileName " << myFileName << std::endl;
    std::cout << "myObjColor " << myObjColor[0] << "\t" << myObjColor[1] << "\t" << myObjColor[2] << std::endl;
    std::cout << "myEchoData " << myEchoData << std::endl;
#endif



// ********
// COPY GEO IF NO WRITE TO RF FILE
// ********

// Check to see that there hasn't been a critical error in cooking the SOP.
    if (error() < UT_ERROR_ABORT) {

        boss = UTgetInterrupt();
        // Start the interrupt server
        boss->opStart("Exporting Real Flow SD File");

        // If this cook was not inititiated by the user pressing the
        // "Write the File" button (such as turning on the display flag), do not write the file.
        if (!calledFromCallback) {
            int input_cnt = 0;
            // For each object (input)
            for (int current_obj = 0; current_obj < myNumInputs; current_obj++) {

                if (!getInput(current_obj))
                    continue;

                // lock the input
                if (lockInput(current_obj, context) < UT_ERROR_ABORT) {

                    // Get the geometry from the current input
                    if (src = (GU_Detail *) inputGeo(current_obj, context)) {
                        // Increment our input geo counter
                        input_cnt++;
                        // If first "piece" of geo, start the copying,
                        // else add the geo to the gdp
                        if (input_cnt == 1)
                            gdp->copy(*src, GEO_COPY_START);
                        else
                            gdp->copy(*src, GEO_COPY_ADD);
                    }
                    // unlock this input
                    unlockInput(current_obj);
                } else
                    break;
            }
            if (error() >= UT_ERROR_ABORT || !input_cnt) {
                gdp->stashAll();
                throw SOP_RF_Export_Exception(couldNotCopyGeo, exceptionError);
            } else {
                gdp->copy(blank_gdp, GEO_COPY_END);
            }

            boss->opEnd();
            return error();
        }


        try {

            // Check to see if the beginning and end frame range is legal
            if (myBeginEnd[1] < myBeginEnd[0])
                throw SOP_RF_Export_Exception(endFrameMustBeGreaterThanBeginningFrame, exceptionError);

#ifdef DEBUG
            std::cout << "Writing SD file" << std::endl << std::endl;
#endif


            // Reset the flag for the next button pressed event
            calledFromCallback = false;

            // Stuff the SD Object file header with values
            // TODO: get rid of this strcpy, use strstring or std::string, etc.
            strcpy(myRFSDFile->myRF_SD_Header.file_id, file_id_str);

            // SD file version (current = 6) for RF4
            myRFSDFile->myRF_SD_Header.version = 6.0;
            // Houdini server (Maya/Houdini = 5)
            myRFSDFile->myRF_SD_Header.server = 5;

            // SD file export a the SOP level does not have camera data
            myRFSDFile->myRF_SD_Header.cam_data = 0;

            // Internal use
            myRFSDFile->myRF_SD_Header.internal_use_1 = 0;
            myRFSDFile->myRF_SD_Header.internal_use_2 = 0;
            myRFSDFile->myRF_SD_Header.internal_use_3 = 0;
            myRFSDFile->myRF_SD_Header.internal_use_4 = 0;
            myRFSDFile->myRF_SD_Header.internal_use_5 = 0;
            myRFSDFile->myRF_SD_Header.internal_use_6 = 0;

            // Set the number of objects to the number of inputs to this SOP
            myRFSDFile->myRF_SD_Header.num_objects = myNumInputs;

            // TODO: Make a menu to set mode for each object, perhaps use a
            // detail attribute if present to set mode(?).
            // set the mode for matrix (0) or vertex (1)
            myRFSDFile->myRF_SD_Obj_Header.obj_mode = (int) myMode;



// *********
// CALCULATE CORRECT FRAME RANGE
// ********

            // Set the start and end frames such that they never go below zero
            long int frame_offset = abs(int (myBeginEnd[0]));

// std::cout << "frame_offset: " << frame_offset << std::endl;

            myStartFrame = int (myBeginEnd[0]);
            myEndFrame =   int (myBeginEnd[1]);

            /*
            	// If the start frame is less than zero
            	if (int (myBeginEnd[0]) < 0) {
            	    rf_start_frame = int (myBeginEnd[0]) + frame_offset;
            	    rf_end_frame = int (myBeginEnd[1]) + frame_offset;
            	}
            	// If the start frame is greater than zero
            	else {
            	    rf_start_frame = int (myBeginEnd[0] - frame_offset);
            	    rf_end_frame = int (myBeginEnd[1] - frame_offset);
            	}
            */

            // If the start frame is less than zero
            if (int (myStartFrame) < 0) {
                rf_start_frame = 0;
                rf_end_frame = int (myEndFrame) + frame_offset + 1;
            }
            // If the start frame is greater than zero
            else {
                rf_start_frame = 0;
                rf_end_frame = int (myEndFrame - frame_offset);
            }


// std::cout << "rf_start_frame: " << rf_start_frame << " rf_end_frame: " << rf_end_frame << std::endl;


            // If this "animated", set the frame range
            if (myStaticAnim && !(myStartFrame == myEndFrame)) {
                myRFSDFile->myRF_SD_Header.beg_frame = rf_start_frame;
                myRFSDFile->myRF_SD_Header.end_frame = rf_end_frame;

            }
            // else this is not "animated" ...
            else {
                myStartFrame = 0;
                myEndFrame = 1;
                myRFSDFile->myRF_SD_Header.beg_frame = 0;
                myRFSDFile->myRF_SD_Header.end_frame = 1;
            }


#ifdef DEBUG
            std::cout << "myStartFrame = " << myStartFrame << "\tmyEndFrame = " << myEndFrame << std::endl;
            std::cout << " myRFSDFile->myRF_SD_Header.beg_frame = " <<  myRFSDFile->myRF_SD_Header.beg_frame <<
                      "\tmyRFSDFile->myRF_SD_Header.end_frame = " <<  myRFSDFile->myRF_SD_Header.end_frame << std::endl;
#endif


            // Calcuate the sizes of the header and frame chunks
            if (calculateChunkSizes(context))
                throw SOP_RF_Export_Exception(canNotCalculateChunkSizes, exceptionError);

#ifdef DEBUG
            std::cout << "objectNames.length() : " << objectNames.entries() << std::endl;
            std::cout << "objectTextureNames.length(): " << objectTextureNames.entries() << std::endl;
            for (int i = 0; i < objectNames.entries(); i++) {
                std::cout << objectNames(i) << std::endl;
            }
            for (int i = 0; i < objectTextureNames.entries(); i++) {
                std::cout << objectTextureNames(i) << std::endl;
            }
#endif

            // Open the .sd file
            if (myRFSDFile->openSDFile(RF_FILE_WRITE))
                throw SOP_RF_Export_Exception(canNotOpenRealFlowSDFileForWriting, exceptionError);

            // Write the SD file header
            if (myRFSDFile->writeSDHeader())
                throw SOP_RF_Export_Exception(canNotWriteTheSDFileHeader, exceptionError);

            // Write the rest geometry
            if (SOP_RF_Export::writeSDFileRestGeo(context, boss))
                throw SOP_RF_Export_Exception(canNotWriteRestGeo, exceptionError);


            /*
             * NOTE: Camera data isn't available at the SOP level, or someone else can implement it (a field in the GUI,
             * and the code to retrieve the position and transformation matrix I suppose would be all that's needed).
            *
             * if(camera_present) {
             * myRFSDFile->writeSDCamData(0);
             * }
             */

            // Write the animated geometry
            if (SOP_RF_Export::writeSDFileAnimGeo(context, boss))
                throw SOP_RF_Export_Exception(canNotWriteAnimGeo, exceptionError);

            // We're done, close the file
            if (myRFSDFile->closeSDFile(RF_FILE_WRITE)) {
                throw SOP_RF_Export_Exception(canNotCloseTheRealFlowSDFile, exceptionError);
            }

        }

        catch (SOP_RF_Export_Exception e) {
            e.what();

            if (e.getSeverity() == exceptionWarning)
                addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
            else if (e.getSeverity() == exceptionError)
                addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

            boss->opEnd();
            unlockInputs();

            if (myRFSDFile->SDofstream.is_open()) {
                // Close the RF SD file
                if (myRFSDFile->closeSDFile(RF_FILE_WRITE)) {
                    addError(SOP_MESSAGE, "EXCEPTION: Can't close Real Flow SD file after SOP_RF_Export_Exception exception was thrown");
                    return error();
                }
            }
            return error();
        }


// Restore the saved frame
        context.setFrame((long) save_frame);

// We're done
        boss->opEnd();

    } // if(error() < UT_ERROR_ABORT)


    return error();
}



/* ******************************************************************************
*  Function Name : calculateChunkSizes()
*
*  Description :
*
*  Input Arguments : calculateChunkSizes
*
*  Return Value :
*
***************************************************************************** */
int SOP_RF_Export::calculateChunkSizes(OP_Context& context)
{
    OP_Node *current_input_node;
    UT_String current_input_path;
    GEO_AttributeHandle attrHandle;
    UT_String  tex_fname_str;

    objectNames.clear();
    objectTextureNames.clear();

#ifdef DEBUG
    std::cout << "SOP_RF_Export::calculateChunkSizes()"  << std::endl;
#endif

// **********
// CALCULATE CHUNK SIZES
// **********

    // Calculate the chunk sizes
    long int num_faces = 0;
    long int num_vertices = 0;

    // For each input, lock the inputs, get the number of faces (and vertices), unlock inputs
    for (int current_obj = 0; current_obj < myNumInputs; current_obj++) {
        if (lockInput(current_obj, context) >= UT_ERROR_ABORT) {
            std::cerr << "Could not lock input in writeSDFile() when trying to calculate the number of faces/vertices" << std::endl;
            throw SOP_RF_Export_Exception(couldNotLockInputInWriteSDFile, exceptionError);
        }

        // Duplicate the geometry from this input (NOTE: Maybe use getInput() instead?)
        duplicateSource(current_obj, context, gdp);
        num_faces += (long int) gdp->primitives().entries();
        unlockInput(current_obj);

        // Get the name of this node to assign it to the object in the SD file
        current_input_node = getInput(current_obj);
        current_input_node->getFullPath(current_input_path);
        objectNames.append(current_input_node->getName());

        // If the "sd_obj_tex_fname" detail attribute is in the geometry, store for later use.
        attrHandle = gdp->getDetailAttribute("sd_obj_tex_fname");
        if (attrHandle.isAttributeValid())
            if (attrHandle.getString(tex_fname_str))
                objectTextureNames.append(tex_fname_str);

#ifdef DEBUG
        std::cout << "Calculating number of vertices/faces: input # " << current_obj << std::endl;
        std::cout << "num_faces: " << num_faces << std::endl;
#endif

    }

    // Since the geometry *has* to be triangular polygons, the number of vertices will
    // be 3 times the number of faces (polys)
    num_vertices = (num_faces * 3);

#ifdef DEBUG
    std::cout << std::endl << "num_faces: " << num_faces << "\tnum_vertices: " << num_vertices << std::endl;
#endif

    // Calculate the length of the name strings
    long int sd_Object_Name_Size = 0;
    for (int i = 0; i < objectNames.entries(); i++)
        sd_Object_Name_Size += objectNames(i).length();

    // Calculate the length of the texture file name strings
    long int sd_Object_Texture_Filename_Size = 0;
    for (int i = 0; i < objectTextureNames.entries(); i++)
        sd_Object_Texture_Filename_Size += objectTextureNames(i).length();

    // SD header chunk size = rf_sd_header + (total_number_of_vertices * 3 * sizeof(float)) + ((rf_sd_obj_header + rf_sd_face_data) * num_objects)
    myRFSDFile->myRF_SD_Header.header_chk_size = (long int) (sdHeaderSize +
            (sd_Object_Name_Size + sd_Object_Texture_Filename_Size) +
            (num_vertices * (sizeof(float) * 3)) +
            ((sdObjectHeaderSize + sdFaceDataSize) * myNumInputs));


    // TODO: Calculate frame chunk size to accomodate mixed object modes (vertex & matrix)
    // If "matrix" mode (non-deforming geometry)
    if (!myMode)
        myRFSDFile->myRF_SD_Header.frame_chk_size = (long int) sdObjectFrameHeaderSize + sd_Object_Name_Size;
    else
        // If "vertex" mode (deforming geometry)
        myRFSDFile->myRF_SD_Header.frame_chk_size = (long int) (sdObjectFrameHeaderSize + sd_Object_Name_Size +
                ((sizeof(float) * 3) * num_vertices));

#ifdef DEBUG
    std::cout << "writeSDFile(): header_chk_size: " << myRFSDFile->myRF_SD_Header.header_chk_size << std::endl;
    std::cout << "writeSDFile(): frame_chk_size: " << myRFSDFile->myRF_SD_Header.frame_chk_size << std::endl;
#endif

    return 0;
}




/* ******************************************************************************
*  Function Name : doObjectXForm()
*
*  Description :
*
*  Input Arguments : UT_DMatrix4 & xform
*
*  Return Value :
*
***************************************************************************** */
void SOP_RF_Export::doObjectXForm(UT_DMatrix4 & xform)
{

    // Set the objects xform matrix ...
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[0] = xform(2, 2);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[1] = xform(1, 2);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[2] = xform(0, 2);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[3] = xform(2, 1);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[4] = xform(1, 1);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[5] = xform(0, 1);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[6] = xform(0, 2);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[7] = xform(0, 1);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[8] = xform(0, 0);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[9] = xform(2, 3);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[10] = xform(1, 3);
    myRFSDFile->myRF_SD_Obj_Frame_Header.obj_world_xform[11] = xform(0, 3);

}



/**********************************************************************************/
//  $Log: SOP_RF_Export_writeSDFile.C,v $
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
