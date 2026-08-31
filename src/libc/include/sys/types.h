//
//	types.h - Заголовок с типами
//
//	- Содержит типы. А вы что думали?
//

// Если вспомогательные заголовки __need_X не определены,
// то заголовок включает программа, а значит, ей нужно всё.
#if    !defined(_TYPES_INCL) && !defined(__need_pid_t) \
        && !defined(__need_mode_t) && !defined(__need_size_t) \
        && !defined(__need_ssize_t) && !defined(__need_off_t) \
        && !defined(__need_dev_t) && !defined(__need_ino_t) \
        && !defined(__need_uid_t) && !defined(__need_gid_t) \
        && !defined(__need_blksize_t) && !defined(__need_blkcnt_t) \
        && !defined(__need_nlink_t)
#define _TYPES_INCL 1
#define __need_pid_t
#define __need_mode_t
#define __need_size_t
#define __need_ssize_t
#define __need_off_t
#define __need_dev_t
#define __need_ino_t
#define __need_uid_t
#define __need_gid_t
#define __need_blksize_t
#define __need_blkcnt_t
#define __need_nlink_t
#include <etc/decl.h>

BEGIN_DECLS

//=====================================
#if defined(__need_pid_t) && !defined(_PID_T_INCL)
#define _PID_T_INCL
// `pid_t` - это тип, определяющий PID.
typedef unsigned int pid_t;
#undef __need_pid_t
#endif

#if defined(__need_mode_t) && !defined(_MODE_T_INCL)
#define _MODE_T_INCL
// `mode_t` - это тип, определяющий режим открытия файла.
typedef unsigned short mode_t;
#undef __need_mode_t
#endif

#if defined(__need_size_t) && !defined(_size_t_incl)
#define _size_t_incl
// `size_t` - это тип, определяющий размер чего-либо.
typedef __SIZE_TYPE__ size_t;
#undef __need_size_t
#endif

#if defined(__need_ssize_t) && !defined(_ssize_t_incl)
#define _ssize_t_incl
// `ssize_t` - это знаковый комплимент `size_t`.
typedef long long ssize_t;
#undef __need_ssize_t
#endif

#if defined(__need_off_t) && !defined(_OFF_T_INCL)
#define _OFF_T_INCL
// `off_t` - это тип, определяющий сдвиг файла.
typedef long long off_t;
#undef __need_off_t
#endif

#if defined(__need_dev_t) && !defined(_dev_t_incl)
#define _dev_t_incl
// `dev_t` - это тип, определяющий номер устройства.
typedef unsigned int dev_t;
#undef __need_dev_t
#endif

#if defined(__need_ino_t) && !defined(_ino_t_incl)
#define _ino_t_incl
// `ino_t` - это тип, определяющий серийный номер файла.
typedef unsigned int ino_t;
#undef __need_ino_t
#endif

#if defined(__need_nlink_t) && !defined(_nlink_t_incl)
#define _nlink_t_incl
// `nlink_t` - это тип, определяющий количество жёстких ссылок на файл.
typedef unsigned int nlink_t;
#undef __need_nlink_t
#endif

#if defined(__need_uid_t) && !defined(_uid_t_incl)
#define _uid_t_incl
// `uid_t` - это тип, определяющий идентификатор пользователя-владельца файла.
typedef unsigned short uid_t;
#undef __need_uid_t
#endif

#if defined(__need_gid_t) && !defined(_gid_t_incl)
#define _gid_t_incl
// `gid_t` - это тип, определяющий идентификатор группы-владельца файла.
typedef unsigned short gid_t;
#undef __need_gid_t
#endif

#if defined(__need_blksize_t) && !defined(_blksize_t_incl)
#define _blksize_t_incl
// `blksize_t` - это тип, определяющий размер блока.
typedef unsigned short blksize_t;
#undef __need_blksize_t
#endif

#if defined(__need_blkcnt_t) && !defined(_blkcnt_t_incl)
#define _blkcnt_t_incl
// `blkcnt_t` - это тип, определяющий количество блоков.
typedef unsigned short blkcnt_t;
#undef __need_blkcnt_t
#endif

#if defined(__need_blkcnt_t) && !defined(_blkcnt_t_incl)
#define _blkcnt_t_incl
// `blkcnt_t` - это тип, определяющий количество блоков.
typedef unsigned short gid_t;
#undef __need_blkcnt_t
#endif

END_DECLS

#endif