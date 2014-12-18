/***************************************************************************************/
/*! \b Project component: 
 *  \file   SharedMem.c
 *  \brief  Purpose:  Shared memory utilities.
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
 *  - 30 May       2004 EP/GF: Imported into ClearCase
 *
 ***************************************************************************************
 * \b COPYRIGHT: (C) STMicrolectronics 2003,2004 ALL RIGHTS RESEVED\n
 * This program contains proprietary information and it is non to be used,
 * copied, nor disclosed without written consent of STMicroelectronics.
 ***************************************************************************************/

#include "global_ast_defines.h"          /* DBS Global Variables Types */
#ifndef WIN32
# define SYSTEMV
#endif /* WIN32 */

#if !defined (USE_SHMEM_DUMP) && (defined (DBS) || defined (CODECREC_SIM))
# include <sys/shm.h>                  /* SysV IPC shared memory declarations */
# include <sys/types.h>
/*# include <sys/stat.h>*/
# include <stdio.h>                                 /* standard I/O functions */
# include <fcntl.h>
# include <signal.h>                          /* signal handling declarations */
# include <unistd.h>
# include <sys/mman.h>
# include <string.h>
# include <errno.h>



/* *************************************************************************
 *
 *  Functions for SysV shared memory
 *
 * ************************************************************************* */
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
void * ShmCreate(key_t ipc_key, int shm_size, int perm, int fill) 
{
    void * shm_ptr;
    int shm_id;                       /* ID of the IPC shared memory segment */
    shm_id = shmget(ipc_key, shm_size, IPC_CREAT|perm);        /* get shm ID */
    if (shm_id == -1) { 
	return NULL;
    }


    shm_ptr = shmat(shm_id, NULL, 0);                  /* map it into memory */
    if ((int)shm_ptr == -1) {
      fprintf(stderr,"Dentro la funzione: %d %p (%ld)\n",shm_id, shm_ptr,(long int)shm_ptr);
	return NULL;
    }
    memset((void *)shm_ptr, fill, shm_size);                 /* fill segment */
    return shm_ptr;
}
/*
 * Function ShmFind:
 * Find a SysV shared memory segment 
 * Input:  an IPC key value
 *         the shared memory segment size
 * Return: the address of the segment (NULL on error)
 */
void * ShmFind(key_t ipc_key, int shm_size) 
{
    void * shm_ptr;
    int shm_id;                      /* ID of the SysV shared memory segment */
    shm_id = shmget(ipc_key, shm_size, 0);          /* find shared memory ID */
    if (shm_id == -1) {
	return NULL;
    }
    shm_ptr = shmat(shm_id, NULL, 0);                  /* map it into memory */
    if ((int)shm_ptr == -1) {
	return NULL;
    }
    return shm_ptr;
}
/*
 * Function ShmRemove:
 * Schedule removal for a SysV shared memory segment 
 * Input:  an IPC key value
 *         the shared memory segment size
 * Return: 0 on success, -1 on error
 */
int ShmRemove(key_t ipc_key, void * shm_ptr) 
{
    int shm_id;                      /* ID of the SysV shared memory segment */ 
    /* first detach segment */
    if (shmdt(shm_ptr) == -1) {
	return -1;
    }
    /* schedule segment removal */
    shm_id = shmget(ipc_key, 0, 0);                 /* find shared memory ID */
    if (shm_id == -1) {
	if (errno == EIDRM) return 0;
	return -1;
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {             /* ask for removal */
	if (errno == EIDRM) return 0;
	return -1;
    }
    return 0;
}
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
void * CreateShm(char * shm_name, off_t shm_size, mode_t perm, int fill) 
{
    void * shm_ptr;
    int fd;
    int flag;
    /* first open the object, creating it if not existent */
    flag = O_CREAT|O_EXCL|O_RDWR;
    fd = shm_open(shm_name, flag, perm);    /* get object file descriptor */
    if (fd == -1) { 
	fprintf(stderr,"shm_open error (file %s, line %d)\n",__FILE__,__LINE__);
	return NULL;
    }
    /* set the object size */
    if (ftruncate(fd, shm_size)) {
	fprintf(stderr,"error in ftruncate (file %s, line %d)\n",__FILE__,__LINE__);
	return NULL;
    }
    /* map it in the process address space */
    shm_ptr = mmap(NULL, shm_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
	fprintf(stderr,"error in mmap (file %s, line %d)\n",__FILE__,__LINE__);
	return NULL;
    }
    memset((void *) shm_ptr, fill, shm_size);                /* fill segment */
    return shm_ptr;
}
/*
 * Function FindShm:
 * Find a POSIX shared memory segment 
 * Input:  a name
 *         the shared memory segment size
 * Return: the address of the segment (NULL on error)
 */
void * FindShm(char * shm_name, off_t shm_size) 
{
    void * shm_ptr;
    int fd;                           /* ID of the IPC shared memory segment */
    /* find shared memory ID */
    if ((fd = shm_open(shm_name, O_RDWR|O_EXCL, 0)) == -1) {
	/*debug("Cannot open %s\n", shm_name);*/
        fprintf(stderr,"Cannot open ShMemory (file %s, line %d)\n",__FILE__,__LINE__);
	return NULL;
    }
    /* take the pointer to it */
    shm_ptr = mmap(NULL, shm_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
	return NULL;
    }
    return shm_ptr;
}
/*
 * Function RemoveShm:
 * Remove a POSIX shared memory segment 
 * Input:  the object name
 * Return: 0 on success, -1 on error
 */
int RemoveShm(char * shm_name)
{
    return shm_unlink(shm_name);
}
# endif
#endif /* USE_SHMEM_DUMP */

