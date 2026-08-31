//
//	errno.h - Заголовок про ошибки
//
//	В этом заголовке можно определить тип ошибки,
//  которую сгенерировала функция.
//

#if !defined(_ERRNO_INCLUDE)
#define _ERRNO_INCLUDE 1
#include <etc/decl.h>

/* Коды ошибок */

// Ошибка области определения
#define EDOM (-1)
// Ошибка неверной последовательности
#define EILSEQ (-2)
// Ошибка пределов (обычно целочисленное переполнение)
#define ERANGE (-3)

#define E2BIG 1
#define EACCES 2
#define EADDRINUSE 3
#define EADDRNOTAVAIL 4
#define EAFNOSUPPORT 5
#define EAGAIN 6
#define EALREADY 7
#define EBADE 8
#define EBADF 9
#define EBADFD 10
#define EBADMSG 11
#define EBADR 12
#define EBADRQC 13
#define EBADSLT 14
#define EBUSY 15
#define ECANCELED 16
#define ECHILD 17
#define ECHRNG 18
#define ECOMM 19
#define ECONNABORTED 20
#define ECONNREFUSED 21
#define ECONNRESET 22
#define EDEADLK 23
#define EDEADLOCK 24
#define EDESTADDRREQ 25
#define EDQUOT 26
#define EEXIST 27
#define EFAULT 28
#define EFBIG 29
#define EHOSTDOWN 30
#define EHOSTUNREACH 31
#define EHWPOISON 32
#define EIDRM 33
#define EINPROGRESS 34
#define EINTR 35
#define EINVAL 36
#define EIO 37
#define EISCONN 38
#define EISDIR 39
#define EISNAM 40
#define EKEYEXPIRED 41
#define EKEYREJECTED 42
#define EKEYREVOKED 43
#define EL2HLT 44
#define EL2NSYNC 45
#define EL3HLT 46
#define EL3RST 47
#define ELIBACC 48
#define ELIBBAD 49
#define ELIBMAX 50
#define ELIBSCN 51
#define ELIBEXEC 52
#define ELNRNG 53
#define ELOOP 54
#define EMEDIUMTYPE 55
#define EMFILE 56
#define EMLINK 57
#define EMSGSIZE 58
#define EMULTIHOP 59
#define ENAMETOOLONG 60
#define ENETDOWN 61
#define ENETRESET 62
#define ENETUNREACH 63
#define ENFILE 64
#define ENOANO 65
#define ENOBUFS 66
#define ENODATA 67
#define ENODEV 68
#define ENOENT 69
#define ENOEXEC 70
#define ENOKEY 71
#define ENOLCK 72
#define ENOLINK 73
#define ENOMEDIUM 74
#define ENOMEM 75
#define ENOMSG 76
#define ENONET 77
#define ENOPKG 78
#define ENOPROTOOPT 79
#define ENOSPC 80
#define ENOSR 81
#define ENOSTR 82
#define ENOSYS 83
#define ENOTBLK 84
#define ENOTCONN 85
#define ENOTDIR 86
#define ENOTEMPTY 87
#define ENOTRECOVERABLE 88
#define ENOTSOCK 89
#define ENOTSUP 90
#define ENOTTY 91
#define ENOTUNIQ 92
#define ENXIO 93
#define EOPNOTSUPP 94
#define EOVERFLOW 95
#define EOWNERDEAD 96
#define EPERM 97
#define EPFNOSUPPORT 98
#define EPIPE 99
#define EPROTO 100
#define EPROTONOSUPPORT 101
#define EPROTOTYPE 102
#define EREMCHG 103
#define EREMOTE 104
#define EREMOTEIO 105
#define ERESTART 106
#define ERFKILL 107
#define EROFS 108
#define ESHUTDOWN 109
#define ESPIPE 110
#define ESOCKTNOSUPPORT 111
#define ESRCH 112
#define ESTALE 113
#define ESTRPIPE 114
#define ETIME 115
#define ETIMEDOUT 116
#define ETOOMANYREFS 117
#define ETXTBSY 118
#define EUCLEAN 119
#define EUNATCH 120
#define EUSERS 121
#define EWOULDBLOCK 122
#define EXDEV 123
#define EXFULL 124

// Код последней возникшей ошибки.
// Если 0, то всё хорошо.
#define errno __errno

BEGIN_DECLS

// Чшшш! Это моё! Не подсматривать!
extern int __errno;

END_DECLS

#endif