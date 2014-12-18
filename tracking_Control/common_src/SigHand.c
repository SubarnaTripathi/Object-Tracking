/***************************************************************************************/
/*! \b Project component: 
 *  \file   SigHand.c
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
 * \b COPYRIGHT: (C) STMicrolectronics 2003,2004 ALL RIGHTS RESEVED\n
 * This program contains proprietary information and it is non to be used,
 * copied, nor disclosed without written consent of STMicroelectronics.
 ***************************************************************************************/

#ifndef USE_SHMEM_DUMP
# include <errno.h>                           /* error simbol definitions */
# include <stdio.h>                           /* standard I/O functions */
# include <signal.h>                          /* signal handling declarations */
# include <sys/types.h>
//# include <sys/wait.h>

# ifndef WAIT_ANY
   #define WAIT_ANY -1
# endif

void HandSig_USR1(int);
void HandSig_USR2(int);


unsigned int pause_kill_USR1(int proc_pid)
{
  struct sigaction new_action, old_action;
  sigset_t old_mask, stop_mask, sleep_mask;

  /* set signal handler */
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = HandSig_USR1;
  new_action.sa_flags = 0;
  sigaction(SIGUSR1, &new_action, &old_action);

  /* race conditions bloccate */
  sigemptyset(&stop_mask);
  sigaddset(&stop_mask, SIGUSR1);
  sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);
  /* vado in attesa */
  sleep_mask = old_mask;
  sigdelset(&sleep_mask, SIGUSR1);
  kill(proc_pid,SIGUSR1);
  sigsuspend(&sleep_mask);

  /* restore */
  sigprocmask(SIG_SETMASK, &old_mask, NULL);
  sigaction(SIGUSR1, &old_action, NULL);

  /* return */
  return(0);
}

unsigned int pause_USR1()
{
  struct sigaction new_action, old_action;
  sigset_t old_mask, stop_mask, sleep_mask;

  /* set signal handler */
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = HandSig_USR1;
  new_action.sa_flags = 0;
  sigaction(SIGUSR1, &new_action, &old_action);

  /* race conditions bloccate */
  sigemptyset(&stop_mask);
  sigaddset(&stop_mask, SIGUSR1);
  sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);

  /* vado in attesa */
  sleep_mask = old_mask;
  sigdelset(&sleep_mask, SIGUSR1);
  sigsuspend(&sleep_mask);

  /* restore */
  sigprocmask(SIG_SETMASK, &old_mask, NULL);
  sigaction(SIGUSR1, &old_action, NULL);

  /* return */
  return(0);
}
void HandSig_USR1(int sig)
{
  return;
}


unsigned int pause_kill_USR2(int proc_pid)
{
  struct sigaction new_action, old_action;
  sigset_t old_mask, stop_mask, sleep_mask;

  /* set signal handler */
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = HandSig_USR2;
  new_action.sa_flags = 0;
  sigaction(SIGUSR2, &new_action, &old_action);

  /* race conditions bloccate */
  sigemptyset(&stop_mask);
  sigaddset(&stop_mask, SIGUSR2);
  sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);

  /* vado in attesa */
  sleep_mask = old_mask;
  sigdelset(&sleep_mask, SIGUSR2);
  kill(proc_pid,SIGUSR2);
  sigsuspend(&sleep_mask);

  /* restore */
  sigprocmask(SIG_SETMASK, &old_mask, NULL);
  sigaction(SIGUSR2, &old_action, NULL);

  /* return */
  return(0);
}

unsigned int pause_USR2()
{
  struct sigaction new_action, old_action;
  sigset_t old_mask, stop_mask, sleep_mask;

  /* set signal handler */
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = HandSig_USR2;
  new_action.sa_flags = 0;
  sigaction(SIGUSR2, &new_action, &old_action);

  /* race conditions bloccate */
  sigemptyset(&stop_mask);
  sigaddset(&stop_mask, SIGUSR2);
  sigprocmask(SIG_BLOCK, &stop_mask, &old_mask);

  /* vado in attesa */
  sleep_mask = old_mask;
  sigdelset(&sleep_mask, SIGUSR2);
  sigsuspend(&sleep_mask);

  /* restore */
  sigprocmask(SIG_SETMASK, &old_mask, NULL);
  sigaction(SIGUSR2, &old_action, NULL);

  /* return */
  return(0);
}

void HandSig_USR2(int sig)
{
  return;
}
#endif /* USE_SHMEM_DUMP */
