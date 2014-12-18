/***************************************************************************************/
/*! Project component: particle filter
 *  \file   cvparticle.h
 *  \brief  Purpose: This file contains the prototypes for functions related to particle filter or CONDENSATION
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009-11-12
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 12 November   2009 ST: Creation
 * - 25 October 2010 ST: two state dynamic model available (1st order and 2nd order AR)
 *
 ***************************************************************************************
 * \b COPYRIGHT: (C) STMicrolectronics 2009 ALL RIGHTS RESEVED\n
 * This program contains proprietary information and it is non to be used,
 * copied, nor disclosed without written consent of STMicroelectronics.
 ***************************************************************************************/

#ifndef CV_PARTICLE_INCLUDED
#define CV_PARTICLE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include "cvcropimageroi.h"
# include "cvanglemean.h"

#include <float.h>

#include <time.h>
#include "utility.h"
#include "subspace.h"

# define CONS_X_DIFF	3	/* in pixels */
# define CONS_Y_DIFF	3	/* in pixels */
# define CONS_W_DIFF	4	/* in pixels */
# define CONS_H_DIFF	4	/* in pixels */
# define CONS_A_DIFF	10   /* in degrees */


/******************************* Structures **********************************/
typedef struct CvParticleState {
    double x;        // center coord of a rectangle
    double y;        // center coord of a rectangle
    double width;    // width of a rectangle
    double height;   // height of a rectangle
    double angle;    // rotation around center. degree

	# ifdef PF_2nd_ORDER_AR
	double xp;       // previous center coord of a rectangle
    double yp;       // previous center coord of a rectangle
    double widthp;   // previous width of a rectangle
    double heightp;  // previous height of a rectangle
    double anglep;   // previous rotation around center. degree
	# endif
} CvParticleState;

/**
 * Particle Filter structure
 */
typedef struct CvParticle {

    int num_states;    // Number of tracking states, e.g., 4 if x, y, width, height
    int num_observes;  // Number of observation models, e.g., 2 if color model and shape model
    int num_particles; 
    CvMat* dynamics;   
    CvRNG  rng;        /**< Random seed */ 
    CvMat* std;        /**< num_states x 1. Standard deviation for gaussian noise
                           Set standard deviation == 0 for no noise */
    CvMat* stds;       /**< num_states x num_particles. Std for each state and
                          each particle. "std" is used if "stds" is not set. */
    CvMat* bound;      /**< num_states x 3 (lowerbound, upperbound, 
                          wrap_around (like angle) flag 0 or 1)
                          Set lowerbound == upperbound to express no bound */
    // particle states
	CvMat* particles;  /**< num_states x num_particles. The particles. 
                              The transition states values of all particles. */
    CvMat* probs;      /**< num_observes x num_particles. The likelihood of 
                              each particle respect to the particle id in "particles" */

	CvMat* weights;    /**< 1 x num_particles. The weights of  
                           each particle respect to the particle id in "particles".  
                           "weights" are used to approximated the posterior pdf. */  
	bool logprob;	// probs are log probabilities

	//CvMat* particle_probs; // 1 x num_particles. marginalization respect to observation models      
	//CvMat* observe_probs;  // num_observes x 1.  marginalization respect to tracking states  

} CvParticle;

# ifdef PF_2nd_ORDER_AR
int num_states = 10;
# else
int num_states = 5;
# endif
// Definition of dynamics model
// new_particle = cvMatMul( dynamics, particle ) + noise
// curr_x =: curr_x + dx + noise = curr_x + (curr_x - prev_x) + noise
// prev_x =: curr_x

# ifndef PF_2nd_ORDER_AR
double dynamics[] = {
    1, 0, 0, 0, 0, 
    0, 1, 0, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 0, 1, 
};
# else
double dynamics[] = {
    2, 0, 0, 0, 0, -1, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 2, 0, 0, 0, 0, -1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
};
# endif


