/*
 * fcntl.h - Управление файлами
 *
 * Библиотека POSIX для более удобного управления файлами.
 */

#ifndef _FCNTL_INCL
#define _FCNTL_INCL

#include <etc/decl.h>

BEGIN_DECLS

#define __need_pid_t
#define __need_mode_t
#define __need_off_t
#include <sys/types.h>

#define F_DUPFD 1
#define F_DUPFD_CLOEXEC 2
#define F_GETFD 3
#define F_SETFD 4
#define F_GETFL 5
#define F_SETFL 6
#define F_GETLK 7
#define F_SETLK 8
#define F_SETLKW 9
#define F_GETOWN 10
#define F_SETOWN 11

#define F_CLOEXEC 1

#define F_RDLCK 1
#define F_UNLCK 2
#define F_WRLCK 3

#ifndef SEEK_SET
# define SEEK_SET 0
#endif

#ifndef SEEK_CUR
# define SEEK_CUR 1
#endif

#ifndef SEEK_END
# define SEEK_END 2
#endif

#define O_CLOEXEC           0b1
#define O_CREAT             0b10
#define O_DIRECTORY         0b100
#define O_EXCL              0b1000
#define O_NOCTTY            0b10000
#define O_NOFOLLOW          0b100000
#define O_TRUNC             0b1000000
#define O_TTY_INIT          0b10000000

#define O_APPEND 1
#define O_DSYNC 2
#define O_NONBLOCK 3
#define O_RSYNC 4
#define O_SYNC 5

#define O_ACCMODE 0xFF

#define O_EXEC 1
#define O_RDONLY 2
#define O_RDWR 3
#define O_SEARCH 4
#define O_WRONLY 5

#define AT_FDCWD 1

#define AT_EACCESS 1

#define AT_SYMLINK_FOLLOW 1

#define AT_REMOVEDIR 1

#define POSIX_FADV_DONTNEED 1
#define POSIX_FADV_NOREUSE 2
#define POSIX_FADV_NORMAL 3
#define POSIX_FADV_RANDOM 4
#define POSIX_FADV_SEQUENTIAL 5
#define POSIX_FADV_WILLNEED 6

struct flock {
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
};

int creat(const char *name, mode_t mode);

int fcntl(int, int, ...);

int open(const char*, int, ...);

int openat(int, const char*, int, ...);

int posix_fadvice(int, off_t, off_t, int);

int posix_fallocate(int, off_t, off_t);

END_DECLS

#endif //_FCNTL_INCL
