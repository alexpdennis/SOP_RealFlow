/* ******************************************************************************
*  Real Flow Particle Geometry File Import
*
* $RCSfile: SOP_RF_Import_ReadRFParticleFile.C,v $
*
* Description : This module implememts the function to read a Real Flow particle file and create it's geometry
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/SOP_RF_Import_ReadRFParticleFile.C,v $
*
* $Author: mstory $
*
* See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2004
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */

/* ******************************************************************************
*  Function Name : ReadRFParticleFile()
*
*  Description : Read a Real Flow particle file and create points and their
*                   attributes
*
*  Input Arguments : OP_Context &context
*
*  Return Value : OP_ERROR
*
***************************************************************************** */
OP_ERROR SOP_RF_Import::ReadRFParticleFile(OP_Context & context)
{

   float now = context.getTime();

   GEO_Point * ppt;
   UT_Interrupt * boss;
   char GUI_str[128];

   myGUIState.t_velocity = static_cast<bool>(VELOCITY(now));
   myGUIState.t_force = static_cast<bool>(FORCE(now));
   myGUIState.t_vorticity = static_cast<bool>(VORTICITY(now));
   myGUIState.t_normal = static_cast<bool>(NORMAL(now));
   myGUIState.t_num_neighbors = static_cast<bool>(NUM_NEIGHBORS(now));
   myGUIState.t_texture_vector = static_cast<bool>(TEX_VECTOR(now));
   myGUIState.t_info_bits = static_cast<bool>(INFO_BITS(now));
   myGUIState.t_age = static_cast<bool>(AGE(now));
   myGUIState.t_isolation_time = static_cast<bool>(ISO_TIME(now));
   myGUIState.t_viscosity = static_cast<bool>(VISCOSITY(now));
   myGUIState.t_density = static_cast<bool>(DENSITY(now));
   myGUIState.t_pressure = static_cast<bool>(PRESSURE(now));
   myGUIState.t_mass = static_cast<bool>(MASS(now));
   myGUIState.t_temperature = static_cast<bool>(TEMPERATURE(now));
   myGUIState.t_id = static_cast<bool>(PARTICLE_ID(now));


   try {

         // Check to see that there hasn't been a critical error in cooking the SOP.
         if(error() < UT_ERROR_ABORT) {

               boss = UTgetInterrupt();

               gdp->clearAndDestroy();

               // Start the interrupt server
               boss->opStart("Importing Real Flow Particles");

#ifdef DEBUG
               std::cout << "myFileName:" << (const char *)myFileName << endl;
#endif

               // Open the Real Flow Particle file
               if(myRFParticleFile->open_part_file((char *)myFileName, RF_FILE_READ)) {
                     sprintf(GUI_str, "%s", "Error");
                     setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);
                     throw SOP_RF_Import_Exception(canNotOpenRealFlowParticleFileForReading, exceptionError);
                  }

               // Read the header
               if(myRFParticleFile->read_part_file_header())
                  throw SOP_RF_Import_Exception(canNotReadTheParticleFileHeader, exceptionError);


               // Set the version number of this particle file.
               myParticleVersion = myRFParticleFile->part_header.version;

               // TODO: get rid of this sprintf
               // Write the version number to the GUI
               sprintf(GUI_str, "%s%d", "Particle File Version #", myRFParticleFile->part_header.version);
               setString((UT_String)GUI_str, CH_STRING_LITERAL, ARG_RF_IMPORT_VER, 0, now);

               // Create the attributes
               ReadRFParticleFileCreateAttrs();

               // Set the total umber of particles (points).
               myTotalPoints = myRFParticleFile->part_header.num_particles;

               // Loop through all the particles and create a point for each particle
               // and if the attribute is needed, then set it's value
               for(int i = 0; i < myRFParticleFile->part_header.num_particles; i++) {
                     // Check to see if the user has interrupted us...
                     if(boss->opInterrupt())
                        throw SOP_RF_Import_Exception(cookInterrupted, exceptionWarning);

                     myCurrPoint = i;

#ifdef DEBUG
                     std::cout << "myCurrPoint: "  << myCurrPoint << endl;
                     std::cout << "sizeof(RealFlow_Particle_File::part_data * myCurrPoint): "
                               << (sizeof(myRFParticleFile->part_data) * myCurrPoint) << "\t" << myCurrPoint << endl;
#endif

                     // Read the particle data from the file (an excption has been thrown by the real flow lib finction read_part_data())
                     if(myRFParticleFile->read_part_data())
                        throw SOP_RF_Import_Exception(canNotReadRealFlowParticleData, exceptionError);

#ifdef DEBUG
                     std::cout << "RF Particle pos:"
                               << myRFParticleFile->part_data.pos[0] << " "
                               << myRFParticleFile->part_data.pos[1] << " "
                               << myRFParticleFile->part_data.pos[2] << endl;
#endif

                     // Append a point the geometry detail and set it's position
                     ppt = gdp->appendPointElement();
                     gdp->points()[i]->setPos((float)myRFParticleFile->part_data.pos[0],
                                              (float)myRFParticleFile->part_data.pos[1],
                                              (float)myRFParticleFile->part_data.pos[2], 1.0);

                     // Now go through all the attributes and if they are needed, set it's value
                     ReadRFParticleFileSetAttrs(i);

                     // Select the geometry
                     select(*ppt, 1, 1);
                  }

               // Close the Real Flow particle file
               if(myRFParticleFile->close_part_file(RF_FILE_READ)) {
                     addError(SOP_MESSAGE, "Can't close Real Flow Particle file");
                     throw SOP_RF_Import_Exception(canNotCloseRealFlowParticleFile, exceptionError);
                  }


// Select the geometry
//     select(GU_SPrimitive);

               // We're done, tell Houdini
               boss->opEnd();
            }

      }

   catch(SOP_RF_Import_Exception e) {
         e.what();

         if(e.getSeverity() == exceptionWarning)
            addWarning(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);
         else
            if(e.getSeverity() == exceptionError)
               addError(SOP_MESSAGE, errorMsgs[e.getErrorCode()]);

         if(myRFParticleFile->RFPartifstream.is_open()) {
               myRFParticleFile->close_part_file(RF_FILE_READ);
            }

         boss->opEnd();
         return error();
      }

   return error();
}



