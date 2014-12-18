/***************************************************************************************/ 
/*! \b Project component: tracking
 *  \file   cvcreateaffine.h
 *  \brief  Purpose:  warping
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009
 ***************************************************************************************
 *
 *  \author
 *    Subarna Tripathi (2009)
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 30 November  2009 GF: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
#ifndef CV_CREATEAFFINE_INCLUDED
#define CV_CREATEAFFINE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <limits.h>

#define CV_AFFINE_SAME 0
#define CV_AFFINE_FULL 1

# include "cvrect32f.h"
# include "utility.h"

# define M_PI 3.14159265358979323846 // math.h included??? _USE_MATH_DEFINES defined.. still M_PI not defined, why???


/*************** prototypes ********************************/
CVAPI(void) cvInvAffine( const CvMat* affine, CvMat* invaffine );
void cvCreateAffine( CvMat* affine, CvRect32f rect, CvPoint2D32f shear);
IplImage* cvCreateAffineImage( const IplImage* src, const CvMat* affine,int flags, CvPoint* origin, CvScalar bgcolor );
void cvDrawRectangle( IplImage* img, CvRect32f rect32f, CvPoint2D32f shear, CvScalar color );

/***********************************************************/
/*Create an affine transform matrix
 */

/**
 * Create an inv affine transform matrix from an affine transform matrix
 *
 * @param affine    The 2 x 3 CV_32FC1|CV_64FC1 affine matrix
 * @param invaffine The 2 x 3 CV_32FC1|CV_64FC1 affine matrix to be created
 */
CVAPI(void) cvInvAffine( const CvMat* affine, CvMat* invaffine )
{
    CvMat* Affine;
    CvMat* InvAffine;

	 
    CV_FUNCNAME( "cvCreateAffine" );
    __BEGIN__;

    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );
    CV_ASSERT( invaffine->rows == 2 && invaffine->cols == 3 );
    CV_ASSERT( affine->type == invaffine->type );
    
    Affine = cvCreateMat( 3, 3, affine->type );
    InvAffine = cvCreateMat( 3, 3, affine->type );
    cvSetIdentity( Affine );
    cvSetRows( affine, Affine, 0, 2, 1 );
    cvInv( Affine, InvAffine );
    cvSetRows( InvAffine, invaffine, 0, 2, 1 );
    cvReleaseMat( &Affine );
    cvReleaseMat( &InvAffine );
    __END__;
}
/*
void cvCreateAffine( CvMat* affine, 
                    CvRect32f rect = cvRect32f(0,0,1,1,0), 
                   CvPoint2D32f shear = cvPoint2D32f(0,0) )
				   */
void cvCreateAffine( CvMat* affine, CvRect32f rect, CvPoint2D32f shear)
{
    double c, s;
    CvMat *R = NULL, *S = NULL, *A = NULL, hdr;
    cvmSet( affine, 0, 2, rect.x ); cvmSet( affine, 1, 2, rect.y );

    // R = [cos -sin; sin cos]
    R = cvCreateMat( 2, 2, affine->type );
    c = cos( -M_PI / 180 * rect.angle );
    s = sin( -M_PI / 180 * rect.angle );
    cvmSet( R, 0, 0, c ); cvmSet( R, 0, 1, -s );
    cvmSet( R, 1, 0, s ); cvmSet( R, 1, 1, c );
    S = cvCreateMat( 2, 2, affine->type );
    cvmSet( S, 0, 0, rect.width ); cvmSet( S, 0, 1, shear.x );
    cvmSet( S, 1, 0, shear.y );    cvmSet( S, 1, 1, rect.height );
    
    // A = R * S
    A = cvGetCols( affine, &hdr, 0, 2 );
    cvMatMul ( R, S, A );

    if ( NULL != R ) cvReleaseMat( &R );
    if ( NULL != S ) cvReleaseMat( &S );
}

