/*
 ******************************************************************************
 *  Real Flow RWC File Import
 *
 * $RCSfile: SOP_RF_Import_ReadRFRWCFile.C,v $
 *
 * Description : This module implememts the function to read a Real Flow RWC file and create it's geometry
 *
 * $Revision: 1.4 $
 *
 * $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import_ReadRFRWCFile.C,v $
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

/*
 ******************************************************************************
 *  Function Name : ReadRFRWCFile()
 *
 *  Description : Read a Real Flow RWC file and create geometry
 *
 *  Input Arguments : OP_Context &context
 *
 *  Return Value : OP_ERROR
 *
 ***************************************************************************** */
OP_ERROR SOP_RF_Import::ReadRFRWCFile(OP_Context & context)
{

    float now = context.getTime();

    GEO_Point * ppt = NULL;
    GA_RWAttributeRef  p_velocity_ref;

    int t_velocity;

    UT_Interrupt * boss;
    char GUI_str[128];

    t_velocity = RWC_VELOCITY(now);

    try {

        // Check to see that there hasn't been a critical error in cooking the SOP.
        if(error() < UT_ERROR_ABORT) {
            boss = UTgetInterrupt();

            gdp->clearAndDestroy();

            // Start the interrupt server
            boss->opStart("Importing Real Flow RWC file");

#ifdef DEBUG
            std::cout << "myFileName:" << (const char *) myFileName << endl;
#endif

            // Open the Real Flow RWC file
            if(myRFRWCFile->openRWCFile((const char *) myFileName, RF_FILE_READ))
                throw SOP_RF_Import_Exception(canNotOpenRealFlowRWCFileForReading, exceptionError);

            // Read the RWC file header
            if(myRFRWCFile->readRWCFileHeader())
                throw SOP_RF_Import_Exception(canNotReadTheRWCFileHeader, exceptionError);

            GA_RWAttributeRef  attrRef;
            GA_RWHandleI attrIntHandle;
            GA_RWHandleF attrFloatHandle;
            GA_RWHandleV3 attrVector3Handle;

            // Set the position and rotation detail attributes
            attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "pos_X", 1);
            if(attrRef.isValid()) {
                attrFloatHandle.bind(attrRef.getAttribute());
                attrFloatHandle.set(0, (float)myRFRWCFile->RWC_header.RW_pos_X);
            }
            attrRef.clear();
            attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "pos_Y", 1);
            if(attrRef.isValid()) {
                attrFloatHandle.bind(attrRef.getAttribute());
                attrFloatHandle.set(0, (float)myRFRWCFile->RWC_header.RW_pos_Y);
            }
            attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "pos_Z", 1);
            if(attrRef.isValid()) {
                attrFloatHandle.bind(attrRef.getAttribute());
                attrFloatHandle.set(0, (float)myRFRWCFile->RWC_header.RW_pos_Z);
            }
            attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "rot_X", 1);
            if(attrRef.isValid()) {
                attrFloatHandle.bind(attrRef.getAttribute());
                attrFloatHandle.set(0, (float)myRFRWCFile->RWC_header.RW_rot_X);
            }
            attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "rot_Y", 1);
            if(attrRef.isValid()) {
                attrFloatHandle.bind(attrRef.getAttribute());
                attrFloatHandle.set(0, (float)myRFRWCFile->RWC_header.RW_rot_Y);
            }
            attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "rot_Z", 1);
            if(attrRef.isValid()) {
                attrFloatHandle.bind(attrRef.getAttribute());
                attrFloatHandle.set(0, (float)myRFRWCFile->RWC_header.RW_rot_Z);
            }

            // Set the "number of X & Z" detail attribute
            attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, "num_X", 1);
            if(attrRef.isValid()) {
                attrIntHandle.bind(attrRef.getAttribute());
                attrIntHandle.set(0, (int)myRFRWCFile->RWC_header.num_X_vtx);
            }
            attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, "num_Z", 1);
            if(attrRef.isValid()) {
                attrIntHandle.bind(attrRef.getAttribute());
                attrIntHandle.set(0, (int)myRFRWCFile->RWC_header.num_Z_vtx);
            }

            // Write the RWC version number to the GUI
            sprintf(GUI_str, "%s%d", "RF RWC File Version #", myRFRWCFile->RWC_header.version);
            setString((UT_String) GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);

            // If the user wants velocity attribute
            if(t_velocity)
                p_velocity_ref = gdp->addVelocityAttribute(GEO_POINT_DICT);

            myCurrPoint = 0;

            // For each point in the RWC file, create a point and assign it's position
            for(long int i = 0; i < myRFRWCFile->RWC_header.num_X_vtx; i++) {

// std::cout << "X: " << i << endl;

                // Check to see if the user has interrupted us...
                if(boss->opInterrupt())
                    throw SOP_RF_Import_Exception(cookInterrupted, exceptionWarning);

                if(!(i%2))
                    for(int j = 0; j < myRFRWCFile->RWC_header.num_Z_vtx; j++) {
                        myRFRWCFile->readRWCData();
                        ReadRFRWCFileAddPoint(ppt, t_velocity, p_velocity_ref, myCurrPoint);
// std::cout << "even Z: " << j << endl;
                    }
                else
                    for(int k = 0; k < myRFRWCFile->RWC_header.num_Z_vtx + 1; k++) {
                        myRFRWCFile->readRWCData();
                        ReadRFRWCFileAddPoint(ppt, t_velocity, p_velocity_ref, myCurrPoint);
// std::cout << "odd Z: " << k << endl;
                    }
#ifdef DEBUG
                std::cout << "RF RWC pos:" << myRFRWCFile->RWC_vtx_data.X << " " << myRFRWCFile->RWC_vtx_data.Y
                          << " " << myRFRWCFile->RWC_vtx_data.Z << endl;
#endif
            }

            // Set my total number of points from the point counter.
            myCurrPoint++;
            myTotalPoints = myCurrPoint;

            // Close the Real Flow RWC file
            if(myRFRWCFile->closeRWCFile(RF_FILE_READ))
                throw SOP_RF_Import_Exception(canNotCloseRealFlowRWCFile, exceptionError);

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

        if(myRFRWCFile->RWCifstream.is_open()) {
            myRFRWCFile->closeRWCFile(RF_FILE_READ);
        }

        boss->opEnd();
        return error();
    }


    return error();
}


