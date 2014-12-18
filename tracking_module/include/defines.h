/***************************************************************************************/
/*! Project component: tracking
 *  \file   defines.h
 *  \brief  Purpose: This file contains the defines for object-tracking and object-coding
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
 *************************************************************************************/
/*** this is for using normalized color histogram as importance sampling while tracking ***/
# define IMPORTANCE_SAMPLING  /**** keep it ON in general */
# define IS_ITERATIONS 2  //3

/*** this is for multiple object handling ***/
//# define MULTIPLE_OBJECTS     /* make this always ON */

/**** if this define is not ON, then closest to incremental mean is taken as tracked object definition ****/
# define DIST_TO_SS

/*** If this define is ON, it detects occlusion, otherwise assumes objects always present  *****/
/*** but after detection of occlusion, it doesn't again check for future presence ***/
//# define OCCLUSION   

/* If this define is ON, print statements are enabled */
//# define DEBUG_PRINT 

/*** this is to put another another strong restriction for drawn samples from the last tracked position, sometimes effective, sometimes not *****/
/*# define ADDITIONAL_BOUND*/  /**** keep it OFF in general */

/* uncomment this define for dumping ROI coding paramaters additionally in another text file */
//# define PARAM_FILE_WRITE

/* uncomment this define for setting different basis image size for different objects */
# define OBJ_FEAT_SIZE

/* uncomment this define for quantizing floating projection coefficients into coarse fixed point */
# define FIXED_POINT_PROJ_COEFF 
# define MULT_FACTOR	1000

// if the below define is OFF, then 1st order AR dynamics model is used (next = current + noise)
//# define PF_2nd_ORDER_AR  //2nd order AR dynamics model ( next = current + speed + noise )

# define M_PI 3.14159265358979323846 // math.h included??? _USE_MATH_DEFINES defined.. still M_PI not defined, why???

# define GENERIC


//# define MAKE_VIDEO

//# define OPTIMIZE


# ifdef OPTIMIZE
# define REF_MAT
# define LOCAL_MEAN
# endif