/**
 * Affine transform of an image. 
 *
 * Memory is allocated. Do not forget cvReleaseImage( &ret );
 *
 * @param src       Image
 * @param affine    2 x 3 Affine transform matrix
 * @param flags     CV_AFFINE_SAME - Outside image coordinates are cut off. 
 *                  CV_AFFINE_FULL - Fully contain the original image pixel values
 * @param origin    The coordinate of origin is returned. The origin is the 
 *                  coordinate in original image respective to the transformed 
 *                  image origin. Useful when CV_AFFINE_FULL is used.
 * @param bgcolor   The color used when corresponding coordinate is not 
 *                  available in the original image. 
 * @return
 * @see cvWarpAffine
 *  this does not support CV_AFFINE_FULL, but supports
 *  everal interpolation methods and so on.
 */
/*
IplImage* 
cvCreateAffineImage( const IplImage* src, const CvMat* affine, 
                     int flags = CV_AFFINE_SAME, CvPoint* origin = NULL,
                     CvScalar bgcolor = CV_RGB(0,0,0) )*/
IplImage* 
cvCreateAffineImage( const IplImage* src, const CvMat* affine, 
                     int flags, CvPoint* origin,
                     CvScalar bgcolor )
{
    IplImage* dst = NULL;
    int minx = INT_MAX;
    int miny = INT_MAX;
    int maxx = INT_MIN;
    int maxy = INT_MIN;
    int i, x, y, xx, yy, xp, yp;
    int ch, width, height;
    CvPoint pt[4];
    CvMat* invaffine = NULL;

    CV_FUNCNAME( "cvAffineImage" );
    __BEGIN__;

    CV_ASSERT( src->depth == IPL_DEPTH_8U );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    // cvBoxPoints supports only rotation (no shear deform)
    // original 4 corner
    pt[0].x = 0;              pt[0].y = 0;
    pt[1].x = src->width - 1; pt[1].y = 0;
    pt[2].x = 0;              pt[2].y = src->height - 1;
    pt[3].x = src->width - 1; pt[3].y = src->height - 1;
    // 4 corner after transformed
    for( i = 0; i < 4; i++ )
    {
        x = cvRound( pt[i].x * cvmGet( affine, 0, 0 ) + 
                     pt[i].y * cvmGet( affine, 0, 1 ) + 
                     cvmGet( affine, 0, 2 ) );
        y = cvRound( pt[i].x * cvmGet( affine, 1, 0 ) + 
                     pt[i].y * cvmGet( affine, 1, 1 ) + 
                     cvmGet( affine, 1, 2 ) );
        pt[i].x = x; pt[i].y = y;
    }
    // min, max
    for( i = 0; i < 4; i++ )
    {
        minx = MIN( pt[i].x, minx );
        miny = MIN( pt[i].y, miny );
        maxx = MAX( pt[i].x, maxx );
        maxy = MAX( pt[i].y, maxy );
    }
    // target image width and height
    if( flags == CV_AFFINE_SAME )
    {
        width = src->width;
        height = src->height;
        minx = miny = 0;
        maxx = src->width - 1;
        maxy = src->height - 1;
    }
	else /*if( flags == CV_AFFINE_FULL ) */
	{
		width = maxx - minx + 1;
        height = maxy - miny + 1;
	}
    //cvPrintMat( affine );
    //printf( "%d %d %d %d\n", minx, miny, maxx, maxy );
    if( origin != NULL )
    {
        origin->x = minx;
        origin->y = miny;
    }
    dst = cvCreateImage( cvSize(width, height), src->depth, src->nChannels );
    cvSet( dst, bgcolor );

    // inverse affine
    invaffine = cvCreateMat( 2, 3, affine->type );
    cvInvAffine( affine, invaffine );
    
    // loop based on image coordinates of transformed image
    for( x = 0; x < width; x++ )
    {
        xx = x + minx;
        for( y = 0; y < height; y++ )
        {
            yy = y + miny;
            xp = cvRound( xx * cvmGet( invaffine, 0, 0 ) + 
                              yy * cvmGet( invaffine, 0, 1 ) + 
                              cvmGet( invaffine, 0, 2 ) );
            yp = cvRound( xx * cvmGet( invaffine, 1, 0 ) + 
                              yy * cvmGet( invaffine, 1, 1 ) + 
                              cvmGet( invaffine, 1, 2 ) );
            if( xp < 0 || xp >= src->width || yp < 0 || yp >= src->height ) continue;
            for( ch = 0; ch < src->nChannels; ch++ )
            {
                dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                    = src->imageData[src->widthStep * yp + xp * src->nChannels + ch];
            }
        }
    }
    if ( NULL != invaffine ) cvReleaseMat( &invaffine );
    __END__;
    return dst;
}

