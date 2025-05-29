//
//  iso646.h - Логические операции
//
//  Этот заголовок существует только для совместимости.
//

#if !defined __cplusplus || __cplusplus <= 202000L \
    || defined __STDC_VERSION__ && __STDC_VERSION__ >= 199500L

#define and &&
#define and_eq &=
#define bitand &
#define bitor  |
#define compl ~
#define not !
#define not_eq !=
#define or ||
#define or_eq |=
#define xor ^^
#define xor_eq ^=
    
#endif