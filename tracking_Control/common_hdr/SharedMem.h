/***************************************************************************************/ 
/*! \b Project component: 
 *  \file   ./SharedMem.h
 *  \brief  Purpose:  Functions for SysV shared memory
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
# include <sys/shm.h>                  /* SysV IPC shared memory declarations */
# include <sys/types.h>

/*
 * Function ShmCreate:
 * Create and attach a SysV shared memory segment to the current process.
 *
 * First call get a shared memory segment with KEY key access and size SIZE,
 * by creating it with R/W privilege for the user (this is the meaning of
 * the ored flags). The function return an identifier shmid used for any 
 * further reference to the shared memory segment. 
 * Second call attach the shared memory segment to this process and return a
 * pointer to it (of void * type). 
 * Then initialize shared memory to the given value
 *
 * Input:  an IPC key value
 *         the shared memory segment size
 *         the permissions
 *         the fill value
 * Return: the address of the shared memory segment (NULL on error)
 */
# ifdef SYSTEMV
void * ShmCreate(key_t ipc_key, int shm_size, int perm, int fill);

/*
 * Function ShmFind:
 * Find a SysV shared memory segment 
 * Input:  an IPC key value
 *         the shared memory segment size
 * Return: the address of the segment (NULL on error)
 */
void * ShmFind(key_t ipc_key, int shm_size);

/*
 * Function ShmRemove:
 * Schedule removal for a SysV shared memory segment 
 * Input:  an IPC key value
 *         the shared memory segment size
 * Return: 0 on success, -1 on error
 */
int ShmRemove(key_t ipc_key, void * shm_ptr); 

# endif


/* *************************************************************************
 *
 *  Functions for POSIX shared memory
 *
 * ************************************************************************* */
/*
 * Function CreateShm:
 * Create a POSIX shared memory segment and map it to the current process.
 *
 *
 * Input:  a pathname
 *         the shared memory segment size
 *         the permissions
 *         the fill value
 * Return: the address of the shared memory segment (NULL on error)
 */

# ifdef POSIX
void * CreateShm(char * shm_name, off_t shm_size, mode_t perm, int fill);
/*
 * Function FindShm:
 * Find a POSIX shared memory segment 
 * Input:  a name
 *         the shared memory segment size
 * Return: the address of the segment (NULL on error)
 */
void * FindShm(char * shm_name, off_t shm_size) ;
/*
 * Function RemoveShm:
 * Remove a POSIX shared memory segment 
 * Input:  the object name
 * Return: 0 on success, -1 on error
 */
int RemoveShm(char * shm_name);
# endif
#endif /* USE_SHMEM_DUMP */