/**************************** Function Prototypes ****************************/

CvParticle* cvCreateParticle( int num_states, int num_observes, int num_particles);
void cvReleaseParticle( CvParticle** p );
void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics );
void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std );
void cvParticleSetBound( CvParticle* p, const CvMat* bound );

# if 0
void cvParticleInit( CvParticle* p, const CvParticle* init = NULL );
# else
void cvParticleInit( CvParticle* p, const CvParticle* init );
# endif

void cvParticleTransition( CvParticle* p );
void icvParticleBound( CvParticle* p );
void cvParticleResample( CvParticle* p, bool normalize ); 

void cvParticleMarginalize( CvParticle* p ); 


double cvRandGauss( CvRNG* rng, double sigma );

// Functions for CvParticleState structure ( constructor, getter, setter )
CvParticleState cvParticleStateFromMat( const CvMat* state );
void cvParticleStateToMat( const CvParticleState &state, CvMat* state_mat );
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id );
void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState &state );

// Particle Filter configuration
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std );

# ifdef ADDITIONAL_BOUND
void cvParticleStateAdditionalBound( CvParticle* p, CvBox32f box);
# endif

// Utility Functions
void cvParticleStateDraw( const CvParticleState& state, IplImage* frame, CvScalar color );
int  cvParticleGetMax( const CvParticle* p ); 
void cvParticleGetMean( const CvParticle* p, CvMat* meanp );  
void cvParticleStatePrint( const CvParticleState& state );
void cvParticleSetWeight( CvParticle* p, float *weights, double max_dist );

/**
 * This function returns a Gaussian random variate, with mean zero and standard deviation sigma.
 */
/*************************** Constructor / Destructor *************************/

CvParticle* cvCreateParticle( int num_states, int num_observes, 
                              int num_particles )
{
    CvParticle *p = NULL;
    p = (CvParticle *) cvAlloc( sizeof( CvParticle ) );
    p->num_particles = num_particles;
    p->num_states    = num_states;
    p->num_observes  = num_observes;
    p->dynamics      = cvCreateMat( num_states, num_states, CV_32FC1 );
    p->rng           = 1;
    p->std           = cvCreateMat( num_states, 1, CV_32FC1 );
    p->bound         = cvCreateMat( num_states, 3, CV_32FC1 );
    p->particles     = cvCreateMat( num_states, num_particles, CV_32FC1 );
    p->probs         = cvCreateMat( num_observes, num_particles, CV_64FC1 );
	p->weights       = cvCreateMat( 1, num_particles, CV_64FC1 );
    p->stds          = NULL;

    // Default dynamics: next state = curr state + noise

	//Rotated rectangle state particle filter +
    //2nd order AR dynamics model ( in fact, next = current + speed + noise )

    cvSetIdentity( p->dynamics, cvScalar(1.0) );
    cvSet( p->std, cvScalar(1.0) );

    cvZero( p->bound );

	p->logprob = 0;
    return p;
}

/**
 * Release Particle filter structure
 */
void cvReleaseParticle( CvParticle** particle )
{
    CvParticle *p = NULL;
    p = *particle;
    if( !p ) exit(1);
    
    cvReleaseMat( &p->dynamics  );
    cvReleaseMat( &p->std ) ;
    cvReleaseMat( &p->bound ) ;
    cvReleaseMat( &p->particles  );
	cvReleaseMat( &p->weights );
    cvReleaseMat( &p->probs ) ;
    if( p->stds != NULL )
		cvReleaseMat( &p->stds );

    cvFree( &p ) ;
    
}

/***************************** Setter ***************************************/

void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics )
{
	if( (p->num_states != dynamics->rows ) || (p->num_states != dynamics->cols) )
	{
		printf("dimension doesn't match");
		exit(1);
	}
    
    //cvCopy( dynamics, p->dynamics );
    cvConvert( dynamics, p->dynamics );
}

