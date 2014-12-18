/***************************************************************************************/ 
/*! \b Project component: tracking
 *  \file   cvrect32f.h
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
 * \b COPYRIGHT: (C) STMicrolectronics 2009 ALL RIGHTS RESEVED

 * This program contains proprietary information and it is non to be used,
 * copied, nor disclosed without written consent of STMicroelectronics.
 ***************************************************************************************/
#ifndef CV_RECT32F_INCLUDED
#define CV_RECT32F_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"


/******************* Structure Definitions ***************************/
/**
 * Floating Rectangle Structure
 */
typedef struct CvRect32f {
    float x;      
    float y;      
    float width;  
    float height;
    float angle;  
} CvRect32f;


typedef struct CvBox32f { 
    float cx;     
    float cy;     
    float width;  
    float height; 
    float angle;  
} CvBox32f;
/****************************** prototypes ************************************/
CvBox32f cvBox32fFromRect32f( CvRect32f rect );

/***************************** Define *****************************************/

#define cvBox32fFromRect(rect) (cvBox32fFromRect32f(cvRect32fFromRect(rect)))
#define cvBox2DFromRect(rect) (cvBox2DFromBox32f(cvBox32fFromRect(rect)))
#define cvRectFromBox32f(box) (cvRectFromRect32f(cvRect32fFromBox32f(box)))
#define cvRectFromBox2D(box) (cvRectFromBox32f(cvBox32fFromBox2D(box)))
#define cvBox2DFromRect32f(rect) (cvBox2DFromBox32f(cvBox32fFromRect32f(rect)))
#define cvRect32fFromBox2D(box) (cvRect32fFromBox32f(cvBox32fFromBox2D(box)))

/******************* Function Implementations ***************************/

/*CV_INLINE CvRect32f cvRect32f( float x, float y, float width, float height, float angle = 0.0 )*/
CV_INLINE CvRect32f cvRect32f( float x, float y, float width, float height, float angle )
{
    CvRect32f rect = { x, y, width, height, angle };
    return rect;
}

/* CV_INLINE CvBox32f cvBox32f( float cx, float cy, float width, float height, float angle = 0.0 ) */
CV_INLINE CvBox32f cvBox32f( float cx, float cy, float width, float height, float angle )
{
    CvBox32f box = { cx, cy, width, height, angle };
    return box;
}

/*CV_INLINE CvRect32f cvRect32fFromRect( CvRect rect, float angle = 0.0 )*/
CV_INLINE CvRect32f cvRect32fFromRect( CvRect rect, float angle )
{
    return cvRect32f( (float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height, angle );
}

CV_INLINE /* CV_INLINE CvRect cvRectFromRect32f( CvRect32f rect )*/
CvRect cvRectFromRect32f( CvRect32f rect )
{
    return cvRect( cvRound( rect.x ), cvRound( rect.y ),
                   cvRound( rect.width ), cvRound( rect.height ) );
}


/*CV_INLINE CvBox32f cvBox32fFromBox2D( CvBox2D box )*/
CV_INLINE CvBox32f cvBox32fFromBox2D( CvBox2D box )
{
    return cvBox32f( box.center.x, box.center.y,
                     box.size.width, box.size.height,
                     box.angle );
}

/*CV_INLINE CvBox2D cvBox2DFromBox32f( CvBox32f box ) */
CV_INLINE CvBox2D cvBox2DFromBox32f( CvBox32f box )
{
    CvBox2D box2d;
    box2d.center.x = box.cx;
    box2d.center.y = box.cy;
    box2d.size.width = box.width;
    box2d.size.height = box.height;
    box2d.angle = box.angle;
    return box2d;
}


CvBox32f cvBox32fFromRect32f( CvRect32f rect )
{
    CvPoint2D32f c;
    // x + ( x + width - 1 ) / 2 = cx
    c.x = (float)(( 2 * rect.x + rect.width - 1 ) / 2.0);
    c.y = (float)(( 2 * rect.y + rect.height - 1 ) / 2.0);
    if( rect.angle != 0 )
    {
        CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( rect.x, rect.y ), rect.angle, 1.0, R );
        c = cvPoint2D32f (
            cvmGet( R, 0, 0 ) * c.x + cvmGet( R, 0, 1 ) * c.y + cvmGet( R, 0, 2 ),
            cvmGet( R, 1, 0 ) * c.x + cvmGet( R, 1, 1 ) * c.y + cvmGet( R, 1, 2 ) );
        cvReleaseMat( &R );
    }
    return cvBox32f( c.x, c.y, rect.width, rect.height, rect.angle );
}


CvRect32f cvRect32fFromBox32f( CvBox32f box )
{
    CvPoint2D32f l;
    // x + ( x + width - 1 ) / 2 = cx
    l.x = (float)(( 2 * box.cx + 1 - box.width ) / 2.0);
    l.y = (float)(( 2 * box.cy + 1 - box.height ) / 2.0);
    if( box.angle != 0.0 )
    {
        CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( box.cx, box.cy ), box.angle, 1.0, R );
        l = cvPoint2D32f (
            cvmGet( R, 0, 0 ) * l.x + cvmGet( R, 0, 1 ) * l.y + cvmGet( R, 0, 2 ),
            cvmGet( R, 1, 0 ) * l.x + cvmGet( R, 1, 1 ) * l.y + cvmGet( R, 1, 2 ) );
        cvReleaseMat( &R );
    }
    return cvRect32f( l.x, l.y, box.width, box.height, box.angle );
}

#endif
