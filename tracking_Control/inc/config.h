/***************************************************************************************/ 
/*! \b Project component: OBJECT_TRACKER
 *  \file   config.h
 *  \brief  Purpose:  Parameters configuration
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009
 ***************************************************************************************
 *
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 26 November  2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
#ifndef HCONFIGH
#define HCONFIGH


#ifndef HGLOBALH
# error "It's neccesary include the global.h file before config.h file"
#endif /* HGLOBALH */

/* {{{ input_output */
static one_config_param input_output[] =
{ 
  {"tracking_exe_path",      &ParFile.tracking_exe_path,			TYPE_STRING, 1, "# Tracker executable pathname"},
  {"tracking_exe_file",      &ParFile.tracking_exe_file,			TYPE_STRING, 1, "# Tracker executable"}, 
  {"in_filename_fullpath",  &ParFile.in_filename,					TYPE_STRING, 1, "# Input stream pathname "},
  {"tracked_out_filename_fullpath", &ParFile.tracked_out_filename,  TYPE_STRING, 1, "# Output tracked stream pathname"},
  {"bIsAVI",                &ParFile.bIsAVI,						TYPE_INT,	 1, "# AVI or YUV"},
  {"frame_width",           &ParFile.frame_width,					TYPE_INT,    1, "# Frame width needed for input YUV"},
  {"frame_height",          &ParFile.frame_height,					TYPE_INT,	 1, "# Frame height needed for input YUV"},
  {"auto_tracker_init",	    &ParFile.auto_tracker_init,				TYPE_INT,    1, "# Initialize tracker automatically (0: Disable, 1: Enable)"},
  {"auto_init_basis_dim",	&ParFile.auto_init_basis_dim,			TYPE_INT,    1, "# Initialie dimensions of basis image automatically (0: Disable, 1:Enable)"},
  {NULL}
};
/* }}} */

/* {{{ basis */
static one_config_param ROI[] =
{ 
  {"num_objects",    &ParFile.num_objects,	  TYPE_INT, 1, "# Total number of objects to be tracked in a frame"},
  {"num_basis_images_minus1",     &ParFile.num_basis_images_minus1,		  TYPE_INT, 1, "# Total number of basis images to be used by the appearence subspace"}, 
  {"b_w",                       &ParFile.b_w,                         TYPE_INT, 1, "# Width of basis dimension"},
  {"b_h",                       &ParFile.b_h,                         TYPE_INT, 1, "# Height of basis dimension"},
  {"particles",					&ParFile.num_particles,				  TYPE_INT, 1, "# Number of particles for CONDENSATION"},
  {NULL}
};
/* }}} */

/* {{{ matrix_configuration_section */
static name_pointer_config_section matrix_configuration_section[] =
{ 
  {input_output , "input/output"},
  {ROI , "ROIParam"},   
  {NULL}
};
/* }}} */

#endif /* HCONFIGH */
