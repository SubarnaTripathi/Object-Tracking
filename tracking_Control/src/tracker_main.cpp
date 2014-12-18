/***************************************************************************************/
/*! \b Project component: OBJECT_TRACKER
 *  \file   tracker_main.c
 *  \brief  Purpose:  This is the principal OBJECT TRACKER module.
 *            It controls the command line and calls all the tracking/encoding functions.
 *            It contains the real "main" function.
 *
 *  \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *  \date   2009
 ***************************************************************************************
 *  \mainpage
 *     OBJECT_TRACKER
 *     For detailed documentation see the comments in each file.
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
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

/* {{{ Include Needed Headers */
#define _GNU_SOURCE 1         /* see unistd.h for the getpgid function*/
#include <errno.h>                  /* error definitions and routines */ 
#include <stdlib.h>                 /* C standard library */
#include <stdio.h>	            /* standard I/O library */
#include <string.h>                 /* string functions */
#include <time.h>
#include "TRACKER_types.h"              /* General type definitions */

#ifndef WIN32
#include <unistd.h>                 /* unix standard library */
#include <signal.h>                 /* signal handling declarations */
#include <sys/wait.h>
#include <sys/ipc.h>
#else
#include "WinInterface.h"
#include <assert.h>
#endif

#include "global_defines.h"
#include "shared_mem.h"
#include "global.h"          /* Global Variables */ 
#include "config_type.h"     /* Configuration file tool CONFIG */
#include "config.h"          /* Configuration file tool CONFIG */

# define LEVEL_MESSAGE 2
# define LEVEL_ERROR 2
# define ERR_INP_PARAM_VAL (-4)
# define ERR_RUNTIME_VAL (-13)
# ifdef TRACKER_PRINT
#  define EXIT(a,b)                                                                      \
{                                                                                      \
  TOOL_MessageError((b),(si8_t *)"file %s at line %d:\n\t%s\n",__FILE__,__LINE__,(a));          \
  exit((b));                                                                           \
}
# else
#  define EXIT(a,b) exit((b))
# endif /* TRACKER_PRINT */

#ifndef WIN32
#include "SharedMem.h"
#endif
/* }}} */

/* {{{ Help printing routines */
static void  usage(void);
static si32_t   Fill_ParFile();
/* }}} */

/* {{{ SigTerm Handler */
#ifndef WIN32
static ui32_t SigTerm_Handler_Install();
static void   SigTerm_Handler(si32_t sig);
#endif
/* }}} */

/************************* declaraions *************************************************************/

/* {{{ TRACKER Main Procedure */
#ifdef WIN32
Exchange_Data_t *Exchange_Data_p;
 extern DWORD WINAPI tracking_thread(LPVOID p);
#else
 /*volatile*/ Exchange_Data_t *Exchange_Data_p;
#endif

/**** declaraions ****/
void TRACKER_info(FILE *fd) ;
void TOOL_Message(sl32_t level, si8_t *fmt, ...);
void TOOL_MessageError(sl32_t level, si8_t *fmt, ...);