/*
 ******************************************************************************
 *  Function Name : ReadRFRWCFileAddPoint()
 *
 *  Description :
 *
 *  Input Arguments : GEO_Point *ppt
 *
 *  Return Value :
 *
 ***************************************************************************** */
inline void SOP_RF_Import::ReadRFRWCFileAddPoint(GEO_Point * ppt, int t_velocity, GA_RWAttributeRef p_velocity_ref, int pt_num)
{

    GA_RWHandleV3 attrVector3Handle;

    // Append a point the geometry detail and set it's position
    ppt = gdp->appendPointElement();

    gdp->points()[pt_num]->setPos((float) myRFRWCFile->RWC_vtx_data.X,
                                  (float) myRFRWCFile->RWC_vtx_data.Y,
                                  (float) myRFRWCFile->RWC_vtx_data.Z, 1.0);

    if(t_velocity) {
        if(p_velocity_ref.isValid()) {
            attrVector3Handle.bind(p_velocity_ref.getAttribute());
            attrVector3Handle.set(gdp->pointOffset(pt_num),
                                  UT_Vector3((float) myRFRWCFile->RWC_vel_data.X,
                                             (float) myRFRWCFile->RWC_vel_data.Y,
                                             (float) myRFRWCFile->RWC_vel_data.Z));
        }

//         vel = ppt->castAttribData<UT_Vector3>(p_velocity_v);
//         vel->assign ((float) myRFRWCFile->RWC_vel_data.X,
//               (float) myRFRWCFile->RWC_vel_data.Y,
//               (float) myRFRWCFile->RWC_vel_data.Z);
    }

}



/**********************************************************************************/
// $Log: SOP_RF_Import_ReadRFRWCFile.C,v $
// Revision 1.4  2012-08-31 03:00:51  mstory
// Continued H12 mods.
//
// Revision 1.3  2012-08-29 03:07:06  mstory
// Initial changes for H12.
//
// Revision 1.2  2011-02-11 04:18:01  mstory
// Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...
//
//
// --mstory
//
// Revision 1.1.1.1  2009-01-30 02:24:05  mstory
// Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.
//
//
//
/**********************************************************************************/
