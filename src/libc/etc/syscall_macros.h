#ifndef _SYSCALL_MACROS_INCL
#define _SYSCALL_MACROS_INCL

#define Syscall_Exit                    0x0
#define Syscall_PutCharacter            0x1000
#define Syscall_ClearScreen             0x1001
#define Syscall_Print                   0x1002
#define Syscall_PositionedPrint         0x1003
#define Syscall_ColouredPrint           0x1004
#define Syscall_PositionedColouredPrint 0x1005
#define Syscall_SetScreenBounds         0x1006
#define Syscall_GetScreenBounds         0x1007
#define Syscall_EnableCursor            0x1008
#define Syscall_DisableCursor           0x1009
#define Syscall_ReadKey                 0x2000
#define Syscall_ReadStr                 0x2001
#define Syscall_OpenFile                0x3000
#define Syscall_CloseFile               0x3001
#define Syscall_Read                    0x3002
#define Syscall_ReadChar                0x3003
#define Syscall_Write                   0x3004
#define Syscall_WriteChar               0x3005
#define Syscall_GetFileSize             0x3006
#define Syscall_AllocateTmpFile         0x3007
#define Syscall_RemoveFile              0x3008
#define Syscall_MoveFile                0x3009
#define Syscall_Malloc                  0x4000
#define Syscall_Free                    0x4001
#define Syscall_System                  0x4002
#define Syscall_GetEnvVar               0x4003
#define Syscall_SetEnvVar               0x4004
#define Syscall_GetUnixTime             0x4005
#define Syscall_GetNanosecTime          0x4006
#define Syscall_GetProcTime             0x4007
#define Syscall_StartThread             0x5000
#define Syscall_ExitThread              0x5001
#define Syscall_GetCurrentThread        0x5002
#define Syscall_ThreadYield             0x5003
#define Syscall_JoinThread              0x5004
#define Syscall_BlockThread             0x5005
#define Syscall_TimedBlockThread        0x5006
#define Syscall_DetachThread            0x5007
#define Syscall_CreateMutex             0x5008
#define Syscall_CreateCondVar           0x5009
#define Syscall_CreateTSS               0x500A
#define Syscall_DestroyMutex            0x500B
#define Syscall_DestroyCondVar          0x500C
#define Syscall_DestroyTSS              0x500D
#define Syscall_GetMutexLocked          0x500E
#define Syscall_LockMutex               0x500F
#define Syscall_UnlockMutex             0x5010
#define Syscall_SignalCondVar           0x5011
#define Syscall_BroadcastCondVar        0x5012
#define Syscall_GetTSS                  0x5013
#define Syscall_SetTSS                  0x5014

#endif