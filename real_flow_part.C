/* ******************************************************************************
* Real Flow Particle Geometry File
*
* $RCSfile: real_flow_part.C,v $
*
* Description : Implementation for the RealFlow_Particle_File class
*
* $Revision: 1.4 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_part.C,v $
*
* $Author: mstory $
*
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

#ifndef __real_flow_part_C__
#define __real_flow_part_C__

namespace dca
{

   /* ******************************************************************************
   *  Function Name : RF_Particle_Exception()
   *
   *  Description : Exception class for the Real Flow Particle File Object
   *
   *
   *  Input Arguments : std::string msg
   *
   *  Return Value : None
   *
   ***************************************************************************** */
   RF_Particle_Exception::RF_Particle_Exception(std::string msg)
   {

//   std::cout << "RF_Particle_Exception: in constructor ... " << endl;

      e_msg = msg;
   };



   /* ******************************************************************************
   *  Function Name : RealFlow_Particle_File()
   *
   *  Description : Constructor for the Real Flow Particle File Object
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : None
   *
   ***************************************************************************** */
   RealFlow_Particle_File::RealFlow_Particle_File()
   {
      int i;

      part_header.verify_code = 0;
      for(i = 0; i < RF_PART_FLUID_NAME_SZ; i++)
         part_header.fluid_name[i] = 0;

      part_header.version = 0;
      part_header.scene_scale = 0;
      part_header.fluid_type = 0;
      part_header.elapsed_time = 0;
      part_header.frame_number = 0;
      part_header.fps = 0;
      part_header.num_particles = 0;
      part_header.radius = 0;

      for(i = 0; i < 3; i++) {
            part_header.pressure[i] = 0;
            part_header.speed[i] = 0;
            part_header.temperature[i] = 0;
            part_header.emitter_pos[i] = 0;
            part_header.emitter_rot[i] = 0;
            part_header.emitter_scale[i] = 0;
         }

      for(i = 0; i < 3; i++) {
            part_data.pos[i] = 0;
            part_data.vel[i] = 0;
            part_data.force[i] = 0;
            part_data.vorticity[i] = 0;
            part_data.normal[i] = 0;
            part_data.texture_vector[i] = 0;
         }

      part_data.num_neighbors = 0;
      part_data.info_bits = 0;
      part_data.age = 0;
      part_data.isolation_time = 0;
      part_data.viscosity = 0;
      part_data.density = 0;
      part_data.pressure = 0;
      part_data.mass = 0;
      part_data.temperature = 0;
      part_data.id = 0;

   }


   /* ******************************************************************************
   *  Function Name : ~RealFlow_Particle_File()
   *
   *  Description : Destructor for the Real Flow Particle File Object
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : None
   *
   ***************************************************************************** */
   RealFlow_Particle_File::~RealFlow_Particle_File()
   {
   }



   /* ******************************************************************************
   *  Function Name : open_part_file()
   *
   *  Description : Open a Real Flow Particle File
   *
   *
   *  Input Arguments : char *file_name, int mode
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::open_part_file(char * file_name, int mode)
   {

      if(mode == RF_FILE_READ) {

            try {
                  RFPartifstream.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
                  RFPartifstream.open((const char *) file_name, std::ios::in | std::ios::binary);
#ifdef DEBUG
                  std::cout << "RealFlow_Particle_File::open_part_file(): Opened Real Flow particle file for reading" << std::endl;
#endif
                  return 0;
               }
            catch(std::ios_base::failure & e) {
//         std::cout << "RealFlow_Particle_File::open_part_file(): EXCEPTION: " << e.what () << std::endl;
//         std::cout << "Can't open Real Flow particle file for reading" << std::endl;
                  RFPartifstream.clear();
                  return 1;
               }

         }

      else {

            try {
                  RFPartofstream.exceptions(std::ofstream::eofbit | std::ofstream::failbit | std::ofstream::badbit);
                  RFPartofstream.open((const char *) file_name, std::ios::out | std::ios::binary);
#ifdef DEBUG
                  std::cout << "RealFlow_Particle_File::open_part_file(): Opened Real Flow particle file for writing" << std::endl;
#endif
                  return 0;
               }
            catch(std::ios_base::failure & e) {
                  std::cerr << "RealFlow_Particle_File::open_part_file(): EXCEPTION: " << e.what() << std::endl;
                  RFPartofstream.clear();
                  return 1;
               }

         }

      return 0;
   }



   /* ******************************************************************************
   *  Function Name : read_part_file_header()
   *
   *  Description : Read the particle file header
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::read_part_file_header()
   {

#ifdef DEBUG
      std::cout << "RealFlow_Particle_File::read_part_file_header(): Reading Real Flow particle header record" << std::endl;
#endif

      try {
            // Read the version number
//    RFPartifstream.read ((char *) &part_header, sizeof (part_header));

            RFPartifstream.read((char *)&part_header.verify_code, sizeof(int));
            RFPartifstream.read((char *)&part_header.fluid_name, 250);
            RFPartifstream.read((char *)&part_header.version, sizeof(short int));
            RFPartifstream.read((char *)&part_header.scene_scale, sizeof(float));
            RFPartifstream.read((char *)&part_header.fluid_type, sizeof(int));
            RFPartifstream.read((char *)&part_header.elapsed_time, sizeof(float));
            RFPartifstream.read((char *)&part_header.frame_number, sizeof(int));
            RFPartifstream.read((char *)&part_header.fps, sizeof(int));
            RFPartifstream.read((char *)&part_header.num_particles, 4);
            RFPartifstream.read((char *)&part_header.radius, sizeof(float));


            for(int i = 0; i <= 2; i++) {
                  RFPartifstream.read((char *)&part_header.pressure[i], sizeof(float));

//       std::cout << "DEBUG part_header.pressure: = " << part_header.pressure[i] << std::endl;
               }


            for(int i = 0; i <= 2; i++)
               RFPartifstream.read((char *)&part_header.speed[i], sizeof(float));

            for(int i = 0; i <= 2; i++)
               RFPartifstream.read((char *)&part_header.temperature[i], sizeof(float));



            for(int i = 2; i >= 0; i--)
               RFPartifstream.read((char *) &part_header.emitter_pos[i], sizeof(float));
            for(int i = 2; i >= 0; i--)
               RFPartifstream.read((char *) &part_header.emitter_rot[i], sizeof(float));
            for(int i = 2; i >= 0; i--)
               RFPartifstream.read((char *) &part_header.emitter_scale[i], sizeof(float));

            // If this is not a Real Flow Particle file or not version 9 or greater, return   TODO: (throw exception)
            if(part_header.verify_code != 0x00FABADA || part_header.version < 9)
               return 1;

#ifdef DEBUG
            std::cout << "sizeof (part_header) = " << sizeof(part_header) << std::endl;
            std::cout << "part_header.verify_code = " << std::hex << part_header.verify_code << std::endl;
            std::cout << "part_header.fluid_name = " << std::dec << part_header.fluid_name << std::endl;
            std::cout << "part_header.version = " << part_header.version << std::endl;
            std::cout << "part_header.scene_scale = " << part_header.scene_scale << std::endl;
            std::cout << "part_header.fluid_type = " << part_header.fluid_type << std::endl;
            std::cout << "part_header.elapsed_time = " << part_header.elapsed_time << std::endl;
            std::cout << "part_header.frame_number = " << part_header.frame_number << std::endl;
            std::cout << "part_header.fps = " << part_header.fps << std::endl;
            std::cout << "part_header.num_particles = " << part_header.num_particles << std::endl;
            std::cout << "part_header.radius = " << part_header.radius << std::endl;
            std::cout << "part_header.pressure = " << part_header.pressure[0] << "\t"
                      << part_header.pressure[1] << "\t"  << part_header.pressure[2] << std::endl;
            std::cout << "part_header.speed = " << part_header.speed[0] << "\t"
                      << part_header.speed[1] << "\t" << part_header.speed[2] << std::endl;
            std::cout << "part_header.temperature = " << part_header.temperature[0] << "\t"
                      << part_header.temperature[1] << "\t" << part_header.temperature[2] << std::endl;
            std::cout << "part_header.emitter_pos = " << part_header.emitter_pos[0] << "\t"
                      << part_header.emitter_pos[1] << "\t" << part_header.emitter_pos[2] << std::endl;
            std::cout << "part_header.emitter_rot = " << part_header.emitter_rot[0] << "\t"
                      << part_header.emitter_rot[1] << "\t" << part_header.emitter_rot[2] << std::endl;
            std::cout << "part_header.emitter_scale = " << part_header.emitter_scale[0] << "\t"
                      << part_header.emitter_scale[1] << "\t" << part_header.emitter_scale[2] << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Particle_File::read_part_file_header(): EXCEPTION: " << e.what() << std::endl;
            RFPartifstream.clear();
            RFPartifstream.close();
            return 1;
         }


      return 0;
   }



   /* ******************************************************************************
   *  Function Name : read_part_data()
   *
   *  Description : Read the particle data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::read_part_data()
   {

#ifdef DEBUG
      std::cout << "RealFlow_Particle_File::read_part_data(): reading particle data record" << std::endl;
#endif

      try {

            RFPartifstream.read((char *) &part_data.pos[2], sizeof(float));
            RFPartifstream.read((char *) &part_data.pos[1], sizeof(float));
            RFPartifstream.read((char *) &part_data.pos[0], sizeof(float));

            RFPartifstream.read((char *) &part_data.vel[2], sizeof(float));
            RFPartifstream.read((char *) &part_data.vel[1], sizeof(float));
            RFPartifstream.read((char *) &part_data.vel[0], sizeof(float));

            RFPartifstream.read((char *) &part_data.force[2], sizeof(float));
            RFPartifstream.read((char *) &part_data.force[1], sizeof(float));
            RFPartifstream.read((char *) &part_data.force[0], sizeof(float));

            RFPartifstream.read((char *) &part_data.vorticity[2], sizeof(float));
            RFPartifstream.read((char *) &part_data.vorticity[1], sizeof(float));
            RFPartifstream.read((char *) &part_data.vorticity[0], sizeof(float));

            RFPartifstream.read((char *) &part_data.normal[2], sizeof(float));
            RFPartifstream.read((char *) &part_data.normal[1], sizeof(float));
            RFPartifstream.read((char *) &part_data.normal[0], sizeof(float));

            RFPartifstream.read((char *) &part_data.num_neighbors, sizeof(int));

            RFPartifstream.read((char *) &part_data.texture_vector[0], sizeof(float));
            RFPartifstream.read((char *) &part_data.texture_vector[1], sizeof(float));
            RFPartifstream.read((char *) &part_data.texture_vector[2], sizeof(float));

            RFPartifstream.read((char *) &part_data.info_bits, 2);
            RFPartifstream.read((char *) &part_data.age, sizeof(float));
            RFPartifstream.read((char *) &part_data.isolation_time, sizeof(float));
            RFPartifstream.read((char *) &part_data.viscosity, sizeof(float));
            RFPartifstream.read((char *) &part_data.density, sizeof(float));
            RFPartifstream.read((char *) &part_data.pressure, sizeof(float));
            RFPartifstream.read((char *) &part_data.mass, sizeof(float));
            RFPartifstream.read((char *) &part_data.temperature, sizeof(float));
            RFPartifstream.read((char *) &part_data.id, sizeof(int));
         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Particle_File::read_part_data(): EXCEPTION: " << e.what() << std::endl;
            RFPartifstream.clear();
            RFPartifstream.close();
            return 1;
         }

      return 0;
   }



   /* ******************************************************************************
   *  Function Name : write_part_file_header()
   *
   *  Description : Write the particle file header
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::write_part_file_header()
   {

#ifdef DEBUG
//std::cout << "RealFlow_Particle_File::write_part_file_header(): Writing Real Flow particle header record" << std::endl;
//std::cout << "part_header.verify_code = " << std::hex << part_header.verify_code << std::endl;
//std::cout << "part_header.fluid_name = " << std::dec << part_header.fluid_name << std::endl;
//std::cout << "part_header.version = " << part_header.version << std::endl;
//std::cout << "part_header.scene_scale = " << part_header.scene_scale << std::endl;
//std::cout << "part_header.fluid_type = " << part_header.fluid_type << std::endl;
//std::cout << "part_header.elapsed_time = " << part_header.elapsed_time << std::endl;
//std::cout << "part_header.frame_number = " << part_header.frame_number << std::endl;
//std::cout << "part_header.fps = " << part_header.fps << std::endl;
//std::cout << "part_header.num_particles = " << part_header.num_particles << std::endl;
//std::cout << "part_header.radius = " << part_header.radius << std::endl;
//std::cout << "part_header.pressure: max = " << part_header.pressure[0] << " min = " << part_header.pressure[1]
//               << " avg = " << part_header.pressure[2] << std::endl;
//std::cout << "part_header.speed: max = " << part_header.speed[0] << " min = " << part_header.speed[1]
//               << " avg = " << part_header.speed[2] << std::endl;
//std::cout << "part_header.temperature: max = " << part_header.temperature[0] << " min = " << part_header.temperature[1]
//               << " avg = " << part_header.temperature[2] << std::endl;
//std::cout << "part_header.emitter_pos = " << part_header.emitter_pos[0] << " " << part_header.emitter_pos[1]
//               << " " << part_header.emitter_pos[2] << std::endl;
//std::cout << "part_header.emitter_rot = " << part_header.emitter_rot[0] << " " << part_header.emitter_rot[1]
//               << " " << part_header.emitter_rot[2] << std::endl;
//std::cout << "part_header.emitter_scale = " << part_header.emitter_scale[0] << " " << part_header.emitter_scale[1]
//               << " " << part_header.emitter_scale[2] << std::endl;
#endif



      try {
            // Write the particle file header
            /*     RFPartofstream.write ((const char *) &part_header.verify_code, sizeof (part_header.verify_code));
                RFPartofstream.write ((const char *) &part_header.fluid_name, sizeof (part_header.fluid_name));
                RFPartofstream.write ((const char *) &part_header.version, sizeof (part_header.version));
                RFPartofstream.write ((const char *) &part_header.scene_scale, sizeof (part_header.scene_scale));
                RFPartofstream.write ((const char *) &part_header.fluid_type, sizeof (part_header.fluid_type));
                RFPartofstream.write ((const char *) &part_header.elapsed_time, sizeof (part_header.elapsed_time));
                RFPartofstream.write ((const char *) &part_header.frame_number, sizeof (part_header.frame_number));
                RFPartofstream.write ((const char *) &part_header.fps, sizeof (part_header.fps));
                RFPartofstream.write ((const char *) &part_header.num_particles, sizeof (part_header.num_particles));
                RFPartofstream.write ((const char *) &part_header.radius, sizeof (part_header.radius));
             */
            RFPartofstream.write((const char *) &part_header.verify_code, sizeof(int));
            RFPartofstream.write((const char *) &part_header.fluid_name, 250);
            RFPartofstream.write((const char *) &part_header.version, 2);
            RFPartofstream.write((const char *) &part_header.scene_scale, sizeof(float));
            RFPartofstream.write((const char *) &part_header.fluid_type, sizeof(int));
            RFPartofstream.write((const char *) &part_header.elapsed_time, sizeof(float));
            RFPartofstream.write((const char *) &part_header.frame_number, sizeof(int));
            RFPartofstream.write((const char *) &part_header.fps, sizeof(int));
            RFPartofstream.write((const char *) &part_header.num_particles, 4);
            RFPartofstream.write((const char *) &part_header.radius, sizeof(float));


            for(int i = 0; i <= 2; i++) {
                  RFPartofstream.write((const char *) &part_header.pressure[i], sizeof(float));

//       std::cout << "DEBUG part_header.pressure: = " << part_header.pressure[i] << std::endl;
               }



            for(int i = 0; i <= 2; i++)
               RFPartofstream.write((const char *) &part_header.speed[i], sizeof(float));
            for(int i = 0; i <= 2; i++)
               RFPartofstream.write((const char *) &part_header.temperature[i], sizeof(float));
            for(int i = 0; i <= 2; i++)
               RFPartofstream.write((const char *) &part_header.emitter_pos[i], sizeof(float));
            for(int i = 0; i <= 2; i++)
               RFPartofstream.write((const char *) &part_header.emitter_rot[i], sizeof(float));
            for(int i = 0; i <= 2; i++)
               RFPartofstream.write((const char *) &part_header.emitter_scale[i], sizeof(float));
         }

      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Particle_File::write_part_file_header(): EXCEPTION: " << e.what() << std::endl;
            RFPartofstream.clear();
            RFPartofstream.close();
            return 1;
         }

      return 0;
   }




   /* ******************************************************************************
   *  Function Name : write_part_data()
   *
   *  Description : Write the particle data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::write_part_data()
   {

#ifdef DEBUG
//        std::cout << "Writing particle data record" << std::endl;
#endif

      try {
            // Write the particle file header
//        std::cout << "Writing particle data record pos" << std::endl;
            RFPartofstream.write((const char *) &part_data.pos[2], sizeof(float));
            RFPartofstream.write((const char *) &part_data.pos[1], sizeof(float));
            RFPartofstream.write((const char *) &part_data.pos[0], sizeof(float));

//        std::cout << "Writing particle data record vel" << std::endl;

            RFPartofstream.write((const char *) &part_data.vel[2], sizeof(float));
            RFPartofstream.write((const char *) &part_data.vel[1], sizeof(float));
            RFPartofstream.write((const char *) &part_data.vel[0], sizeof(float));

//        std::cout << "Writing particle data record force" << std::endl;
            RFPartofstream.write((const char *) &part_data.force[2], sizeof(float));
            RFPartofstream.write((const char *) &part_data.force[1], sizeof(float));
            RFPartofstream.write((const char *) &part_data.force[0], sizeof(float));

//        std::cout << "Writing particle data record vorticity" << std::endl;
            RFPartofstream.write((const char *) &part_data.vorticity[2], sizeof(float));
            RFPartofstream.write((const char *) &part_data.vorticity[1], sizeof(float));
            RFPartofstream.write((const char *) &part_data.vorticity[0], sizeof(float));

//        std::cout << "Writing particle data record normal" << std::endl;
            RFPartofstream.write((const char *) &part_data.normal[2], sizeof(float));
            RFPartofstream.write((const char *) &part_data.normal[1], sizeof(float));
            RFPartofstream.write((const char *) &part_data.normal[0], sizeof(float));

//        std::cout << "Writing particle data record num_neighbors" << std::endl;
            RFPartofstream.write((const char *) &part_data.num_neighbors, sizeof(int));

//        std::cout << "Writing particle data record texture_vector" << std::endl;
            RFPartofstream.write((const char *) &part_data.texture_vector[0], sizeof(float));
            RFPartofstream.write((const char *) &part_data.texture_vector[1], sizeof(float));
            RFPartofstream.write((const char *) &part_data.texture_vector[2], sizeof(float));

//        std::cout << "Writing particle data record misc." << std::endl;
            RFPartofstream.write((const char *) &part_data.info_bits, 2);
            RFPartofstream.write((const char *) &part_data.age, sizeof(float));
            RFPartofstream.write((const char *) &part_data.isolation_time, sizeof(float));
            RFPartofstream.write((const char *) &part_data.viscosity, sizeof(float));
            RFPartofstream.write((const char *) &part_data.density, sizeof(float));
            RFPartofstream.write((const char *) &part_data.pressure, sizeof(float));
            RFPartofstream.write((const char *) &part_data.mass, sizeof(float));
            RFPartofstream.write((const char *) &part_data.temperature, sizeof(float));
            RFPartofstream.write((const char *) &part_data.id, sizeof(int));
         }

      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Particle_File::write_part_data(): EXCEPTION: " << e.what() << std::endl;
            RFPartofstream.clear();
            RFPartofstream.close();
            return 1;
         }

      return 0;
   }



   /* ******************************************************************************
   *  Function Name : read_additional_data()
   *
   *  Description : Read the [Additional Data] section ... not really used I guess.
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::read_additional_data()
   {

      try {
            int empty_int = 0;

            RFPartifstream.read((char *) &empty_int, sizeof(int));
            RFPartifstream.read((char *) &empty_int, sizeof(int));
//    RFPartifstream.read ((char *) &foo_char, sizeof (char) * 5);
         }

      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Particle_File::read_additional_data(): EXCEPTION: " << e.what() << std::endl;
            RFPartofstream.clear();
            RFPartofstream.close();
            return 1;
         }

      return 0;
   }




   /* ******************************************************************************
   *  Function Name : write_additional_data()
   *
   *  Description : Write the [Additional Data] section ... not really used I guess.
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::write_additional_data()
   {

      try {
            int empty_int = 0;

            RFPartofstream.write((const char *) &empty_int, sizeof(int));
            RFPartofstream.write((const char *) &empty_int, sizeof(int));
         }

      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Particle_File::write_additional_data(): EXCEPTION: " << e.what() << std::endl;
            RFPartofstream.clear();
            RFPartofstream.close();
            return 1;
         }

      return 0;
   }



   /* ******************************************************************************
   *  Function Name : close_part_file()
   *
   *  Description : Close the particle file
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int (function status)
   *
   ***************************************************************************** */
   int RealFlow_Particle_File::close_part_file(int mode)
   {

#ifdef DEBUG
      std::cout << "Closing Real Flow particle file" << std::endl;
#endif


      if(mode == RF_FILE_READ) {

            try {

                  RFPartifstream.close();
#ifdef DEBUG
                  std::cout << "RealFlow_Particle_File::close_part_file(): Closed Real Flow particle input stream" << std::endl;
#endif

               }
            catch(std::ios_base::failure & e) {
                  std::cerr << "RealFlow_Particle_File::close_part_file - EXCEPTION: " << e.what() << std::endl;
                  RFPartifstream.clear();
                  return 1;
               }

         }

      else {

            try {
                  RFPartofstream.close();

#ifdef DEBUG
                  std::cout << "RealFlow_Particle_File::close_part_file(): Closed Real Flow particle output stream" << std::endl;
#endif

               }
            catch(std::ios_base::failure & e) {
                  std::cerr << "RealFlow_Particle_File::close_part_file - EXCEPTION: " << e.what() << std::endl;
                  RFPartofstream.clear();
                  return 1;
               }

         }

      return 0;

   }


#endif

}


/**********************************************************************************/
//  $Log: real_flow_part.C,v $
//  Revision 1.4  2012-09-03 15:36:56  mstory
//  Fixed bug when importing mesh files & selecting texture attributes.
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