si32_t main(si32_t argc, char *argv[])
{

  /* {{{ Variables definition */
  si32_t i, error_code=-1;
# ifndef WIN32
  si32_t status;
# endif
  char *TRACKER_config_file = "tracker_config.cfg";
 
  /********************************************************/
#ifndef WIN32
  pid_t pid1=0, pid2=0, pid3=0;
  pid_t pid, track_pid = 0;
  key_t Exchange_Data_Key;
  /* }}} */
  SigTerm_Handler_Install();
#else

  HANDLE tracking_handler;
  DWORD THThreadID;
  CNTRL_STRUCT control_data;
#endif
  if(argc==1)
    usage();
     
  /* {{{ Input section: decode command line parameters */
  i=1;
  while( i<argc )
    {
      switch(argv[i][1])
	{
	  /* 
	   * Handling options 
	   */ 
	case 'h':   /* help option */
	  usage();
	  break;
	case 'v':   /* info option */
	  TRACKER_info(stdout);
	  return -1;
	  break;
	case 'f':   /* configuration file */
	  i += 1;
	  TRACKER_config_file = argv[i];
	  break;
	case 'd':   /* configuration file */
	  Fill_ParFile();
	  i += 1;
	  TRACKER_config_file = argv[i];  
	  DumpDefaultConfigFile(TRACKER_config_file,matrix_configuration_section);
	  TOOL_Message(-1,(si8_t *)"Dumped %s parameter file\n",TRACKER_config_file);
	  return -1;
	  break;
	case '?':   /* unrecognized options */
#ifndef WIN32
	  TOOL_MessageError(-1,(si8_t *)"unknow option, -%c\n",optopt);
#endif
	  usage();
	default:    /* should not reached */
	  TOOL_MessageError(-1,(si8_t *)"bad parameters\n"); 
	  usage();
	}
      i++;
    }
  /* }}} */


  /*
   * Main code beginning 
   */

  if (!( Fill_ParFile() )) usage();   
  
  ParseConfigFile(TRACKER_config_file,matrix_configuration_section);
      
  /* {{{ Print Environment */
  TRACKER_info(stdout);

#ifndef WIN32
  TOOL_Message( 1, (si8_t *)"  Tracking Exe Path :  %s \n",ParFile.tracking_exe_path); 
  TOOL_Message( 1, (si8_t *)"  Tracker          :  %s \n",ParFile.tracking_exe_file);
  
  TOOL_Message( 1, (si8_t *)"  Input File       :  %s \n",ParFile.in_filename);
  TOOL_Message( 1, (si8_t *)"  Tracked OutPut File      :  %s \n",ParFile.tracked_out_filename);
  TOOL_Message( 1, (si8_t *)"\n");
# endif
    
#ifndef WIN32
  /* Shared memory key generation */
  Exchange_Data_Key = getpid(); /*master_pid */
#endif

  /* {{{ setup shared memory */
#ifdef WIN32
  Exchange_Data_p = (Exchange_Data_t *)calloc(1,sizeof(Exchange_Data_t));
  if(!Exchange_Data_p)
    {
      EXIT("Error on Shared Memory creation, %s\n", ERR_RUNTIME_VAL);
    }
#else
# ifdef SYSTEMV
  Exchange_Data_p = (void *) ShmCreate(Exchange_Data_Key, sizeof(Exchange_Data_t), 0666, 0); 
  if (Exchange_Data_p == NULL) 
    {
      si32_t shm_id;                      /* ID of the SysV shared memory segment */ 
      TOOL_MessageError(-1,(si8_t *)"Error on Shared Memory creation\n");
    
      shm_id = shmget(Exchange_Data_Key, 0, 0);                 /* find shared memory ID */
      if (shm_id == -1) 
	{
	  if (errno == EIDRM) 
	    EXIT("", 0);
	  EXIT("", -1);
	}
      if (shmctl(shm_id, IPC_RMID, NULL) == -1) 
	{
	  /* ask for removal */
	  if (errno == EIDRM) 
	    EXIT("", 0);
	  EXIT("", -1);
	}
      TOOL_MessageError(-1,(si8_t *)" Shared Memory Detached Normally: %d \n",Exchange_Data_Key);
      EXIT( "", ERR_RUNTIME_VAL);
    }

# endif /* SYSTEMV */
# ifdef POSIX
#  error "POSIX IS NOT SUPPORTED YET"
# endif

  /* }}} */

  /* SHMEM access synchronization */
  Exchange_Data_p->pid_tracking = 0; 
  track_pid = 0; 
  
#endif /* WIN32 */

  Exchange_Data_p->bIsAVI = ParFile.bIsAVI;
  Exchange_Data_p->frame_height = ParFile.frame_height;
  Exchange_Data_p->frame_width = ParFile.frame_width;
  strcpy( (char *)Exchange_Data_p->TrackedOutFileName, (const char *)ParFile.tracked_out_filename);
  
  Exchange_Data_p->num_objects = (ParFile.num_objects >= MAX_NUM_OBJECTS)? (MAX_NUM_OBJECTS-1) : ParFile.num_objects;
  Exchange_Data_p->num_basis_images = ((ParFile.num_basis_images_minus1+1)>MAX_NUM_BASIS_VECTORS)? (MAX_NUM_BASIS_VECTORS-1) : ParFile.num_basis_images_minus1; // subarna : added
  Exchange_Data_p->b_w = ParFile.b_w;
  Exchange_Data_p->b_h = ParFile.b_h;
  Exchange_Data_p->num_particles = ParFile.num_particles;
  
  Exchange_Data_p->auto_init_basis_dim = ParFile.auto_init_basis_dim;

  if(ParFile.auto_tracker_init)
	  printf("\nWARNING: Automatic Tracker Initialization not supported yet\n"); 
  
#ifdef WIN32
  {
    char sTemp[511];

    control_data.tracking_done = CreateSemaphore(NULL, 0, 1, NULL);
    control_data.end_track_code = CreateSemaphore(NULL, 0, 1, NULL);
    SetThreadPriority(NULL, THREAD_PRIORITY_BELOW_NORMAL );  
  
	{
	  TOOL_Message( 1, (si8_t *)"Appearance-Based ObjectTracking\n");
	  /* create tracking thread */
	  control_data.argc_tracking = 0;

	  sprintf(sTemp,"%s%s",
		  ParFile.tracking_exe_path,
		  ParFile.tracking_exe_file);
				
	  control_data.argv_tracking[control_data.argc_tracking++] = sTemp;
				
	  control_data.argv_tracking[control_data.argc_tracking++] = ParFile.in_filename;

	  char str1[2], str2[4], str3[4];  
	  control_data.argv_tracking[control_data.argc_tracking++] = itoa(ParFile.bIsAVI, str1, 10);
	  control_data.argv_tracking[control_data.argc_tracking++] = itoa(ParFile.frame_width, str2, 10);
	  control_data.argv_tracking[control_data.argc_tracking++] = itoa(ParFile.frame_height, str3, 10);
	 
	  control_data.argv_tracking[control_data.argc_tracking]=NULL;
				
	  tracking_handler = CreateThread(NULL, 2000000, tracking_thread /*tracking_thread_wrapper*/, &control_data, 0, &THThreadID);
	  assert(tracking_handler != NULL);
	  SetThreadPriority(tracking_handler, THREAD_PRIORITY_ABOVE_NORMAL );
		
	}

    /* Wait Till transcoding completes */
    WaitForSingleObject(control_data.end_track_code, INFINITE);

    /********************************************************/
 
    /********************************************************/
    {
      DWORD ExitCode = 0;
      /* Wait for tracker to exit */
      do{
	GetExitCodeThread(tracking_handler, &ExitCode );
      }while(ExitCode == STILL_ACTIVE);
      

      free(Exchange_Data_p);
    
      TOOL_Message( 1, (si8_t *)"\n  GoodBye. \n\n\n");
      Sleep(2000);
    
    }
  }  
#else /* WIN32 */
  {
    si32_t parametr_counter_Dec=0;
    si32_t parametr_counter_Enc=0;
    char Tracker_Command_Line[2048];
    char *Tracker_Parameters[128]; /* maximum number of parameter: 128 */

    /* ############################################# */
    /* Tracker parameters and process start commands */
    /* ############################################# */
    {
      si32_t type_msg = 0;
      
      parametr_counter_Dec=0;
      sprintf(Tracker_Command_Line,"%s%s",
	      ParFile.tracking_exe_path,
	      ParFile.tracking_exe_file);
      
      Tracker_Parameters[parametr_counter_Dec++] = Tracker_Command_Line;         
       {
	    char *base_stream_opt = {"-b"};

	    if(!type_msg)
	      {
		TOOL_Message( 1, (si8_t *)"Appearance-Based ObjectTracking\n");
		type_msg = 1;
	      }
	    Tracker_Parameters[parametr_counter_Dec++] = base_stream_opt;
	    break;
	  }
	
      
      Tracker_Parameters[parametr_counter_Dec++] = ParFile.in_filename;
      Tracker_Parameters[parametr_counter_Dec]=NULL;
    }

   
    /********************************************************/
    /* First CHILD: Tracker */
    if ( (track_pid = pid1 = fork()) < 0) {
      TOOL_MessageError( -1, (si8_t *)"Error on Tracker child creation, %s\n");
      EXIT ( "", ERR_RUNTIME_VAL );
    }
    
#if 0
    {
      int par;
 
      for(par=0; par<parametr_counter_Dec; par++)
	fprintf(stderr,"%s ",Tracker_Parameters[par]);
      fprintf(stderr,"\n");
    }
#endif /* 0 */

    if (pid1 == 0) 
      {
	/*****************/
	/* Tracker child */
	/*****************/
	if ( execv(Tracker_Command_Line,Decoder_Parameters) < 0)  {
	  EXIT ( "Error on EXECUTING Tracker module process", ERR_RUNTIME_VAL );	
	}
      } 
    else 
      { /* Father */
	/* TOOL_Message( 1, (si8_t *)"\t\tPid Tracker  %d\n",track_pid);  */
	/* Second CHILD: H264 ENCODER */
	  { /* Father */
	    /* Master */
		/* start childs : POLLING on Shared Memory */        

		TOOL_Message( 1, (si8_t *)"wait until the Tracker gets ready\n"); 

		do
		  { /* wait until the Tracker is ready */ }
		while(Exchange_Data_p->pid_tracking != track_pid);
	      	      
		kill(track_pid,SIGUSR1);  /* To let the childs to proceed autonomously */
	      
		/* wait for childs */
		while (track_pid ) 
		  {
		    pid = wait(&status);
	    
		    /* The Tracker is exited */
		    if (track_pid == pid) 
		      {		
			if (WIFEXITED(status))
			  {
			    TOOL_Message( -1, (si8_t *)"\nTRACKER: Tracker Normal Exit. (EOF)");
			    
			  }
			else
			  { 
			    TOOL_Message( 1, (si8_t *)"\nnTRACKER: Tracker Forced Exit ! ");
			    
			  }
			track_pid = 0;
		    
			break;
		      }
		      }
		  
            
		/* destroy shared memory */
#ifdef SYSTEMV
		if (ShmRemove(Exchange_Data_Key, (void *)Exchange_Data_p) != 0) 
		  {
		    EXIT("Error on Shared Memory Destroy\n", ERR_RUNTIME_VAL);
		  }
#endif
#ifdef POSIX
#  error "POSIX IS NOT SUPPORTED YET"
#endif
		TOOL_Message( 1, (si8_t *)"\n\nHave a nice day. ;-)\n\n");
	      
	      
	  }
	/* normal exit */
	error_code = 0;
	return error_code;
      }
  }
#endif /* WIN32 */
  
  /* normal exit */
  return error_code;
}
/* }}} */

