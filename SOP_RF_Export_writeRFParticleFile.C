/* ******************************************************************************
*  Real Flow particle BIN2 file exporter for Houdini
*
* $RCSfile: SOP_RF_Export_writeRFParticleFile.C,v $
*
*  Description : Write a Real Flow BIN2 particle file to disk from the incoming points
*
* $Revision: 1.5 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Export_writeRFParticleFile.C,v $
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
*  Function Name :  writeBINFile()
*
*  Description : Write the Real Flow BIN2 particle file(s)
*
*  Input Arguments : OP_Context &context
*
*  Return Value : enumErrorList error_num
*
***************************************************************************** */

OP_ERROR SOP_RF_Export::writeBINFile(OP_Context &context) {

   float now = context.getTime();

    GA_ROAttributeRef v_ref, force_ref, vorticity_ref, N_ref, num_neighbors_ref, uv_ref, info_bits_ref,
        age_ref, isolation_time_ref, viscosity_ref, density_ref, pressure_ref,
        mass_ref, temperature_ref, id_ref;

    UT_Vector3   v_vec(0.0, 0.0, 0.0);
    UT_Vector3   force_vec(0.0, 0.0, 0.0);
    UT_Vector3   vorticity_vec(0.0, 0.0, 0.0);
    UT_Vector3   uv_vec(0.0, 0.0, 0.0);
    UT_Vector3   N_vec(0.0, 0.0, 0.0);

    int        cur_frame;
    float      emitter_pos[3], *emitter_pos_ptr = emitter_pos;
    float      emitter_rot[3], *emitter_rot_ptr = emitter_rot;
    float      emitter_scl[3], *emitter_scl_ptr = emitter_scl;
    long int   save_frame = context.getFrame();
    UT_Vector3 normal, p;
    UT_Vector4 pos;
//    GEO_Point  *ppt;
//    char       frame_str[32];
    long int   part_num = 0;
    UT_Vector3 foo_vec;
    UT_Interrupt *boss = UTgetInterrupt();
    UT_String  export_stat_str = "";
    long int   frame_offset, start_frame, end_frame;

    // If this cook was not inititiated by the user pressing the
    // "Write the File" button (the display flag was set), do not write the file.
    if(!calledFromCallback) {
        if (lockInputs(context) >= UT_ERROR_ABORT)
            throw SOP_RF_Export_Exception(canNotLockInputsInWriteBINFile, exceptionError);

        // Duplicate the geometry from the first input
        duplicateSource(0, context);
        unlockInputs();
        // Restore the frame
         context.setFrame((long)save_frame);
         return error();
    }

      // If not called by the callback (to write the file, duplicate incoming geo so the attributes can be found
      // without having to turn on the OP's display flag
   else {
        if (lockInputs(context) >= UT_ERROR_ABORT)
            throw SOP_RF_Export_Exception(canNotLockInputsInWriteBINFile, exceptionError);

        // Duplicate the geometry from the first input
        duplicateSource(0, context);
        unlockInputs();
   }


/*

CH_Manager *mgr = OPgetDirector()->getChannelManager()
mgr->getEvaluateTime(); // current evaluation time
mgr->getSamplesPerSec(); // get FPS
mgr->getGlobalStart(); // get global animation start time
mgr->getGlobalEnd(); // get global animation end time

*/

    // Reset the flag for the next button pressed event
    calledFromCallback = false;


try {

    // Evaluate the GUI parameters
    FNAME(myFileName, now);
    // Get the fluid type and assign the correct fluid type value
    FLUID_NAME(myFluidName, now);
    myFluidType =  FLUID_TYPE(now);
    myRadius = RADIUS(now);
    mySceneScale = SCENE_SCALE(now);
    myPressureMin = PRESSURE_MIN(now);
    myPressureMax = PRESSURE_MAX(now);
    myPressureAvg = PRESSURE_AVG(now);
    mySpeedMin = SPEED_MIN(now);
    mySpeedMax = SPEED_MAX(now);
    mySpeedAvg = SPEED_AVG(now);
    myTempMin = TEMP_MIN(now);
    myTempMax = TEMP_MAX(now);
    myTempAvg = TEMP_AVG(now);
    EMITTER_POS(emitter_pos_ptr, now);
    EMITTER_ROT(emitter_rot_ptr, now);
    EMITTER_SCALE(emitter_scl_ptr, now);
    myEchoData = ECHO_CONSOLE(now);

    /*  RF3 fluid types
    #define GAS 1
    #define LIQUID 8
    #define DUMB 5
    #define ELASTICS 7
    */

   switch (myFluidType) {
      case 0:
         myFluidType = 1;
         break;
      case 1:
         myFluidType = 8;
         break;
      case 2:
         myFluidType = 5;
         break;
      case 3:
         myFluidType = 7;
         break;
      default:
         myFluidType = 1;
    }


    myRFBINFile->myFileName = myFileName;

#ifdef DEBUG
std::cout << "myStaticAnim " << myStaticAnim << std::endl;
std::cout << "Begin/End " << myBeginEnd[0] << "\t" << myBeginEnd[1] << std::endl;
std::cout << "myFileName " << myFileName << std::endl;
std::cout << "myFluidName " << myFluidName << std::endl;
std::cout << "myFPS " << myFPS << std::endl;
std::cout << "myFluidType " << myFluidType << std::endl;
std::cout << "mySceneScale " << mySceneScale << std::endl;
std::cout << "myRadius " << myRadius << std::endl;
std::cout << "Pressure: " << myPressureMin << " " << myPressureMax << " " << myPressureAvg << std::endl;
std::cout << "Speed: " << mySpeedMin << " " << mySpeedMax << " " << mySpeedAvg << std::endl;
std::cout << "Temperature: " << myTempMin << " " << myTempMax << " " << myTempAvg << std::endl;
std::cout << "Emitter Position: " << emitter_pos[0] << " " << emitter_pos[1] << " " << emitter_pos[2] << std::endl;
std::cout << "Emitter Rotation: " << emitter_rot[0] << " " << emitter_rot[1] << " " << emitter_rot[2] << std::endl;
std::cout << "Emitter Scale: " << emitter_scl[0] << " " << emitter_scl[1] << " " << emitter_scl[2] << std::endl;
std::cout << "myEchoData " << myEchoData << std::endl;
#endif


    frame_offset = abs(int(myBeginEnd[0]));

    if(int(myBeginEnd[0]) < 0) {
        start_frame = int(myBeginEnd[0]) + frame_offset;
        end_frame = int(myBeginEnd[1]) + frame_offset;
    }
    else {
        start_frame = int(myBeginEnd[0]);
        end_frame = int(myBeginEnd[1]);
    }

#ifdef DEBUG
std::cout << "start_frame: " << start_frame << std::endl;
std::cout << "end_frame: " << end_frame << std::endl;
std::cout << "frame_offset: " << frame_offset << std::endl;
#endif

   // Write the "frame 0" file
   myRFBINFile->part_header.verify_code = 0x00FABADA;
   strcpy(myRFBINFile->part_header.fluid_name, myFluidName);

   myRFBINFile->part_header.version = 11;
   myRFBINFile->part_header.scene_scale = mySceneScale;
   myRFBINFile->part_header.fluid_type = myFluidType;
   myRFBINFile->part_header.elapsed_time = 0.0;
   myRFBINFile->part_header.frame_number = 0;
   myRFBINFile->part_header.fps = myFPS;
   myRFBINFile->part_header.num_particles = 0;
   myRFBINFile->part_header.radius = myRadius;
   myRFBINFile->part_header.pressure[0] = myPressureMax;
   myRFBINFile->part_header.pressure[1] = myPressureMin;
   myRFBINFile->part_header.pressure[2] = myPressureAvg;
   myRFBINFile->part_header.speed[0] = mySpeedMax;
   myRFBINFile->part_header.speed[1] = mySpeedMin;
   myRFBINFile->part_header.speed[2] = mySpeedAvg;
   myRFBINFile->part_header.temperature[0] = myTempMax;
   myRFBINFile->part_header.temperature[1] = myTempMin;
   myRFBINFile->part_header.temperature[2] = myTempAvg;
   myRFBINFile->part_header.emitter_pos[0] = emitter_pos[0];
   myRFBINFile->part_header.emitter_pos[1] = emitter_pos[1];
   myRFBINFile->part_header.emitter_pos[2] = emitter_pos[2];
   myRFBINFile->part_header.emitter_rot[0] = emitter_rot[0];
   myRFBINFile->part_header.emitter_rot[1] = emitter_rot[1];
   myRFBINFile->part_header.emitter_rot[2] = emitter_rot[2];
   myRFBINFile->part_header.emitter_scale[0] = emitter_scl[0];
   myRFBINFile->part_header.emitter_scale[1] = emitter_scl[1];
   myRFBINFile->part_header.emitter_scale[2] = emitter_scl[2];

   // Create the "0" frame file name
   std::string tmp_str((const char *)myFileName) ;

   int found=tmp_str.find_last_of("0123456789");
   while (found!=string::npos) {
      tmp_str[found]='0';
      found=tmp_str.find_first_of("0123456789",found+1);
   }

    // Open the Real Flow particle BIN2 file
    if(myRFBINFile->open_part_file((char *)tmp_str.c_str(), RF_FILE_WRITE))
      throw SOP_RF_Export_Exception(writeBINFileOpenRFFileErr, exceptionError);

    // Write Real Flow particle BIN2 header
    if(myRFBINFile->write_part_file_header())
      throw SOP_RF_Export_Exception(canNotBINFileWriteHeaderFrame0, exceptionError);

//    // Write the "additional data" record
//    if(myRFBINFile->write_additional_data())
//      throw SOP_RF_Export_Exception(canNotWriteTheAdditionalDataSectionFrame0, exceptionError);

    // Close the Real Flow particle file
    if(myRFBINFile->close_part_file(RF_FILE_WRITE))
      throw SOP_RF_Export_Exception(canNotCloseTheRealFlowBINFileFrame0, exceptionError);


    float timestep = 0.1;
    GA_ROHandleV3       v_h(gdp, GEO_POINT_DICT, "v");
    GA_RWHandleV3       p_h(gdp->getP());

    if (v_h.isValid() && p_h.isValid())
    {
        for (GA_Iterator it(gdp->getPointRange()); !it.atEnd(); ++it)
            p_h.add(it.getOffset(), v_h.get(it.getOffset()) * timestep);
    }



    // For each frame in our animation ...
    for (cur_frame = start_frame; cur_frame <= end_frame; cur_frame++) {

      boss->opStart("Exporting Geometry");

      std::cout << "Real Flow Export Particles-cur_frame: " << cur_frame << "\tend_frame: " << end_frame << std::endl;

      // Set the current frame
      context.setFrame((long)cur_frame);

      if (lockInputs(context) >= UT_ERROR_ABORT)
         throw SOP_RF_Export_Exception(couldNotLockInputInWriteBINFile, exceptionError);

      if (boss->opInterrupt())
         throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);

        // Check to see that there hasn't been a critical error in cooking the SOP.
        if (error() < UT_ERROR_ABORT) {

            // Get current time and duplicate the geometry
            now = context.getTime();
            duplicateSource(0, context);

            // Get the parameters that may change on a point by point/frame to frame basis
            FNAME(myFileName, now);
            mySceneScale = SCENE_SCALE(now);
            myRadius = RADIUS(now);
            myPressureMin = PRESSURE_MIN(now);
            myPressureMax = PRESSURE_MAX(now);
            myPressureAvg = PRESSURE_AVG(now);
            mySpeedMin = SPEED_MIN(now);
            mySpeedMax = SPEED_MAX(now);
            mySpeedAvg = SPEED_AVG(now);
            myTempMin = TEMP_MIN(now);
            myTempMax = TEMP_MAX(now);
            myTempAvg = TEMP_AVG(now);
            EMITTER_POS(emitter_pos_ptr, now);
            EMITTER_ROT(emitter_rot_ptr, now);
            EMITTER_SCALE(emitter_scl_ptr, now);

            // Set the file type id
            myRFBINFile->part_header.verify_code = 0x00FABADA;

            // Set the fluid name
            strcpy(myRFBINFile->part_header.fluid_name, myFluidName);

            myRFBINFile->part_header.version = 11;

            myRFBINFile->part_header.scene_scale = mySceneScale;
            myRFBINFile->part_header.fluid_type = myFluidType;

            // TODO: Get the FPS to properly calculate the elapsed time
            myRFBINFile->part_header.elapsed_time = cur_frame/(float)myFPS;
            myRFBINFile->part_header.frame_number = cur_frame;

            myRFBINFile->part_header.fps = myFPS;

            myRFBINFile->part_header.num_particles = (long int)gdp->points().entries();

            myRFBINFile->part_header.radius = myRadius;
            myRFBINFile->part_header.pressure[0] = myPressureMax;
            myRFBINFile->part_header.pressure[1] = myPressureMin;
            myRFBINFile->part_header.pressure[2] = myPressureAvg;
            myRFBINFile->part_header.speed[0] = mySpeedMax;
            myRFBINFile->part_header.speed[1] = mySpeedMin;
            myRFBINFile->part_header.speed[2] = mySpeedAvg;
            myRFBINFile->part_header.temperature[0] = myTempMax;
            myRFBINFile->part_header.temperature[1] = myTempMin;
            myRFBINFile->part_header.temperature[2] = myTempAvg;
            myRFBINFile->part_header.emitter_pos[0] = emitter_pos[0];
            myRFBINFile->part_header.emitter_pos[1] = emitter_pos[1];
            myRFBINFile->part_header.emitter_pos[2] = emitter_pos[2];
            myRFBINFile->part_header.emitter_rot[0] = emitter_rot[0];
            myRFBINFile->part_header.emitter_rot[1] = emitter_rot[1];
            myRFBINFile->part_header.emitter_rot[2] = emitter_rot[2];
            myRFBINFile->part_header.emitter_scale[0] = emitter_scl[0];
            myRFBINFile->part_header.emitter_scale[1] = emitter_scl[1];
            myRFBINFile->part_header.emitter_scale[2] = emitter_scl[2];

            myRFBINFile->myFileName = myFileName;

            // Reset the particle number
            part_num = myRFBINFile->part_header.num_particles;

            // Open the Real Flow particle BIN2 file
            if(myRFBINFile->open_part_file((char *)myFileName, RF_FILE_WRITE))
               throw SOP_RF_Export_Exception(canNotOpenTheRealFlowParticleBINFile, exceptionError);

            // Write the header record
            if(myRFBINFile->write_part_file_header())
               throw SOP_RF_Export_Exception(canNotWriteHeaderParticleBINFile, exceptionError);


            GA_FOR_ALL_GPOINTS_NC (gdp, GEO_Point, ppt) {

               if (boss->opInterrupt())
                  throw SOP_RF_Export_Exception(cookInterrupted, exceptionWarning);



    v_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "v", 3);
    if (v_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleVel, exceptionError);

    force_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "force", 3);
    if (force_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleForce, exceptionError);

    N_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "N", 3);
    if (N_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleNormal, exceptionError);

    vorticity_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "vorticity", 3, 3);
    if (vorticity_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleVorticity, exceptionError);

    uv_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "uv", 3);
    if (uv_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleUV, exceptionError);

    info_bits_ref = gdp->findIntTuple(GA_ATTRIB_POINT, "info_bits", 1, 1);
    if (info_bits_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleInfoBits, exceptionError);

    age_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "age", 1, 1);
    if (age_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleAge, exceptionError);

    isolation_time_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "isolation_time", 1, 1);
    if (isolation_time_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleIsolation, exceptionError);

    viscosity_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "rf_viscosity", 1, 1);
    if (viscosity_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleViscosity, exceptionError);

    pressure_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "pressure", 1, 1);
    if (pressure_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandlePressure, exceptionError);

    density_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "rf_density", 1, 1);
    if (density_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleDensity, exceptionError);

    mass_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "mass", 1, 1);
    if (mass_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleMass, exceptionError);

    temperature_ref = gdp->findFloatTuple(GA_ATTRIB_POINT, "temperature", 1, 1);
    if (temperature_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleTemperature, exceptionError);

    num_neighbors_ref = gdp->findIntTuple(GA_ATTRIB_POINT, "num_neighbors", 1, 1);
    if (num_neighbors_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleNumNeighbors, exceptionError);

    id_ref = gdp->findIntTuple(GA_ATTRIB_POINT, "id", 1, 1);
    if (id_ref.isInvalid())
      throw SOP_RF_Export_Exception(invalidAttrHandleID, exceptionError);

                // Set the particle data structure
                pos = ppt->getPos();
                myRFBINFile->part_data.pos[0] = static_cast<float>(pos.x());
                myRFBINFile->part_data.pos[1] = static_cast<float>(pos.y());
                myRFBINFile->part_data.pos[2] = static_cast<float>(pos.z());

               // Add the various attributes to export to Real Flow
                    v_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(v_ref));
                    myRFBINFile->part_data.vel[0] = static_cast<float>(v_vec.x());
                    myRFBINFile->part_data.vel[1] = static_cast<float>(v_vec.y());
                    myRFBINFile->part_data.vel[2] = static_cast<float>(v_vec.z());

                    force_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(force_ref));
                    myRFBINFile->part_data.force[0] = static_cast<float>(force_vec.x());
                    myRFBINFile->part_data.force[1] = static_cast<float>(force_vec.y());
                    myRFBINFile->part_data.force[2] = static_cast<float>(force_vec.z());

                    vorticity_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(vorticity_ref));
                    myRFBINFile->part_data.vorticity[0] = static_cast<float>(vorticity_vec.x());
                    myRFBINFile->part_data.vorticity[1] = static_cast<float>(vorticity_vec.y());
                    myRFBINFile->part_data.vorticity[2] = static_cast<float>(vorticity_vec.z());

                    N_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(N_ref));
                    myRFBINFile->part_data.normal[0] = static_cast<float>(N_vec.x());
                    myRFBINFile->part_data.normal[1] = static_cast<float>(N_vec.y());
                    myRFBINFile->part_data.normal[2] = static_cast<float>(N_vec.z());

                    myRFBINFile->part_data.num_neighbors = static_cast<int>(ppt->getValue<int>(num_neighbors_ref));

                    uv_vec = static_cast<UT_Vector3>(ppt->getValue<UT_Vector3>(uv_ref));
                    myRFBINFile->part_data.texture_vector[0] = static_cast<float>(uv_vec.x());
                    myRFBINFile->part_data.texture_vector[1] = static_cast<float>(uv_vec.y());
                    myRFBINFile->part_data.texture_vector[2] = static_cast<float>(uv_vec.z());

                    myRFBINFile->part_data.info_bits = static_cast<short int>(ppt->getValue<int>(info_bits_ref));
                    myRFBINFile->part_data.age = static_cast<float>(ppt->getValue<float>(age_ref));
                    myRFBINFile->part_data.isolation_time = static_cast<float>(ppt->getValue<float>(isolation_time_ref));
                    myRFBINFile->part_data.viscosity = static_cast<float>(ppt->getValue<float>(viscosity_ref));
                    myRFBINFile->part_data.density = static_cast<float>(ppt->getValue<float>(density_ref));
                    myRFBINFile->part_data.pressure = static_cast<float>(ppt->getValue<float>(pressure_ref));
                    myRFBINFile->part_data.mass = static_cast<float>(ppt->getValue<float>(mass_ref));
                    myRFBINFile->part_data.temperature = static_cast<float>(ppt->getValue<float>(temperature_ref));
                    myRFBINFile->part_data.id = static_cast<int>(ppt->getValue<int>(id_ref));

                // Write particle data to disk
                if(myRFBINFile->write_part_data())
                  throw SOP_RF_Export_Exception(canNotWriteParticleDataToBINFile, exceptionError);
            }