/**
 * Create a mask image for cvCreateAffineImage
 *
 * @param src       Image. Used to get image size.
 * @param affine    2 x 3 Affine transform matrix
 * @param flags     CV_AFFINE_SAME - Outside image coordinates are cut off
 *                  CV_AFFINE_FULL - Fully contain the original image pixel values
 * @param origin    The coordinate of origin (the coordinate in original image respective to 
 *                  the transformed image origin). 
 *                  Useful when CV_AFFINE_FULL is used.
 */
/*CV_INLINE IplImage* 
cvCreateAffineMask( const IplImage* src, const CvMat* affine, 
                    int flags = CV_AFFINE_SAME, CvPoint* origin = NULL )*/
CV_INLINE IplImage* 
cvCreateAffineMask( const IplImage* src, const CvMat* affine, 
                    int flags, CvPoint* origin )
{
    IplImage* orig = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
    cvSet( orig, cvScalar(1) );
    IplImage* mask = cvCreateAffineImage( orig, affine, flags, origin, cvScalar(0) );
    cvReleaseImage( &orig );
    return mask;
}
/**
 * Draw an rotated and sheared rectangle
 */

/*void cvDrawRectangle( IplImage* img, 
                 CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                 CvPoint2D32f shear = cvPoint2D32f(0,0), 
                 CvScalar color = CV_RGB(255, 255, 255), 
                 int thickness = 1, 
                 int line_type = 8,
                 int shift = 0 )*/

void cvDrawRectangle( IplImage* img, 
                 CvRect32f rect32f,
                 CvPoint2D32f shear, 
                 CvScalar color )
{
	int thickness = 1; 
    int line_type = 8;
    int shift = 0;
    CvRect rect = cvRectFromRect32f( rect32f );
    float angle = rect32f.angle;
    if( angle == 0 && shear.x == 0 && shear.y == 0 )
    {
        CvPoint pt1 = cvPoint( rect.x, rect.y );
        CvPoint pt2 = cvPoint( rect.x + rect.width - 1, rect.y + rect.height - 1 );

        cvRectangle( img, pt1, pt2, color, thickness, line_type, shift );
    }
    else if( shear.x == 0 && shear.y == 0 )
    {
        int x, y, ch, xp, yp;
        double c = cos( -M_PI / 180 * angle );
        double s = sin( -M_PI / 180 * angle );
        for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y += MAX(1, rect.height - 1) )
            {
                xp = cvRound( c * x + -s * y ) + rect.x;
                yp = cvRound( s * x + c * y ) + rect.y;

                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }

        for( y = 0; y < rect.height; y++ )
        {
            for( x = 0; x < rect.width; x += MAX( 1, rect.width - 1) )
            {
                xp = cvRound( c * x + -s * y ) + rect.x;
                yp = cvRound( s * x + c * y ) + rect.y;

                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
    }
    else
    {
        int x, y, ch, xp, yp;
        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        CvMat* xy     = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat* xyp    = cvCreateMat( 2, 1, CV_32FC1 );
        cvmSet( xy, 2, 0, 1.0 );
        cvCreateAffine( affine, rect32f, shear );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / rect32f.width );
            for( y = 0; y < rect.height; y += MAX(1, rect.height - 1) )
            {
                cvmSet( xy, 1, 0, y / rect32f.height );
                cvMatMul( affine, xy, xyp );
                xp = cvRound( cvmGet( xyp, 0, 0 ) );
                yp = cvRound( cvmGet( xyp, 1, 0 ) );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
        for( y = 0; y < rect.height; y++ )
        {
            cvmSet( xy, 1, 0, y / rect32f.height );
            for( x = 0; x < rect.width; x += MAX( 1, rect.width - 1) )
            {
                cvmSet( xy, 0, 0, x / rect32f.width );
                cvMatMul( affine, xy, xyp );
                xp = cvRound( cvmGet( xyp, 0, 0 ) );
                yp = cvRound( cvmGet( xyp, 1, 0 ) );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
        cvReleaseMat( &affine );
        cvReleaseMat( &xy );
        cvReleaseMat( &xyp );
    }
}


#endif
