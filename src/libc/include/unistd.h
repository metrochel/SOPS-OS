//
//	unistd.h - Доступ к API POSIX
//
//	Позволяет программе использовать API POSIX-совместимых систем.
//	(СОпС, похоже, будет входить в их число.)
//

#if !defined(_UNISTD_INCL)
#define _UNISTD_INCL 1
#include <etc/decl.h>

#define __need_size_t
#define __need_ssize_t
#define __need_uid_t
#define __need_gid_t
#define __need_off_t
#define __need_pid_t
#include <sys/types.h>

#define __need_NULL
#include <stddef.h>

#include <etc/FILE.h>

BEGIN_DECLS

#define _POSIX_VERSION 200809L
#define _POSIX2_VERSION 200809L
#define _XOPEN_VERSION 700

#define _POSIX_ASYNC_IO -1
#define _POSIX_PRIO_IO -1
#define _POSIX_SYNC_IO -1

#define F_OK 0b1
#define R_OK 0b10
#define W_OK 0b100
#define X_OK 0b1000

#define _CS_PATH 1
#define _CS_POSIX_V7_ILP32_OFF32_CFLAGS 2
#define _CS_POSIX_V7_ILP32_OFF32_LDFLAGS 3
#define _CS_POSIX_V7_ILP32_OFF32_LIBS 4
#define _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS 5
#define _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS 6
#define _CS_POSIX_V7_ILP32_OFFBIG_LIBS 7
#define _CS_POSIX_V7_LP64_OFF64_CFLAGS 8
#define _CS_POSIX_V7_LP64_OFF64_LDFLAGS 9
#define _CS_POSIX_V7_LP64_OFF64_LIBS 10
#define _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS 11
#define _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS 12
#define _CS_POSIX_V7_LPBIG_OFFBIG_LIBS 13
#define _CS_POSIX_V7_THREADS_CFLAGS 14
#define _CS_POSIX_V7_THREADS_LDFLAGS 15
#define _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS 16
#define _CS_V7_ENV 17

#define F_LOCK  0b1
#define F_TEST  0b10
#define F_TLOCK 0b100
#define F_ULOCK 0b1000

#define _PC_2_SYMLINKS 1
#define _PC_ALLOC_SIZE_MIN 2
#define _PC_ASYNC_IO 3
#define _PC_CHOWN_RESTRICTED 4
#define _PC_FILESIZEBITS 5
#define _PC_LINK_MAX 6
#define _PC_MAX_CANON 7
#define _PC_MAX_INPUT 8
#define _PC_NAME_MAX 9
#define _PC_NO_TRUNC 10
#define _PC_PATH_MAX 11
#define _PC_PIPE_BUF 12
#define _PC_PRIO_IO 13
#define _PC_REC_INCR_XFER_SIZE 14
#define _PC_REC_MAX_XFER_SIZE 15
#define _PC_REC_MIN_XFER_SIZE 16
#define _PC_REC_XFER_ALIGN 17
#define _PC_SYMLINK_MAX 18
#define _PC_SYNC_IO 19
#define _PC_TIMESTAMP_RESOLUTION 20
#define _PC_VDISABLE 21

