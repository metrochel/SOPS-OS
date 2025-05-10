//
//	types.h - Заголовок с типами
//
//	- Содержит типы. А вы что думали?
//

// Если вспомогательные заголовки __need_X не определены,
// то заголовок включает программа, а значит, ей нужно всё.
#if    !defined(_TYPES_INCL) && !defined(__need_pid_t)
#define _TYPES_INCL 1
#define __need_pid_t
#include <../etc/decl.h>

BEGIN_DECLS

//=====================================
#if defined(__need_pid_t) && !defined(_PID_T_INCL)
#define _PID_T_INCL
// `pid_t` - это тип, определяющий PID.
typedef unsigned short pid_t;
#undef __need_pid_t
#endif

END_DECLS

#endif