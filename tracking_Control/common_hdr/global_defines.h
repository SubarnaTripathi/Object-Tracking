/***************************************************************************************/
/*! \b Project component: OBJECT_TRACKER
 *  \file   global_defines.h
 *  \brief  Purpose: compilation and global defines of the whole TRACKER project
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009-11-30
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 30 November  2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
#ifndef HGLOBALDEFINESH
# define HGLOBALDEFINESH 

# define TRACKER_VERSION "2.0"
# define TRACKER_AUTHOR "ST_IITD Collaborator"


/* ---------- Defines for TRACKER ---------- */
# define MAX_NUM_BASIS_VECTORS 8

# define MAX_NUM_OBJECTS 8

/*! uncomment the following line if you want printf/fprintf built-in functions in the whole library */
//# define TRACKER_PRINT 

/*! uncomment the following line if you want to include X11 support */
# ifndef WIN32
/* #  define DISPLAY */
# endif


/**************************/
/*! Automatic definitions */
 /* HDTV case (1920x1088 => 120*68 */
#  define MAX_H_MB_NUM   120
#  define MAX_V_MB_NUM   68

# define MAX_MB_NUM (MAX_H_MB_NUM*MAX_V_MB_NUM)

#ifndef WIN32
# define SYSTEMV
#endif /* WIN32 */

/****************************************************/
#define CALC_TIME  /* this define, if ON, calculates time elapsed in transcoding */

#endif /* HGLOBALDEFINESH */