void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std )
{
	if ( p->num_states != std->rows )
	{
		printf("dimension doesn't match");
		exit(1);
	}

    p->rng = rng;
    //cvCopy( std, p->std );
    cvConvert( std, p->std );
}
void cvParticleSetWeight( CvParticle* p, float *weights, double max_dist )
{
	int i;
	double weight;
	for( i = 0; i < p->num_particles; i++ ) 
	{
		weight = max_dist/(weights[i]);
		cvmSet( p->weights, 0, i, weight ); 
	}
}

/**
 * Set lowerbound and upperbound used for bounding tracking state transition
 *
 */
void cvParticleSetBound( CvParticle* p, const CvMat* bound )
{
    cvConvert( bound, p->bound );
}


/******************* Main (Related to Algorithm) *****************************/

/**
 * Initialize states
 */
void cvParticleInit( CvParticle* p, const CvParticle* init )
{
    int i, j, k;
    if( init )
    {   
        int *num_copy = NULL;
        CvMat init_particle;

        int divide = p->num_particles / init->num_particles;
        int remain = p->num_particles - divide * init->num_particles;
        num_copy = (int*) calloc( init->num_particles, sizeof(int) );
		if ( num_copy == NULL )
		{
			printf(" num_copy memory allocation error ");
		}
        for( i = 0; i < init->num_particles; i++ )
        {
            num_copy[i] = divide + ( i < remain ? 1 : 0 );
        }
        
        k = 0;
        for( i = 0; i < init->num_particles; i++ )
        {
            cvGetCol( init->particles, &init_particle, i );
            for( j = 0; j < num_copy[i]; j++ )
            {
                cvSetCol( &init_particle, p->particles, k++ );
            }
        }
        
        free( num_copy );
    } 
}

/**
 * Samples new particles from given particles
 */
void cvParticleTransition( CvParticle* p )
{
    int i, j;
    CvMat* transits = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noises   = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noise, noisehdr;
    double std;
    
    // dynamics
    cvMatMul( p->dynamics, p->particles, transits );
    
    // noise generation
    if( p->stds == NULL )
    {
        for( i = 0; i < p->num_states; i++ )
        {
            std = cvmGet( p->std, i, 0 );
            noise = cvGetRow( noises, &noisehdr, i );
            if( std == 0.0 )
                cvZero( noise );
            else
                cvRandArr( &p->rng, noise, CV_RAND_NORMAL, cvScalar(0), cvScalar( std ) );
        }
    }
    else
    {
        for( i = 0; i < p->num_states; i++ )
        {
            for( j = 0; j < p->num_particles; j++ )
            {
                std = cvmGet( p->stds, i, j );
                if( std == 0.0 )
                    cvmSet( noises, i, j, 0.0 );
                else
                    cvmSet( noises, i, j, cvRandGauss( &p->rng, std ) );
            }
        }
    }

    // dynamics + noise
    cvAdd( transits, noises, p->particles );

    cvReleaseMat( &transits );
    cvReleaseMat( &noises );

    icvParticleBound( p );
}
/**
 * Apply lower bound and upper bound for particle states.
 *
 */
void icvParticleBound( CvParticle* p )
{
    int row, col;
    double lower, upper;
    int circular;
    CvMat* stateparticles, hdr;
    double state;  
    // @todo:     np.width   = (double)MAX( 2.0, MIN( maxX - 1 - x, width ) );
    for( row = 0; row < p->num_states; row++ )
    {
        lower = cvmGet( p->bound, row, 0 );
        upper = cvmGet( p->bound, row, 1 );
        circular = (int) cvmGet( p->bound, row, 2 );
        if( lower == upper ) continue; // no bound flag
        if( circular ) {
            for( col = 0; col < p->num_particles; col++ ) {
                state = cvmGet( p->particles, row, col );
                state = state < lower ? state + upper : ( state >= upper ? state - upper : state );
                cvmSet( p->particles, row, col, state );
            }
        } else {
            stateparticles = cvGetRow( p->particles, &hdr, row );
            cvMinS( stateparticles, upper, stateparticles );
            cvMaxS( stateparticles, lower, stateparticles );
        }
    }
}
double cvRandGauss( CvRNG* rng, double sigma )
{
    CvMat* mat = cvCreateMat( 1, 1, CV_64FC1 );
    double var = 0;
    cvRandArr( rng, mat, CV_RAND_NORMAL, cvRealScalar(0), cvRealScalar(sigma) );
    var = cvmGet( mat, 0, 0 );
    cvReleaseMat( &mat );
    return var;
}



