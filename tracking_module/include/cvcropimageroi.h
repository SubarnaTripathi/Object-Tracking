/***************************************************************************************/ 
/*! \b Project component: tracking
 *  \file   cvcropimageroi.h
 *  \brief  Purpose:  taking out oriented rectangle from image
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
 * \b COPYRIGHT: (C) STMicrolectronics 2009 ALL RIGHTS RESEVED

 * This program contains proprietary information and it is non to be used,
 * copied, nor disclosed without written consent of STMicroelectronics.
 ***************************************************************************************/
#ifndef CV_CROPIMAGEROI_INCLUDED
#define CV_CROPIMAGEROI_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

# include "cvcreateaffine.h" 


#define _USE_MATH_DEFINES
#include <math.h>

# define M_PI 3.14159265358979323846 // math.h included??? _USE_MATH_DEFINES defined.. still M_PI not defined, why???


/**** function defintion *****/
void cvCropImageROI( const IplImage* img, IplImage* dst, 
                CvRect32f rect32f, CvPoint2D32f shear );

/**
 * Crop image with rotated and sheared rectangle
 */
/*void cvCropImageROI( const IplImage* img, IplImage* dst, 
                CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                CvPoint2D32f shear = cvPoint2D32f(0,0) )*/

void cvCropImageROI( const IplImage* img, IplImage* dst, 
                CvRect32f rect32f, CvPoint2D32f shear )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    float angle = rect32f.angle;

    if( (angle == 0) && (shear.x == 0) && (shear.y == 0) && 
        (rect.x >= 0) && (rect.y >= 0) && 
        ((rect.x + rect.width) <= img->width) && ((rect.y + rect.height) <= img->height )) // '=' added
    {
        CvMat subimg;

        cvGetSubRect( img, &subimg, rect );
        cvConvert( &subimg, dst );
    }
    else if( shear.x == 0 && shear.y == 0 )
    {
        int x, y, ch, xp, yp;
        double c = cos( -M_PI / 180 * angle );
        double s = sin( -M_PI / 180 * angle );
        cvZero( dst );

	    for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y++ )
            {
                xp = cvRound( c * x + -s * y ) + rect.x;
                yp = cvRound( s * x + c * y ) + rect.y;

                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
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
        cvCreateAffine( affine, rect32f, shear );
        cvmSet( xy, 2, 0, 1.0 );
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / rect32f.width );
            for( y = 0; y < rect.height; y++ )
            {
                cvmSet( xy, 1, 0, y / rect32f.height );
                cvMatMul( affine, xy, xyp );
                xp = cvRound( cvmGet( xyp, 0, 0 ) );
                yp = cvRound( cvmGet( xyp, 1, 0 ) );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                }
            }
        }
        cvReleaseMat( &affine );
        cvReleaseMat( &xy );
        cvReleaseMat( &xyp );
    }
}


#endif