#define _SC_2_C_BIND 1
#define _SC_2_C_DEV 2
#define _SC_2_CHAR_TERM 3
#define _SC_2_FORT_DEV 4
#define _SC_2_FORT_RUN 5
#define _SC_2_LOCALEDEF 6
#define _SC_2_PBS 7
#define _SC_2_PBS_ACCOUNTING 8
#define _SC_2_PBS_CHECKPOINT 9
#define _SC_2_PBS_LOCATE 10
#define _SC_2_PBS_MESSAGE 11
#define _SC_2_PBS_TRACK 12
#define _SC_2_SW_DEV 13
#define _SC_2_UPE 14
#define _SC_2_VERSION 15
#define _SC_ADVISORY_INFO 16
#define _SC_AIO_LISTIO_MAX 17
#define _SC_AIO_MAX 18
#define _SC_AIO_PRIO_DELTA_MAX 19
#define _SC_ARG_MAX 20
#define _SC_ASYNCHRONOUS_IO 21
#define _SC_ATEXIT_MAX 22
#define _SC_BARRIERS 23
#define _SC_BC_BASE_MAX 24
#define _SC_BC_DIM_MAX 25
#define _SC_BC_SCALE_MAX 26
#define _SC_BC_STRING_MAX 27
#define _SC_CHILD_MAX 28
#define _SC_CLK_TCK 29
#define _SC_CLOCK_SELECTION 30
#define _SC_COLL_WEIGHTS_MAX 31
#define _SC_CPUTIME 32
#define _SC_DELAYTIMER_MAX 33
#define _SC_EXPR_NEST_MAX 34
#define _SC_FSYNC 35
#define _SC_GETGR_R_SIZE_MAX 36
#define _SC_GETPW_R_SIZE_MAX 37
#define _SC_HOST_NAME_MAX 38
#define _SC_IOV_MAX 39
#define _SC_IPV6 40
#define _SC_JOB_CONTROL 41
#define _SC_LINE_MAX 42
#define _SC_LOGIN_NAME_MAX 43
#define _SC_MAPPED_FILES 44
#define _SC_MEMLOCK 45
#define _SC_MEMLOCK_RANGE 46
#define _SC_MEMORY_PROTECTION 47
#define _SC_MESSAGE_PASSING 48
#define _SC_MONOTONIC_CLOCK 49
#define _SC_MQ_OPEN_MAX 50
#define _SC_MQ_PRIO_MAX 51
#define _SC_NGROUPS_MAX 52
#define _SC_OPEN_MAX 53
#define _SC_PAGE_SIZE 54
#define _SC_PAGESIZE 55
#define _SC_PRIORITIZED_IO 56
#define _SC_PRIORITY_SCHEDULING 57
#define _SC_RAW_SOCKETS 58
#define _SC_RE_DUP_MAX 59
#define _SC_READER_WRITER_LOCKS 60
#define _SC_REALTIME_SIGNALS 61
#define _SC_REGEXP 62
#define _SC_RTSIG_MAX 63
#define _SC_SAVED_IDS 64
#define _SC_SEM_NSEMS_MAX 65
#define _SC_SEM_VALUE_MAX 66
#define _SC_SEMAPHORES 67
#define _SC_SHARED_MEMORY_OBJECTS 68
#define _SC_SHELL 69
#define _SC_SIGQUEUE_MAX 70
#define _SC_SPAWN 71
#define _SC_SPIN_LOCKS 72
#define _SC_SPORADIC_SERVER 73
#define _SC_SS_REPL_MAX 74
#define _SC_STREAM_MAX 75
#define _SC_SYMLOOP_MAX 76
#define _SC_SYNCHRONIZED_IO 77
#define _SC_THREAD_ATTR_STACKADDR 78
#define _SC_THREAD_ATTR_STACKSIZE 79
#define _SC_THREAD_CPUTIME 80
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 81
#define _SC_THREAD_KEYS_MAX 82
#define _SC_THREAD_PRIO_INHERIT 83
#define _SC_THREAD_PRIO_PROTECT 84
#define _SC_THREAD_PRIORITY_SCHEDULING 85
#define _SC_THREAD_PROCESS_SHARED 86
#define _SC_THREAD_ROBUST_PRIO_INHERIT 87
#define _SC_THREAD_ROBUST_PRIO_PROTECT 88
#define _SC_THREAD_SAFE_FUNCTIONS 89
#define _SC_THREAD_SPORADIC_SERVER 90
#define _SC_THREAD_STACK_MIN 91
#define _SC_THREAD_THREADS_MAX 92
#define _SC_THREADS 93
#define _SC_TIMEOUTS 94
#define _SC_TIMER_MAX 95
#define _SC_TIMERS 96
#define _SC_TRACE 97
#define _SC_TRACE_EVENT_FILTER 98
#define _SC_TRACE_EVENT_NAME_MAX 99
#define _SC_TRACE_INHERIT 100
#define _SC_TRACE_LOG 101
#define _SC_TRACE_NAME_MAX 102
#define _SC_TRACE_SYS_MAX 103
#define _SC_TRACE_USER_EVENT_MAX 104
#define _SC_TTY_NAME_MAX 105
#define _SC_TYPED_MEMORY_OBJECTS 106
#define _SC_TZNAME_MAX 107
#define _SC_V7_ILP32_OFF32 108
#define _SC_V7_ILP32_OFFBIG 109
#define _SC_V7_LP64_OFF64 110
#define _SC_V7_LPBIG_OFFBIG 111
#define _SC_V6_ILP32_OFF32 112
#define _SC_V6_ILP32_OFFBIG 113
#define _SC_V6_LP64_OFF64 114
#define _SC_V6_LPBIG_OFFBIG 115
#define _SC_VERSION 116
#define _SC_XOPEN_CRYPT 117
#define _SC_XOPEN_ENH_I18N 118
#define _SC_XOPEN_REALTIME 119
#define _SC_XOPEN_REALTIME_THREADS 120
#define _SC_XOPEN_SHM 121
#define _SC_XOPEN_STREAMS 122
#define _SC_XOPEN_UNIX 123
#define _SC_XOPEN_UUCP 124
#define _SC_XOPEN_VERSION 125