/****************** Functions for CvParticleState structure ******************/

// This kinds of state definitions are not necessary, 
// but helps readability of codes for sure.

/**
 * Constructor
 */
inline CvParticleState cvParticleState( double x, 
                                        double y, 
                                        double width, 
                                        double height, 
                                        double angle 
										# ifdef PF_2nd_ORDER_AR
										,double xp, 
                                        double yp, 
                                        double widthp, 
                                        double heightp,
                                        double anglep 
										# endif
										)
{
    CvParticleState state = { x, y, width, height, angle
		# ifdef PF_2nd_ORDER_AR
		, xp, yp, widthp, heightp, anglep
		# endif
	}; 
    return state;
}

/**
 * Convert a matrix state representation to a state structure
 */
CvParticleState cvParticleStateFromMat( const CvMat* state )
{
    CvParticleState s;
    s.x       = cvmGet( state, 0, 0 );
    s.y       = cvmGet( state, 1, 0 );
    s.width   = cvmGet( state, 2, 0 );
    s.height  = cvmGet( state, 3, 0 );
    s.angle   = cvmGet( state, 4, 0 );
# ifdef PF_2nd_ORDER_AR
	s.xp      = cvmGet( state, 5, 0 );
    s.yp      = cvmGet( state, 6, 0 );
    s.widthp  = cvmGet( state, 7, 0 );
    s.heightp = cvmGet( state, 8, 0 );
    s.anglep  = cvmGet( state, 9, 0 );
# endif
    return s;
}

/**
 * Convert a state structure to CvMat
 *
 */
void cvParticleStateToMat( const CvParticleState& state, CvMat* state_mat )
{
    cvmSet( state_mat, 0, 0, state.x );
    cvmSet( state_mat, 1, 0, state.y );
    cvmSet( state_mat, 2, 0, state.width );
    cvmSet( state_mat, 3, 0, state.height );
    cvmSet( state_mat, 4, 0, state.angle );
# ifdef PF_2nd_ORDER_AR
	cvmSet( state_mat, 5, 0, state.xp );
    cvmSet( state_mat, 6, 0, state.yp );
    cvmSet( state_mat, 7, 0, state.widthp );
    cvmSet( state_mat, 8, 0, state.heightp );
    cvmSet( state_mat, 9, 0, state.anglep );
# endif

}

/**
 * Get a state from a particle filter structure
 *
 */
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id )
{
    CvMat* state, hdr;
    state = cvGetCol( p->particles, &hdr, p_id );
	return cvParticleStateFromMat( state );  
}

/**
 * Set a state to a particle filter structure
 *
 */
void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState& state )
{
    CvMat* state_mat, hdr;
    state_mat = cvGetCol( p->particles, &hdr, p_id );
    cvParticleStateToMat( state, state_mat );
}

/*************************** Particle Filter Configuration *********************************/