//
//            if(myRFBINFile->write_additional_data())
//               throw SOP_RF_Export_Exception(canNotWriteTheAdditionalDataSection, exceptionError);

            // We're done, close the file
            if(myRFBINFile->close_part_file(RF_FILE_WRITE))
               throw SOP_RF_Export_Exception(canNotCloseTheRealFlowBINFile, exceptionError);

      }

      // We're done with this frame
      boss->opEnd();
      unlockInputs();

    } // for (cur_frame)

 }

// Exception handler
 catch (SOP_RF_Export_Exception e) {
   e.what();

   if(e.getSeverity() == exceptionWarning)
      addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
   else if(e.getSeverity() == exceptionError)
      addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

   boss->opEnd();
   unlockInputs();
   context.setFrame((long)save_frame);

   if(myRFBINFile->RFPartofstream.is_open()) {
      // Close the RF particle file
      if(myRFBINFile->close_part_file(RF_FILE_WRITE)) {
         addError(SOP_MESSAGE, "Can't close Real Flow particle file after SOP_RF_Export_Exception exception was thrown");
         return error();
      }
   }
   return error();
}


   // Restore the frame
   context.setFrame((long)save_frame);


return error();
}


/**********************************************************************************/
//  $Log: SOP_RF_Export_writeRFParticleFile.C,v $
//  Revision 1.5  2012-09-03 15:36:55  mstory
//  Fixed bug when importing mesh files & selecting texture attributes.
//
//  Revision 1.4  2012-08-31 03:00:51  mstory
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
