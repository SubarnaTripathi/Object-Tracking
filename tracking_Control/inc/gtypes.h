/***************************************************************************************/ 
/*! \b Project component: 
 *  \file   gtypes.h
 *  \brief  Purpose:  Global variable types
 *
*  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009
 ***************************************************************************************
 *
 *  \author
 *    ST (2009)
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 02 December  2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
#ifndef HGTYPESH
#define HGTYPESH

/* {{{ Output writing */
#include <stdio.h>


#ifdef PRINT

/* {{{ PRINTOUT-PRINTERR */
#ifndef PRINTOUT
#define PRINTOUT 	stdout
#endif
#ifndef PRINTERR
#define PRINTERR 	stderr
#endif
/* }}} */

/* {{{ PRINTMES */
#define PRINTMES1(l,a) if((guc_verbose>=(l)) && !gb_quiet) \
			{fprintf(PRINTOUT,(a)); fflush(PRINTOUT);}
#define PRINTMES2(l,a,b) if((guc_verbose>=(l)) && !gb_quiet) \
			{fprintf(PRINTOUT,(a),(b)); fflush(PRINTOUT);}
#define PRINTMES3(l,a,b,c) if((guc_verbose>=(l)) && !gb_quiet) \
			{fprintf(PRINTOUT,(a),(b),(c)); fflush(PRINTOUT);}
#define PRINTMES4(l,a,b,c,d) if((guc_verbose>=(l)) && !gb_quiet) \
				{fprintf(PRINTOUT,(a),(b),(c),(d)); fflush(PRINTOUT);}
#define PRINTMES5(l,a,b,c,d,e) if((guc_verbose>=(l)) && !gb_quiet) \
				{fprintf(PRINTOUT,(a),(b),(c),(d),(e)); fflush(PRINTOUT);}
#define PRINTMES6(l,a,b,c,d,e,f) if((guc_verbose>=(l)) && !gb_quiet) \
				{fprintf(PRINTOUT,(a),(b),(c),(d),(e),(f)); fflush(PRINTOUT);}
/* }}} */
/* {{{ PRINTSTAT */				
#define PRINTSTAT1(l,a) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_statfile,(a)); fflush(gpF_statfile);}
#define PRINTSTAT2(l,a,b) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_statfile,(a),(b)); fflush(gpF_statfile);}
#define PRINTSTAT3(l,a,b,c) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_statfile,(a),(b),(c)); fflush(gpF_statfile);}
#define PRINTSTAT4(l,a,b,c,d) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_statfile,(a),(b),(c),(d)); fflush(gpF_statfile);}
#define PRINTSTAT5(l,a,b,c,d,e) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_statfile,(a),(b),(c),(d),(e)); fflush(gpF_statfile);}
#define PRINTSTAT6(l,a,b,c,d,e,f) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_statfile,(a),(b),(c),(d),(e),(f)); fflush(gpF_statfile);}
				
#define PRINTRCSTAT1(l,a) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_statfile,(a)); fflush(gpF_statfile);}
#define PRINTRCSTAT2(l,a,b) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_statfile,(a),(b)); fflush(gpF_statfile);}
#define PRINTRCSTAT3(l,a,b,c) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_statfile,(a),(b),(c)); fflush(gpF_statfile);}
#define PRINTRCSTAT4(l,a,b,c,d) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_statfile,(a),(b),(c),(d)); fflush(gpF_statfile);}
#define PRINTRCSTAT5(l,a,b,c,d,e) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_statfile,(a),(b),(c),(d),(e)); fflush(gpF_statfile);}
#define PRINTRCSTAT6(l,a,b,c,d,e,f) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_statfile,(a),(b),(c),(d),(e),(f)); fflush(gpF_statfile);}
/* }}} */
#if 0
/* {{{ PRINTMATLAB */				
#define PRINTMATLAB1(l,a) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_MatlabStatfile,(a)); fflush(gpF_MatlabStatfile);}
#define PRINTMATLAB2(l,a,b) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_MatlabStatfile,(a),(b)); fflush(gpF_MatlabStatfile);}
#define PRINTMATLAB3(l,a,b,c) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_MatlabStatfile,(a),(b),(c)); fflush(gpF_MatlabStatfile);}
#define PRINTMATLAB4(l,a,b,c,d) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_MatlabStatfile,(a),(b),(c),(d)); fflush(gpF_MatlabStatfile);}
#define PRINTMATLAB5(l,a,b,c,d,e) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_MatlabStatfile,(a),(b),(c),(d),(e)); fflush(gpF_MatlabStatfile);}
#define PRINTMATLAB6(l,a,b,c,d,e,f) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_MatlabStatfile,(a),(b),(c),(d),(e),(f)); fflush(gpF_MatlabStatfile);}
#else

#define PRINTMATLAB1(l,a) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_MatlabStatfile,(a)); fflush(gpF_MatlabStatfile);}          
#define PRINTMATLAB2(l,a,b) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_MatlabStatfile,(a),(b)); fflush(gpF_MatlabStatfile);}

#define PRINTMATLAB3(l,a,b,c) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
			{fprintf(gpF_MatlabStatfile,(a),(b),(c)); fflush(gpF_MatlabStatfile);}
    