#define STDOUT_FILENO 0
#define STDIN_FILENO 1
#define STDERR_FILENO 2

#define _POSIX_VDISABLE 1

// Нам потребуется размер слова для определения `intptr_t`.
#include <etc/word.h>
#if WORDSZ == 64
typedef signed long intptr_t;
#else
typedef signed int  intptr_t;
#endif

extern char *optorg;
extern int opterr, optind, optopt;

int access(const char*, mode_t);

FILE* fdopen(int, const char*);

unsigned int alarm(unsigned int);

int chdir(const char*);

int chown(const char*, uid_t, gid_t);

int close(int);

size_t confstr(int, char*, size_t);

char* crypt(const char*, const char*);

int dup(int);

int dup2(int, int);

void _exit(int);

void encrypt(char[64], int);

int execl(const char*, const char*, ...);

int execle(const char*, const char*, ...);

int execlp(const char*, const char*, ...);

int execv(const char*, char *const[]);

int execve(const char*, char *const[], char *const[]);

int execvp(const char*, char *const[]);

int faccessat(int, const char*, int, int);

int fchdir(int);

int fchown(int, uid_t, gid_t);

int fchownat(int, const char*, uid_t, gid_t, int);

int fdatasync(int);

int fexecve(int, char *const[], char *const[]);

pid_t fork();

long fpathconf(int, int);

int fsync(int);

int ftruncate(int, off_t);

char* getcwd(char*, size_t);

gid_t getegid();

uid_t geteuid();

gid_t getgid();

int getgroups(int, gid_t[]);

long gethostid();

int gethostname(char*, size_t);

char* getlogin();

int getlogin_r();

int getopt(int, char *const[], const char*);

pid_t getpgid(pid_t);

gid_t getpgrp();

pid_t getpid();

pid_t getppid();

pid_t getsid(pid_t);

uid_t getuid();

int isatty(int);

int lchown(const char*, uid_t, gid_t);

int link(const char*, const char*);

int linkat(int, const char*, int, const char*, int);

int lockf(int, int, off_t);

off_t lseek(int, off_t, int);

int nice(int);

long pathconf(const char*, int);

int pause();

int pipe(int[2]);

ssize_t pread(int, void*, size_t, off_t);

ssize_t pwrite(int, const void*, size_t, off_t);

ssize_t read(int, void*, size_t);

ssize_t readlink(const char *restrict, char *restrict, size_t);

ssize_t readlinkat(int, const char *restrict, char *restrict, size_t);

int rmdir(const char*);

int setegid(gid_t);

int seteuid(uid_t);

int setgid(gid_t);

int setpgid(pid_t, pid_t);

pid_t setpgrp();

int setregid(gid_t, gid_t);

int setreuid(uid_t, uid_t);

pid_t setsid();

int setuid(uid_t);

unsigned int sleep(unsigned int);

void swab(const void *restrict, void *restrict, ssize_t);

int symlink(const char*, const char*);

int symlinkat(const char*, int, const char*);

void sync();

long sysconf(int);

pid_t tcgetpgrp(int);

int truncate(const char*, int);

char* ttyname(int);

int ttyname_r(int, char*, size_t);

int unlink(const char*);

int unlinkat(int, const char*, int);

ssize_t write(int, const void*, size_t);

END_DECLS

#endif