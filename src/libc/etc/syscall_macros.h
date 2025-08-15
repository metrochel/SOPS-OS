#ifndef _SYSCALL_MACROS_INCL
#define _SYSCALL_MACROS_INCL
#define Syscall_Exit                    0
#define Syscall_PutCharacter            0x100
#define Syscall_ClearScreen             0x101
#define Syscall_Print                   0x102
#define Syscall_PositionedPrint         0x103
#define Syscall_ColouredPrint           0x104
#define Syscall_PositionedColouredPrint 0x105
#define Syscall_SetScreenBounds         0x106
#define Syscall_GetScreenBounds         0x107
#define Syscall_EnableCursor            0x108
#define Syscall_DisableCursor           0x109
#define Syscall_ReadKey                 0x200
#define Syscall_ReadStr                 0x201
#define Syscall_OpenFile                0x300
#define Syscall_CloseFile               0x301
#define Syscall_Read                    0x302
#define Syscall_ReadFull                0x303
#define Syscall_Write                   0x304
#define Syscall_GetFileSize             0x305
#define Syscall_Malloc                  0x400
#define Syscall_Free                    0x401
#define Syscall_System                  0x402
#define Syscall_GetEnvVar               0x403
#define Syscall_SetEnvVar               0x404
#define Syscall_GetUnixTime             0x405
#define Syscall_GetNanosecTime          0x406
#define Syscall_GetProcTime             0x407

#endif