/*
***********************************************************************************
*  STMicroelectronics
***********************************************************************************
*/

/* {{{ * routine to fill the Default ParFile Structure */
si32_t Fill_ParFile() 
{
  ParFile.bIsAVI = 1;
  ParFile.frame_width = 832;
  ParFile.frame_height = 480;
  sprintf( ParFile.tracking_exe_path,"%s","../bin/");
  sprintf( ParFile.tracking_exe_file,"%s","");
  sprintf( ParFile.in_filename,"%s","");
  sprintf( ParFile.tracked_out_filename,"%s","./tracked_out.avi");
  
  return 1;
}
/* }}} */

/* {{{ * routine to print usage info and exit */
void usage(void) 
{
  TOOL_Message( 1, (si8_t *)"\n_____________________________________________________________________\n");
  TOOL_Message( 1, (si8_t *)"\tAppearance-based Object tracking version %s\n\t%s\n", TRACKER_VERSION, TRACKER_AUTHOR);
  TOOL_Message( 1, (si8_t *)"_____________________________________________________________________\n");
  TOOL_Message( 1, (si8_t *)"Usage:\n");
  TOOL_Message( 1, (si8_t *)"  -h               print this help\n");
  TOOL_Message( 1, (si8_t *)"  -d file          generate foo parameter file\n");
  TOOL_Message( 1, (si8_t *)"  -v               print version info\n");
  TOOL_Message( 1, (si8_t *)"  -f file          execute tracking with specific parameter file\n\n\n");
  
  EXIT ( "", ERR_INP_PARAM_VAL );
}
/* }}} */

