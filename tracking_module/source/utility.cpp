/***************************************************************************************/
/*! Project component: utility
 *  \file   utility.cpp
 *  \brief  Purpose: This file contains the implementations for utility function
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

# include "utility.h"


/**** sorting related functions ****/
/*inline*/ void swap(float& x, float& y)
{ 
   float  t; 
   t = x; 
   x = y; 
   y = t; 
}
inline void order(float& x, float& y)
{
   if (x > y) swap(x, y);
}

bool find_pivot(float *left, float *right, 
                float *pivot_ptr)
{
   float   a, b, c, *p;
   a = *left;                         	// left value 
   b = *(left + (right - left) / 2);  	// middle value
   c = *right;                        	// right value 
   order(a, b); 
   order(a, c);
   order(b, c);   	// order these 3 values 
   if (a < b) {   	// pivot is higher of 2 values 
      *pivot_ptr = b;
      return true;
   }
   if (b < c) {
      *pivot_ptr = c;
      return true;
   }

   for (p = left + 1; p <= right; ++p)
      if (*p != *left) {
         *pivot_ptr = (*p < *left) ? *left : *p;
         return true;
      }
   return false;      	// all elements have same value 
}

float *partition(float *left, float *right, float pivot)
{
   while (left <= right) {
      while (*left < pivot)
       ++left;
      while (*right >= pivot)
       --right;
      if (left < right) {
       swap(*left, *right);
       ++left;
       --right;
      }
   }
   return left;
}

void quicksort(float *left, float *right)
{
   float   *p, pivot;
   if (find_pivot(left, right, &pivot)) {
      p = partition(left, right, pivot);
      quicksort(left, p - 1);
      quicksort(p, right);
   }
}


/***** row/col setting related functions ******/

void cvSetCols( const CvArr* src, CvArr* dst,
                int start_col, int end_col )
{
    int coi;
    CvMat *srcmat = (CvMat*)src, srcmatstub;
    CvMat *dstmat = (CvMat*)dst, dstmatstub;
    CvMat *refmat, refmathdr;
    int cols;

	CV_FUNCNAME( "cvSetCols" );
    __BEGIN__; 

    if( !CV_IS_MAT(dstmat) )
    {
        CV_CALL( dstmat = cvGetMat( dstmat, &dstmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(srcmat) )
    {
        CV_CALL( srcmat = cvGetMat( srcmat, &srcmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    cols = end_col - start_col;
    CV_ASSERT( srcmat->cols == cols || dstmat->cols == cols );
    if( srcmat->cols == cols )
    {
        refmat = cvGetCols( dstmat, &refmathdr, start_col, end_col );
        cvCopy( srcmat, refmat );
    }
    else
    {
        refmat = cvGetCols( srcmat, &refmathdr, start_col, end_col );
        cvCopy( refmat, dstmat );
    }
    __END__;
}

/*CV_INLINE void cvSetCol( const CvArr* src, CvArr* dst, int col )*/
CV_INLINE void cvSetCol( const CvArr* src, CvArr* dst, int col )
{
    cvSetCols( src, dst, col, col+1 );
}

void cvSetRows( const CvArr* src, CvArr* dst,int start_row, int end_row, int delta_row = 1 )
{
    int coi;
    CvMat *srcmat = (CvMat*)src, srcmatstub;
    CvMat *dstmat = (CvMat*)dst, dstmatstub;
    CvMat *refmat, refmathdr;
    int rows;

    CV_FUNCNAME( "cvSetRows" );
    __BEGIN__;

    if( !CV_IS_MAT(dstmat) )
    {
        CV_CALL( dstmat = cvGetMat( dstmat, &dstmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(srcmat) )
    {
        CV_CALL( srcmat = cvGetMat( srcmat, &srcmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    rows = cvFloor( ( end_row - start_row ) / delta_row );
    CV_ASSERT( srcmat->rows == rows || dstmat->rows == rows );
    if( srcmat->rows == rows )
    {
        refmat = cvGetRows( dstmat, &refmathdr, start_row, end_row, delta_row );
        cvCopy( srcmat, refmat );
    }
    else
    {
        refmat = cvGetRows( srcmat, &refmathdr, start_row, end_row, delta_row );
        cvCopy( refmat, dstmat );
    }
    __END__;
}

CV_INLINE void cvSetRow( const CvArr* src, CvArr* dst, int row )
{
    cvSetRows( src, dst, row, row+1, 1 );
}


