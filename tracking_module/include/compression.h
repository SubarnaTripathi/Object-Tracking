//# ifndef _COMPRESSION
//# define _COMPRESSION
/**************************** compression.cpp ******************************/
/**** all functions related to compression and reconstruction **************/
/***************************************************************************************/
/*! Project component: compression
 *  \file   compression.cpp
 *  \brief  Purpose: This file contains the functions prototypes for the compression of object
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009-11-12
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 12 November   2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
# include "cxtypes.h"
# include "defines.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "cv.h"


extern void ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1, unsigned char *src2, unsigned char *dst_ori,int width,int height) ;
extern void makeRGBfromfile(unsigned char *Dst, int width, int height, char *name, int fr_num ) ;
extern void init_dither_tab () ;

/**** function prototypes *****/
extern void InitLookupTable()  ;

extern int RGB2YUV420 (int x_dim, int y_dim,    
             unsigned char *bmp,    
             unsigned char *yuv, int flip) ;


// # endif _COMPRESS
