/***************************************************************************************/ 
/*! \b Project component: OBJECT_TRACKER
 *  \file   global.h
 *  \brief  Purpose:  choose between declaration and definition
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009
 ***************************************************************************************
 *
 *  \author
 *    ST (2009)
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 26 November  2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/

#include <stdio.h>
#include "gtypes.h"

#ifndef GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

#ifndef HGLOBALH
#define HGLOBALH
/* }}} */

/* General Purpose Global Variables */
struct ParFile_t ParFile;  

#endif /*HGLOBALH*/
/*END FILE*/