/* ******************************************************************************
*  Function Name : ReadRFParticleFileCreateAttrs()
*
*  Description : Set attributes
*
*  Input Arguments :
*
*  Return Value : int
*
***************************************************************************** */

inline int SOP_RF_Import::ReadRFParticleFileCreateAttrs()
{

//   UT_Vector3  *emit_pos, *emit_rot, *emit_scale;
   GA_RWAttributeRef attrRef;
   GA_RWHandleI attrIntHandle;
   GA_RWHandleF attrFloatHandle;
   GA_RWHandleV3 attrVector3Handle;

   // Create the various point attribute the user wants to read
   if(myGUIState.t_velocity)
      myAttributeRefs.p_velocity = gdp->addVelocityAttribute(GEO_POINT_DICT);

   if(myGUIState.t_force)
      myAttributeRefs.p_force = gdp->addFloatTuple(GA_ATTRIB_POINT, "force", 3);

   if(myGUIState.t_vorticity && (myRFParticleFile->part_header.version >= 9))
      myAttributeRefs.p_vorticity = gdp->addFloatTuple(GA_ATTRIB_POINT, "vorticity", 3);

   if(myGUIState.t_normal)
      myAttributeRefs.p_normal_N = gdp->addFloatTuple(GA_ATTRIB_POINT, "N", 3);


   if(myGUIState.t_num_neighbors)
      myAttributeRefs.p_num_neighbors = gdp->addIntTuple(GA_ATTRIB_POINT, "num_neighbors", 1);

   if(myGUIState.t_texture_vector)
      myAttributeRefs.p_texture_vector_uv = gdp->addFloatTuple(GA_ATTRIB_POINT, "uv", 3);

   if(myGUIState.t_info_bits)
      myAttributeRefs.p_info_bits = gdp->addIntTuple(GA_ATTRIB_POINT, "info_bits", 1);

   if(myGUIState.t_age)
      myAttributeRefs.p_age = gdp->addFloatTuple(GA_ATTRIB_POINT, "age", 1);

   if(myGUIState.t_isolation_time)
      myAttributeRefs.p_isolation_time = gdp->addFloatTuple(GA_ATTRIB_POINT, "isolation_time", 1);

   if(myGUIState.t_viscosity)
      myAttributeRefs.p_viscosity = gdp->addFloatTuple(GA_ATTRIB_POINT, "viscosity", 1);

   if(myGUIState.t_density)
      myAttributeRefs.p_density = gdp->addFloatTuple(GA_ATTRIB_POINT, "density", 1);

   if(myGUIState.t_pressure)
      myAttributeRefs.p_pressure = gdp->addFloatTuple(GA_ATTRIB_POINT, "pressure", 1);

   if(myGUIState.t_mass)
      myAttributeRefs.p_mass = gdp->addFloatTuple(GA_ATTRIB_POINT, "mass", 1);

   if(myGUIState.t_temperature)
      myAttributeRefs.p_temperature = gdp->addFloatTuple(GA_ATTRIB_POINT, "temperature", 1);

   if(myGUIState.t_id)
      myAttributeRefs.p_id = gdp->addIntTuple(GA_ATTRIB_POINT, "id", 1);


   // Assign the header data to the geometry's detail
   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "scene_scale", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.scene_scale);
      }

   attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, "fluid_type", 1);
   if(attrRef.isValid()) {
         attrIntHandle.bind(attrRef.getAttribute());
         attrIntHandle.set(0, (int)myRFParticleFile->part_header.fluid_type);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "elapsed_time", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.elapsed_time);
      }

   attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, "frame_number", 1);
   if(attrRef.isValid()) {
         attrIntHandle.bind(attrRef.getAttribute());
         attrIntHandle.set(0, (int)myRFParticleFile->part_header.frame_number);
      }

   attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, "fps", 1);
   if(attrRef.isValid()) {
         attrIntHandle.bind(attrRef.getAttribute());
         attrIntHandle.set(0, (float)myRFParticleFile->part_header.fps);
      }

   attrRef = gdp->addIntTuple(GA_ATTRIB_DETAIL, "p_num_particles", 1);
   if(attrRef.isValid()) {
         attrIntHandle.bind(attrRef.getAttribute());
         attrIntHandle.set(0, (int)myRFParticleFile->part_header.num_particles);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "radius", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.radius);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "min_pressure", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.pressure[1]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "max_pressure", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.pressure[0]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "avg_pressure", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.pressure[2]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "min_speed", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.speed[1]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "max_speed", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.speed[0]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "avg_speed", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.speed[2]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "min_temperature", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.temperature[1]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "max_temperature", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.temperature[0]);
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "avg_temperature", 1);
   if(attrRef.isValid()) {
         attrFloatHandle.bind(attrRef.getAttribute());
         attrFloatHandle.set(0, (float)myRFParticleFile->part_header.temperature[2]);
      }

   // Assign emmiter's position rotation and scale to geometry's detail.
   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "emit_pos", 3);
   if(attrRef.isValid()) {
         attrVector3Handle.bind(attrRef.getAttribute());
         attrVector3Handle.set(0, UT_Vector3(myRFParticleFile->part_header.emitter_pos));
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "emit_rot", 3);
   if(attrRef.isValid()) {
         attrVector3Handle.bind(attrRef.getAttribute());
         attrVector3Handle.set(0, UT_Vector3(myRFParticleFile->part_header.emitter_rot));
      }

   attrRef = gdp->addFloatTuple(GA_ATTRIB_DETAIL, "emit_scale", 3);
   if(attrRef.isValid()) {
         attrVector3Handle.bind(attrRef.getAttribute());
         attrVector3Handle.set(0, UT_Vector3(myRFParticleFile->part_header.emitter_scale));
      }