#define PRINTMATLAB4(l,a,b,c,d) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_MatlabStatfile,(a),(b),(c),(d)); fflush(gpF_MatlabStatfile);}
#define PRINTMATLAB5(l,a,b,c,d,e) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \

#define PRINTMATLAB6(l,a,b,c,d,e,f) if((guc_verbose>=(l)) && (gc8_statname[0] != '-')) \
				{fprintf(gpF_MatlabStatfile,(a),(b),(c),(d),(e),(f)); fflush(gpF_MatlabStatfile);}
/* }}} */
#endif /* 0 */	
/* {{{ PRINTERROR */			
#define PRINTERROR1(l,a) if(guc_severity>=(l)) fprintf(PRINTERR,(a))
#define PRINTERROR2(l,a,b) if(guc_severity>=(l))  fprintf(PRINTERR,(a),(b))
#define PRINTERROR3(l,a,b,c) if(guc_severity>=(l)) fprintf(PRINTERR,(a),(b),(c))
#define PRINTERROR4(l,a,b,c,d) if(guc_severity>=(l)) \
				fprintf(PRINTERR,(a),(b),(c),(d))
#define PRINTERROR5(l,a,b,c,d,e) if(guc_severity>=(l)) \
				fprintf(PRINTERR,(a),(b),(c),(d),(e))
#define PRINTERROR6(l,a,b,c,d,e,f) if(guc_severity>=(l)) \
				fprintf(PRINTERR,(a),(b),(c),(d),(e),(f))
/* }}} */  
#else
/* {{{ PRINTMES - PRINTSTAT - PRINTMATLAB - PRINTERROR */ 
#define PRINTMES1(l,a)              
#define PRINTMES2(l,a,b)            
#define PRINTMES3(l,a,b,c)          
#define PRINTMES4(l,a,b,c,d)        
#define PRINTMES5(l,a,b,c,d,e)      
#define PRINTMES6(l,a,b,c,d,e,f)    
#define PRINTSTAT1(l,a)             
#define PRINTSTAT2(l,a,b)           
#define PRINTSTAT3(l,a,b,c)         
#define PRINTSTAT4(l,a,b,c,d)       
#define PRINTSTAT5(l,a,b,c,d,e)     
#define PRINTSTAT6(l,a,b,c,d,e,f)   
#define PRINTRCSTAT1(l,a)           
#define PRINTRCSTAT2(l,a,b)         
#define PRINTRCSTAT3(l,a,b,c)       
#define PRINTRCSTAT4(l,a,b,c,d)     
#define PRINTRCSTAT5(l,a,b,c,d,e)   
#define PRINTRCSTAT6(l,a,b,c,d,e,f) 
#define PRINTMATLAB1(l,a)           
#define PRINTMATLAB2(l,a,b)         
#define PRINTMATLAB3(l,a,b,c)       
#define PRINTMATLAB4(l,a,b,c,d)     
#define PRINTMATLAB5(l,a,b,c,d,e)   
#define PRINTMATLAB6(l,a,b,c,d,e,f) 
#define PRINTERROR1(l,a)            
#define PRINTERROR2(l,a,b)          
#define PRINTERROR3(l,a,b,c)        
#define PRINTERROR4(l,a,b,c,d)      
#define PRINTERROR5(l,a,b,c,d,e)    
#define PRINTERROR6(l,a,b,c,d,e,f) 
#define Stats(a,b)
/* }}} */

#endif /* PRINT */
/* }}} */


/* {{{ seq_informations_t = General sequence informations*/
struct seq_informations_t
{
  si32_t nframes;
  si32_t frame0;
  si32_t h;
  si32_t m;
  si32_t s;
  si32_t f;
  si32_t N;
  si32_t M;
  si32_t mpeg1;
  si32_t fieldpic;
  ui32_t ui32_horizontal_size;
  ui32_t ui32_vertical_size;
  si32_t frame_rate_code;
  double bit_rate;
  si32_t vbv_buffer_size;
  si32_t low_delay;
  si32_t chroma_format;
  si32_t dc_prec;
  si32_t topfirst;
  si32_t frame_pred_dct_tab[3];
  si32_t conceal_tab[3];
  si32_t qscale_tab[3];
  si32_t intravlc_tab[3];
  si32_t altscan_tab[3];
  si32_t repeatfirst;
  si32_t prog_frame;
  si32_t aspectratio;
  si32_t constrparms;
  si32_t profile;
  si32_t level;
  si32_t prog_seq;
  si32_t video_format;
  si32_t color_primaries;
  si32_t transfer_characteristics;
  si32_t matrix_coefficients;
  si32_t  display_horizontal_size;
  si32_t display_vertical_size;
};
/* }}} */

/* {{{ TRACKER Parameter File structure */
struct ParFile_t
{
  char tracking_exe_path[255];
  char tracking_exe_file[255];
  char in_filename[255];
  char tracked_out_filename[255];
  int  bIsAVI;
  int  frame_width;
  int  frame_height;
  int auto_tracker_init;
  int auto_init_basis_dim;
 
  
  int num_objects;
  int num_basis_images_minus1;
  int b_w;
  int b_h;
  int num_particles;
};
/* }}} */



#endif /* HGTYPESH */
/*END FILE*/
