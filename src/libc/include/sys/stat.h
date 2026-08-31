/*
 * stat.h
 */

#ifndef _STAT_INCL
#define _STAT_INCL

#define __need_dev_t
#define __need_ino_t
#define __need_mode_t
#define __need_nlink_t
#define __need_uid_t
#define __need_gid_t
#define __need_off_t
#define __need_blksize_t
#define __need_blkcnt_t
#include <sys/types.h>
#include <etc/timespec.h>
#include <etc/decl.h>

BEGIN_DECLS

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;

    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;

    blksize_t st_blksize;
    blkcnt_t st_blocks;
};

#define S_IFMT 0
#define S_IFBLK 1
#define S_IFCHR 2
#define S_IFIFO 3
#define S_IFREG 4
#define S_IFDIR 5
#define S_IFLNK 6
#define S_IFSOCK 7

#define S_IFWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100

#define S_IRWXG 070
#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010

#define S_IRWXO 07
#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01

#define S_ISUID 04000
#define S_ISGID 02000
#define S_ISVTX 01000

#define S_ISBLK(m) ((m) == (S_IFBLK))
#define S_ISCHR(m) ((m) == (S_IFCHR))
#define S_ISDIR(m) ((m) == (S_IFDIR))
#define S_ISFIFO(m) ((m) == (S_IFIFO))
#define S_ISREG(m) ((m) == (S_IFREG))
#define S_ISLNK(m) ((m) == (S_IFLNK))
#define S_ISSOCK(m) ((m) == (S_IFSOCK))

#define S_TYPEISMSQ(buf) (0)
#define S_TYPEISSEM(buf) (0)
#define S_TYPEISSHM(buf) (0)
#define S_TYPEISTMO(buf) (0)

#define UTIME_NOW (-1)
#define UTIME_OMIT (-2)

int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fchmodat(int, const char *, mode_t, int);
int    fstat(int, struct stat *);
int    fstatat(int, const char *restrict, struct stat *restrict, int);
int    futimens(int, const struct timespec [2]);
int    lstat(const char *restrict, struct stat *restrict);
int    mkdir(const char *, mode_t);
int    mkdirat(int, const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mkfifoat(int, const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    mknodat(int, const char *, mode_t, dev_t);
int    stat(const char *restrict, struct stat *restrict);
mode_t umask(mode_t);
int    utimensat(int, const char *, const struct timespec [2], int);

END_DECLS

#endif //_STAT_INCL