#ifdef DEBUG
   std::cout << "emit_pos: "  << myRFParticleFile->part_header.emitter_pos[0] << " " << myRFParticleFile->part_header.emitter_pos[1] << " "
             << myRFParticleFile->part_header.emitter_pos[2] << endl;
   std::cout << "emit_rot: "  << myRFParticleFile->part_header.emitter_rot[0] << " " << myRFParticleFile->part_header.emitter_rot[1] << " "
             << myRFParticleFile->part_header.emitter_rot[2] << endl;
   std::cout << "emit_scale: "  << myRFParticleFile->part_header.emitter_scale[0] << " " << myRFParticleFile->part_header.emitter_scale[1] << " "
             << myRFParticleFile->part_header.emitter_scale[2] << endl;
#endif


   return 0;
}




/* ******************************************************************************
*  Function Name : ReadRFParticleFileSetAttrs()
*
*  Description : Set attributes
*
*  Input Arguments : GEO_Point *ppt
*
*  Return Value : int
*
***************************************************************************** */
inline int SOP_RF_Import::ReadRFParticleFileSetAttrs(long int pt_num)
{

//   UT_Vector3  *vel, *force, *vorticity, *normal, *texture_vector;
// float       *age, *isolation_time, *viscosity, *density, *pressure,
//             *mass, *temperature;
// int         *num_neighbors, *info_bits, *id;


   GA_RWAttributeRef attrRef;
   GA_RWHandleI attrIntHandle;
   GA_RWHandleF attrFloatHandle;
   GA_RWHandleV3 attrVector3Handle;


#ifdef DEBUG
   std::cout << "SOP_RF_Import::ReadRFParticleFileSetAttrs()" << std::endl;
#endif

   if(myGUIState.t_velocity) {
         if(myAttributeRefs.p_velocity.isValid()) {
               attrVector3Handle.bind(myAttributeRefs.p_velocity.getAttribute());
               attrVector3Handle.set(gdp->pointOffset(pt_num), UT_Vector3(myRFParticleFile->part_data.vel));
            }
      }

   if(myGUIState.t_force) {
         if(myAttributeRefs.p_force.isValid()) {
               attrVector3Handle.bind(myAttributeRefs.p_force.getAttribute());
               attrVector3Handle.set(gdp->pointOffset(pt_num), UT_Vector3(myRFParticleFile->part_data.force));
            }
      }

   if(myGUIState.t_vorticity) {
         if(myAttributeRefs.p_vorticity.isValid()) {
               attrVector3Handle.bind(myAttributeRefs.p_vorticity.getAttribute());
               attrVector3Handle.set(gdp->pointOffset(pt_num), UT_Vector3(myRFParticleFile->part_data.vorticity));
            }
      }

   if(myGUIState.t_normal) {
         if(myAttributeRefs.p_normal_N.isValid()) {
               attrVector3Handle.bind(myAttributeRefs.p_normal_N.getAttribute());
               attrVector3Handle.set(gdp->pointOffset(pt_num), UT_Vector3(myRFParticleFile->part_data.normal));
            }
      }

   if(myGUIState.t_texture_vector) {
         if(myAttributeRefs.p_texture_vector_uv.isValid()) {
               attrVector3Handle.bind(myAttributeRefs.p_texture_vector_uv.getAttribute());
               attrVector3Handle.set(gdp->pointOffset(pt_num), UT_Vector3(myRFParticleFile->part_data.texture_vector));
            }
      }

   if(myGUIState.t_age) {
         if(myAttributeRefs.p_age.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_age.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.age);
            }
      }

   if(myGUIState.t_isolation_time) {
         if(myAttributeRefs.p_isolation_time.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_isolation_time.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.isolation_time);
            }
      }

   if(myGUIState.t_viscosity) {
         if(myAttributeRefs.p_viscosity.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_viscosity.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.viscosity);
            }
      }

   if(myGUIState.t_density) {
         if(myAttributeRefs.p_density.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_density.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.density);
            }
      }

   if(myGUIState.t_pressure) {
         if(myAttributeRefs.p_pressure.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_pressure.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.pressure);
            }
      }

   if(myGUIState.t_mass) {
         if(myAttributeRefs.p_mass.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_mass.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.mass);
            }
      }

   if(myGUIState.t_temperature) {
         if(myAttributeRefs.p_temperature.isValid()) {
               attrFloatHandle.bind(myAttributeRefs.p_temperature.getAttribute());
               attrFloatHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.temperature);
            }
      }

   if(myGUIState.t_num_neighbors) {
         if(myAttributeRefs.p_num_neighbors.isValid()) {
               attrIntHandle.bind(myAttributeRefs.p_num_neighbors.getAttribute());
               attrIntHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.num_neighbors);
            }
      }

   if(myGUIState.t_info_bits) {
         if(myAttributeRefs.p_info_bits.isValid()) {
               attrIntHandle.bind(myAttributeRefs.p_info_bits.getAttribute());
               attrIntHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.info_bits);
            }
      }

   if(myGUIState.t_id) {
         if(myAttributeRefs.p_id.isValid()) {
               attrIntHandle.bind(myAttributeRefs.p_id.getAttribute());
               attrIntHandle.set(gdp->pointOffset(pt_num), myRFParticleFile->part_data.id);
            }
      }


   return 0;

}


/**********************************************************************************/
//  $Log: SOP_RF_Import_ReadRFParticleFile.C,v $
//  Revision 1.4  2012-08-31 03:00:51  mstory
//  Continued H12 mods.
//
//  Revision 1.3  2012-08-29 03:07:06  mstory
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
