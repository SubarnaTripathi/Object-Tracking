/***************************************************************************************/
/*! \b Project component: OBJECT_TRACKER 
 *  \file   TRACKER_types.h
 *  \brief  Purpose: Definition of data-types used inside the whole TRACKER project
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009-12-01
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 1 December      2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/

#include <stdio.h>

#ifndef HTRACKERTYPESH
# define HTRACKERTYPESH

/*! ANSI-C basic type ri-definition */
typedef char strn_t;           /*!< 8-bit  signed   char    type used for name */

typedef signed char si8_t;     /*!< 8-bit  signed   char    type */
typedef unsigned char ui8_t;   /*!< 8-bit  unsigned char    type */

typedef signed short si16_t;   /*!< 16-bit signed   short   type */
typedef unsigned short ui16_t; /*!< 16-bit unsigned short   type */

typedef signed int si32_t;     /*!< 32-bit signed   integer type on 32-bits architectures */
typedef unsigned int ui32_t;   /*!< 32-bit unsigned integer type on 32-bits architectures */

typedef signed int sl32_t;    /*!< 32-bit signed   integer type on 32-bits architectures */
typedef unsigned int ul32_t;  /*!< 32-bit unsigned integer type on 32-bits architectures */

typedef float f32_t;           /*!< 32-bit floating point type */
typedef double f64_t;          /*!< 64-bit floating point type */

#  ifdef WIN32
typedef __int64                  si64_t;/*!< 64-bit signed   integer type */
#  else
#   ifdef NON_ANSI_ALLOWED
typedef long long                si64_t;/*!< 64-bit signed   integer type */
typedef unsigned long long       ul64_t; /*!< 64-bit unsigned integer type */
#   else
typedef long                     si64_t;/*!< 64-bit signed   integer type */
typedef unsigned long            ul64_t; /*!< 64-bit unsigned integer type */
#   endif /* NON_ANSI_ALLOWED  */
#  endif


/* Types definition */

/* MATTEO NACCARI */
/*! \typedef Selection: for H264 to MPEG2 transcoding only */
typedef struct Selection{
  int secondfield;
  int self; /*0=TOP , 1=BOTTOM*/
  int selr; /*0=TOP , 1=BOTTOM*/
}selection;
/* MATTEO NACCARI */

/*! \typedef bool_et */
/*! \enum bool_et */
/*! \brief boolean type */
# if !defined (TRUE) && !defined (FALSE)
typedef enum
{
  FALSE=0,
  TRUE=1
}
bool_et;
# else
/* TRUE and/or FALSE are already defined */
#  if defined (TRUE) && defined (FALSE) 
#   define bool_et int
#  else
#   ifdef TRUE
#    define FALSE (!TRUE)
#   else
#    define TRUE (!FALSE)
#  endif /* TRUE */
# endif /* TRUE && FALSE */

# endif /* !TRUE && !FALSE */
#endif /* HTRACKERTYPESH */