/**
 * Configuration of Particle filter
 */
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std )
{
    // config dynamics model
    CvMat dynamicsmat = cvMat( p->num_states, p->num_states, CV_64FC1, dynamics );

    // config random noise standard deviation
    CvRNG rng = cvRNG( time( NULL ) );
    double stdarr[] = {
        std.x,
        std.y,
        std.width,
        std.height,
        std.angle,
		0,
		0,
		0,
		0,
		0
    };
    CvMat stdmat = cvMat( p->num_states, 1, CV_64FC1, stdarr );

    // config minimum and maximum values of states
    // lowerbound, upperbound, circular flag (useful for degree)
    // lowerbound == upperbound to express no bounding
    double boundarr[] = {
        0, imsize.width - 1, false,
        0, imsize.height - 1, false,
        1, imsize.width, false,
        1, imsize.height, false,
        0, 360, true,
		0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    CvMat boundmat = cvMat( p->num_states, 3, CV_64FC1, boundarr );
    cvParticleSetDynamics( p, &dynamicsmat );
    cvParticleSetNoise( p, rng, &stdmat );
    cvParticleSetBound( p, &boundmat );
}

# ifdef ADDITIONAL_BOUND
void cvParticleStateAdditionalBound( CvParticle* p, CvBox32f last_tracked_state )
{
	double rl, ll;
	last_tracked_state = last_tracked_state;
    for( int np = 0; np < p->num_particles; np++ ) 
    {
        double x      = cvmGet( p->particles, 0, np );
        double y      = cvmGet( p->particles, 1, np );
        double width  = cvmGet( p->particles, 2, np );
        double height = cvmGet( p->particles, 3, np );
		double angle  = cvmGet( p->particles, 4, np );

# if 0
		/***** x ****/
		if ( x < (last_tracked_state.cx - CONS_X_DIFF ) )
			x = last_tracked_state.cx - CONS_X_DIFF; // another state x is used
		if ( x > (last_tracked_state.cx + CONS_X_DIFF ) )
			x = last_tracked_state.cx + CONS_X_DIFF; // another state x is used   
		

		/***** y ****/
		if ( y < (last_tracked_state.cy - CONS_Y_DIFF ) )
			y = last_tracked_state.cy - CONS_Y_DIFF; // another state y is used
		if ( y > (last_tracked_state.cy + CONS_Y_DIFF ) )
			y = last_tracked_state.cy + CONS_Y_DIFF; // another state y is used      
		
		/***** width ****/
		if ( width < (last_tracked_state.width - CONS_W_DIFF ) )
			width = last_tracked_state.width - CONS_W_DIFF; // another state w is used
		if ( width > (last_tracked_state.width + CONS_W_DIFF ) )
			width = last_tracked_state.width + CONS_W_DIFF; // another state w is used      
				

		/**** height ****/
		if ( height < (last_tracked_state.height - CONS_H_DIFF ) )
			height = last_tracked_state.height - CONS_H_DIFF; // another state w is used
		if ( height > (last_tracked_state.height + CONS_H_DIFF ) )
			height = last_tracked_state.height + CONS_H_DIFF; // another state w is used
				

		/**** angle ****/
		if ( (last_tracked_state.angle - CONS_A_DIFF ) < 0 )
		{
			rl = 360 + last_tracked_state.angle - CONS_A_DIFF;
			ll = last_tracked_state.angle + CONS_A_DIFF;
		}
		else
		{
			rl = last_tracked_state.angle + CONS_A_DIFF;
			ll = last_tracked_state.angle - CONS_A_DIFF;
		}
		if ( angle > rl )angle = rl;
		if ( angle < ll ) angle = ll; // another state A is used

		cvmSet( p->particles, 0, np, x);
		cvmSet( p->particles, 1, np, y );
        cvmSet( p->particles, 2, np, width );
        cvmSet( p->particles, 3, np, height );
		cvmSet( p->particles, 4, np, angle );
		
# else
		width = MIN( width, imsize.width - x ); // another state x is used
        height = MIN( height, imsize.height - y ); // another state y is used
		cvmSet( p->particles, 2, np, width );
        cvmSet( p->particles, 3, np, height );
# endif

    }
}
# endif

/***************************** Utility Functions ****************************************/

void cvParticleStateDraw( const CvParticleState& state, IplImage* img, CvScalar color )
{
    CvBox32f box32f = cvBox32f( (float)state.x, (float)state.y, (float)state.width, (float)state.height, (float)state.angle );
    CvRect32f rect32f = cvRect32fFromBox32f( box32f );

    cvDrawRectangle( img, rect32f, cvPoint2D32f(0,0), color );

}

void cvParticleStatePrint( const CvParticleState& state )
{
    printf( "x :%d ", int(state.x) );
    printf( "y :%d ", int(state.y) );
    printf( "width :%d ", int(state.width) );
    printf( "height :%d ", int(state.height) );
    printf( "angle :%.2f\n", state.angle );
# ifdef PF_2nd_ORDER_AR
	printf( "xp:%.2f ", state.xp );
    printf( "yp:%.2f ", state.yp );
    printf( "widthp:%.2f ", state.widthp );
    printf( "heightp:%.2f ", state.heightp );
    printf( "anglep:%.2f\n", state.anglep );
    fflush( stdout );
# endif

}

/************************ Utility ******************************************/ 
  
 /** 
  * Get id of the most probable particle 
  * 
  * @param particle 
  * @return int 
  */ 
int cvParticleGetMax( const CvParticle* p ) 
 { 
     double minval, maxval; 
     CvPoint min_loc, max_loc; 
     cvMinMaxLoc( p->weights, &minval, &maxval, &min_loc, &max_loc ); 
     return max_loc.x; 
 } 
  
 /** 
  * Get the mean state (particle) 
  * 
  * @param particle 
  * @param meanp     num_states x 1, CV_32FC1 or CV_64FC1 
  * @return void 
  */ 
void cvParticleGetMean( const CvParticle* p, CvMat* meanp ) 
 { 
     CvMat* weights = NULL; 
     CvMat* particles_i, hdr; 
     int i, j; 
     CV_FUNCNAME( "cvParticleGetMean" ); 
     __BEGIN__; 
     CV_ASSERT( meanp->rows == p->num_states && meanp->cols == 1 ); 
      
     weights = p->weights; 
  
       
     for( i = 0; i < p->num_states; i++ ) 
     { 
        int circular = (int) cvmGet( p->bound, i, 2 ); 
         
		if( !circular ) // usual mean
        {
            particles_i = cvGetRow( p->particles, &hdr, i );
            double mean = 0;
            for( j = 0; j < p->num_particles; j++ )
            {
                mean += cvmGet( particles_i, 0, j ) * cvmGet( weights, 0, j );
            }
            cvmSet( meanp, i, 0, mean );
        }
        else // wrapped mean (angle)
        {
            double wrap = cvmGet( p->bound, i, 1 ) - cvmGet( p->bound, i, 0 );
            particles_i = cvGetRow( p->particles, &hdr, i );
            CvScalar mean = cvAngleMean( particles_i, weights, wrap );
            cvmSet( meanp, i, 0, mean.val[0] );
        }   
     } 
  
     if( weights != p->weights ) 
         cvReleaseMat( &weights ); 
     __END__; 
 } 
 
 /****************************** Helper functions ******************************/ 
 /* 
  * Do normalization of weights 
  * 
  * @param particle 
  * @see cvParticleResample 
  */ 
void cvParticleNormalize( CvParticle* p ) 
 { 
    CvScalar normterm = cvSum( p->weights ); 
    cvScale( p->weights, p->weights, 1.0 / normterm.val[0] ); 

# if 0
	if( !p->logweight )
    {
        CvScalar normterm = cvSum( p->weights );
        cvScale( p->weights, p->weights, 1.0 / normterm.val[0] );
    }
    else // log version
    {
        CvScalar normterm = cvLogSum( p->weights );
        cvSubS( p->weights, normterm, p->weights );
    }
# endif

# ifdef PF_2nd_ORDER_AR
# if 0
	// normalize particle_probs 
	if( p->logprob ) 
	{ 
		CvScalar logsum = cvLogSum( p->particle_probs ); 
		cvSubS( p->particle_probs, logsum, p->particle_probs ); 
	} 
	else 
	{ 
		CvScalar sum = cvSum( p->particle_probs ); 
		cvScale( p->particle_probs, p->particle_probs, 1.0 / sum.val[0] ); 
	} 

	// normalize observe_probs 
	if( p->logprob ) 
	{ 
		CvScalar logsum = cvLogSum( p->observe_probs ); 
		cvSubS( p->observe_probs, logsum, p->observe_probs ); 
	} 
	else 
	{ 
		CvScalar sum = cvSum( p->observe_probs ); 
		cvScale( p->observe_probs, p->observe_probs, 1.0 / sum.val[0] ); 
	}
# endif
# endif

 } 
 ////////////////////////////////////////////////////////////

/** 
  * Re-samples a set of particles according to their weights to produce a 
  * new set of unweighted particles 
  * 
  * Simply copy, not uniform randomly selects 
  * 
  * @param particle 
  * @note Uses See also functions inside. 
  * @see cvParticleNormalize 
  * @see cvParticleGetMax 
  */ 
void cvParticleResample( CvParticle* p, bool normalize /* = true*/ ) 
 { 
     int i, j, np, k = 0; 
     CvMat* particle, hdr; 
     CvMat* new_particles = cvCreateMat( p->num_states, p->num_particles, p->particles->type ); 
     double weight; 
     int max_loc; 
  
	 //cvParticleMarginalize( p );  // added
     if( normalize ) 
     { 
         cvParticleNormalize( p ); 
     } 
  
     k = 0; 
     for( i = 0; i < p->num_particles; i++ ) 
     { 
         particle = cvGetCol( p->particles, &hdr, i ); 
         weight = cvmGet( p->weights, 0, i ); 
         np = cvRound( weight * p->num_particles ); 
         for( j = 0; j < np; j++ ) 
         { 
             cvSetCol( particle, new_particles, k++ ); 
             if( k == p->num_particles ) 
                 goto exit; 
         } 
     } 
  
     max_loc = cvParticleGetMax( p ); 
     particle = cvGetCol( p->particles, &hdr, max_loc ); 
     while( k < p->num_particles ) 
         cvSetCol( particle, new_particles, k++ ); 
  
 exit: 
     cvReleaseMat( &p->particles ); 
     p->particles = new_particles; 
 } 


 /** 
* Create particle_probs, and observe_probs by marginalizing 
* 
* @param particle 
* @todo priors 
*/ 
void cvParticleMarginalize( CvParticle* p ) 
{ 
# if 0
	if( p->logprob ) 
	{ 
		int np, no; 
		CvScalar logsum; 
		CvMat *particle_prob, *observe_prob, hdr; 
		// number of particles of the same state represents priors 
		for( np = 0; np < p->num_particles; np++ ) 
		{ 
			particle_prob = cvGetCol( p->probs, &hdr, np ); 
			logsum = cvLogSum( particle_prob ); 
			cvmSet( p->particle_probs, 0, np, logsum.val[0] ); 
		} 
		// @todo: priors 
		for( no = 0; no < p->num_observes; no++ ) 
		{ 
			observe_prob = cvGetRow( p->probs, &hdr, no ); 
			logsum = cvLogSum( particle_prob ); 
			cvmSet( p->observe_probs, no, 0, logsum.val[0] ); 
		} 
	} 
	else 
	{ 
		// number of particles of the same state represents priors 
		cvReduce( p->probs, p->particle_probs, -1, CV_REDUCE_SUM ); 
		// @todo: priors 
		cvReduce( p->probs, p->observe_probs, -1, CV_REDUCE_SUM ); 
	} 
# endif
} 

 


#endif