#ifndef WIN32
/* {{{ Kill & Quit Handler */
static unsigned int SigTerm_Handler_Install()
{
  struct sigaction new_action, old_action;
  sigset_t stop_mask;
  /*old_mask, stop_mask, sleep_mask;*/

  /* set signal handler */
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = SigTerm_Handler;
  new_action.sa_flags = 0;

  sigaction(SIGTERM, &new_action, &old_action);
  sigaction(SIGQUIT, &new_action, &old_action);
  sigaction(SIGINT, &new_action, &old_action);

  sigemptyset(&stop_mask);
  sigaddset(&stop_mask, SIGTERM);

  /* return */
  return(0);
}


static void SigTerm_Handler(si32_t sig)
{ 
  key_t Exchange_Data_Key;
  pid_t pid;

  TRACKER_info(stdout);
  TOOL_Message( 1, (si8_t *)"\n\n Forced Exiting ....");
  Exchange_Data_Key = getpid();
  Exchange_Data_p = (void *)ShmFind(Exchange_Data_Key, sizeof(Exchange_Data_t));
  if (Exchange_Data_p != NULL)
    {
      /* destroy shared memory */
#ifdef SYSTEMV
      /* Kill Child */
      pid = Exchange_Data_p->pid_tracking;
      if ((pid) && (getpgid(pid)>0))
	{
	  TOOL_Message(-1,(si8_t *)"\n Killing Tracker ...\n");
	  kill(pid,SIGKILL);
	}

      /* Free Memory */
      if (ShmRemove(Exchange_Data_Key, (void *)Exchange_Data_p) != 0) 
	{
	  TOOL_Message(-1,(si8_t *)" Error on Shared Memory Destroy\n");
	  /* exit(-1); */
	}
      else
	{
	  TOOL_Message( 1,(si8_t *)" Shared Memory Detached Normally: %d \n",Exchange_Data_Key);
	}
	
#endif
#ifdef POSIX
#  error "POSIX IS NOT SUPPORTED YET"
#endif
    }
  EXIT("\n  GoodBye. \n\n\n", NO_ERR);
  
  return;
}
/* }}} */
#endif



