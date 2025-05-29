//
//  stdalign.h - Ровнение типов
//
//  Позволяет вручную выравнивать типы данных.
//

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201100L && __STDC_VERSION__ < 202300L

// Устанавливает выравнивание типа данных как `align`.
#define alignas(align) __attribute__((aligned(align)))
// Устанавливает выравнивание типа данных таким же, как и у типа `type`.
#define alignof(type) __alignof__(type)

// Означает, что макрос alignas определен.
#define __alignas_is_defined 1
// Означает, что макрос alignof определен.
#define __alignof_is_defined 1

#endif