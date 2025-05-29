//
//  stdnoreturn.h - Макрос невозврата
//
//  Объявляет макрос noreturn, показывающий, что функция не доходит
//  до своей точки возврата.
//

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L

#define noreturn _Noreturn

#endif