void TRACKER_info(FILE *fd) 
{
  fprintf(fd,
	  "            ______________________________________________________________ \n"
	  "                   _______________        \n"
	  "                  /______________/        \n"
	  "                 // _______  ___          \n"          
	  "                / \\ \\     / /  /        \n"         
	  "               /   \\ \\   / /  /         \n"             
	  "              /_____\\ | / /  /           \n"            
	  "             ________/_/ /__/ (R)         \n"
	  "                                          \n"
	  "            AST-India                     \n");
  fprintf(fd,
	  "\n"
	  "            Project: OBJECT TRACKER   \n\n");

  fprintf(fd,
	  "            Authors and contributors:\n");
  fprintf(fd,
	  "               Advance System Technology: \n"
	  "                     Subarna Tripathi      subarna.tripathi@st.com\n");
	
  fprintf(fd,"\n");
  fprintf(fd,
	  "            Appearance Based Object Tracking  			 \n"
	  "           ______________________________________________________________ \n");
  fprintf(fd,"\t    OBJECT_TRACKER, (appearance-based object-tracking), version %s\n",TRACKER_VERSION);
  fprintf(fd,"\t    (C) 2010 %s\n",TRACKER_AUTHOR);
  fprintf(fd,"           ______________________________________________________________ \n");
}

/***************************************************************************************/
/*!
 *  \fn void TOOL_Message(sl32_t level, si8_t *fmt, ...)
 *                                  
 *  \brief This function writes one Message to stdout
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-27
 * \param IN : sl32_t* level : message level
 * \param IN : si8_t*  fmt   : pointer to the list of arguments
 * \return None
 *
 ***************************************************************************************/ 
void TOOL_Message(sl32_t level, si8_t *fmt, ...)
{
  va_list args;
  
  if( LEVEL_MESSAGE >= level )
    {
      va_start(args, fmt);
      vfprintf(stdout,(char *)fmt, args);
      va_end(args);
    }
  fflush(stdout);
  
  return ; 
} /* End of Subroutine TOOL_Message() */

/***************************************************************************************/
/*!
 *  \fn void TOOL_MessageError(sl32_t level, si8_t *fmt, ...)
 *                                  
 *  \brief This function writes one Error Message to stdout and calls the exit functions
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-12-01
 * \param IN : sl32_t* level : message level
 * \param IN : si8_t*  fmt   : pointer to the list of arguments
 * \return None
 *
 ***************************************************************************************/ 
void TOOL_MessageError(sl32_t level, si8_t *fmt, ...)
{
    va_list args;

    if( LEVEL_ERROR >= level )
    {
        va_start(args, fmt);
        fprintf(stderr,"\n\n***************************\nError: ");
        vfprintf(stderr,(char *)fmt, args);
        va_end(args);
        fprintf(stderr,"***************************\n");
    }
    fflush(stderr);

    return ;
} /* End of Subroutine TOOL_MessageError() */

