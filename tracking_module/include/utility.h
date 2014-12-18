/***************************************************************************************/
/*! Project component: utility
 *  \file   utility.h
 *  \brief  Purpose: This file contains the prototypes for utility function
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

# ifndef UTILITY_FUNCTIONS
# define UTILITY_FUNCTIONS

#include <stdio.h>

#include "cv.h"

/**** prototyoes for sorting functions *****/
extern bool find_pivot(float *left, float *right, 
                float *pivot_ptr);
extern float* partition(float *left, float *right, float pivot);
extern void quicksort(float *left, float *right);


/*** prototypes for row/col setting ****/
extern void cvSetCols( const CvArr* src, CvArr* dst,int start_col, int end_col );
extern CV_INLINE void cvSetCol( const CvArr* src, CvArr* dst, int col );
extern void cvSetRows( const CvArr* src, CvArr* dst, int start_row, int end_row, int delta_row );
extern CV_INLINE void cvSetRow( const CvArr* src, CvArr* dst, int row );

# endif /*UTILITY_FUNCTIONS*/

