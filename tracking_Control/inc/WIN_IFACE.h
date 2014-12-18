
#ifdef  __cplusplus
extern "C" {
#endif


#include <stdio.h>

typedef void* HANDLE;
//typedef long LONG;
typedef long* LPLONG;
//typedef U32_WMC DWORD;
//typedef int BOOL;

typedef struct 
{
	int argc_tracking;
	char *argv_enc[128];
	char *argv_tracking[128];
	HANDLE encoding_done;
	HANDLE tracking_done;
    HANDLE end_track_code;

} CNTRL_STRUCT;

HANDLE CreateSemaphore_AST();

extern CNTRL_STRUCT *control_data_ptr;
#ifdef __cplusplus
}
#endif