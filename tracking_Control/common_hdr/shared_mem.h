/***************************************************************************************/ 
/*! \b Project component: OBJECT_TRACKER
 *  \file   shared_mem.h
 *  \brief  Purpose:  Shared memory structure definition
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   11-26-2009
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
 *  - 01 December  2009 ST: Created
 *
 ***************************************************************************************
 ***************************************************************************************/
#ifndef HSHAREDMEMH

# define HSHAREDMEMH
#ifdef  __cplusplus
extern "C" {
#endif

# include "global_defines.h"  /* for MAX_MB_NUM definition */
# include "TRACKER_types.h"


typedef /*volatile*/ struct /*Exchange_Data_t*/
{
  char TrackedOutFileName[255]; /*!< Output tracked file name with full path (from TRACKER parameter file) */

  si32_t  pid_tracking; /*!< tracking process identifier */ 
  
  si32_t  frame_width; /*!< Frame dimension: horizontal width (encodeable) */
  si32_t  frame_height; /*!< Frame dimension: vertical height (encodeable)  */
  
  //ui32_t  RC_output_bitrate_value; /*!< Sequence target bit rate in bits/s(from TRACKER parameter file) */ 
  
  si32_t End_Of_Bitstream_Flag; /*!< 1=no more pictures to code 0=still coding (set frame by frame from incoming stream)*/

  si32_t Sequence_Framenum;  /*!< to write the right IDR_INTRA*/
  /*!< Linux: SHMEMMAX = 32Mbyte */
  /* To change this value, you have to change the SHMEMMAX kernel variable in theis way (as root): 
     `echo 67108864 > /proc/sys/kernel/shmmax`  for 64 MBytes (example) */
  ui8_t frame_y[(MAX_MB_NUM*256)]; /*!< Luma component */
  ui8_t frame_u[(MAX_MB_NUM*256)/4]; /*!< Chroma component: U (4:2:0) */
  ui8_t frame_v[(MAX_MB_NUM*256)/4]; /*!< Chroma component: V (4:2:0) */

  int bIsAVI;
  
  int tot_frame_num;

  unsigned char num_objects;
  unsigned char num_basis_images;
  int b_w;
  int b_h;
  unsigned int num_particles;

  
  unsigned char object_ID;

  unsigned char inst_num_basis_vectors;
  int object_present_flag;
  int object_init_flag;

  //Warping Parameters
# ifndef DIFF_WARP
  unsigned short x;
  unsigned short y;
  unsigned short width;
  unsigned short height;
  unsigned short angle;
# else
  short x;
  short y;
  short width;
  short height;
  short angle;
# endif

  unsigned short *x_last;
  unsigned short *y_last;
  unsigned short *width_last;
  unsigned short *height_last;
  unsigned short *angle_last;
    
  int tot_obj_num;

  int auto_init_basis_dim;
  unsigned int frame_num;

  int init_frame;

}Exchange_Data_t;

extern Exchange_Data_t *Exchange_Data_p;

#ifdef  __cplusplus
}
#endif

#endif /* HSHAREDMEMH */


/*END FILE*/

