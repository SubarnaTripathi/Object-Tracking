/***************************************************************************************/ 
/*! \b Project component: 
 *  \file   SigHand.h
 *  \brief  Purpose:  Processes management
 *
 *  \author <a href="mailto: gianluca.filippini@st.com"> Gianluca Filippini (gianluca.filippini@st.com) </a>
 *  \author <a href="mailto: emiliano.piccinelli@st.com"> Emiliano Piccinelli (emiliano.piccinelli@st.com) </a>
 *  \date   2003
 ***************************************************************************************
 *
 *  \author
 *    Advanced System Technology lab (2003)
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 * \b Organization: AST Agrate
 *
 * \b History:
 *  - 26 November  2003 GF: Creation
 *  - 12 July      2004 EP/GF: Imported into ClearCase
 *
 ***************************************************************************************
 * \b COPYRIGHT: (C) STMicrolectronics 2003,2004 ALL RIGHTS RESEVED

 * This program contains proprietary information and it is non to be used,
 * copied, nor disclosed without written consent of STMicroelectronics.
 ***************************************************************************************/

#ifndef USE_SHMEM_DUMP

void HandSig_USR1(int);
void HandSig_USR2(int);


unsigned int pause_kill_USR1(int proc_pid);
unsigned int pause_USR1();

unsigned int pause_kill_USR2(int proc_pid);

unsigned int pause_USR2();

void HandSig_USR2(int sig);

#endif /* USE_SHMEM_DUMP */
