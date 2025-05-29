//
//  stdatomic.h - Атомарные операции
//
//  Определяет типы, макросы и функции для работы с атомарными операциями.
//

#if !defined (_STDATOMIC_INCL) && defined __STDC_VERSION__ && __STDC_VERSION__ >= 201100L
#define _STDATOMIC_INCL 1

#include <etc/decl.h>

// Показывает, что атомарный тип не блокирующий:
// 0 - всегда блокирующий
// 1 - иногда блокирующий
// 2 - никогда не блокирующий

#define ATOMIC_BOOL_LOCK_FREE 2
#define ATOMIC_CHAR_LOCK_FREE 2
#define ATOMIC_CHAR16_T_LOCK_FREE 2
#define ATOMIC_CHAR32_T_LOCK_FREE 2
#define ATOMIC_WCHAR_T_LOCK_FREE 2
#define ATOMIC_SHORT_LOCK_FREE 2
#define ATOMIC_INT_LOCK_FREE 2
#define ATOMIC_LONG_LOCK_FREE 2
#define ATOMIC_LLONG_LOCK_FREE 2
#define ATOMIC_POINTER_LOCK_FREE 2

/* Порядки сохранения атомарных величин в памяти */

#define MEM_ORD_RELAXED     1
#define MEM_ORD_CONSUME     2
#define MEM_ORD_ACQUIRE     3
#define MEM_ORD_RELEASE     4
#define MEM_ORD_ACQ_REL     5
#define MEM_ORD_SEQ_CST     6

typedef enum {
    
} memory_order;

// К поносу макросов приготовиться!

BEGIN_DECLS

// Сохраняет значение `val` в атомарном объекте `obj` с использованием порядка операции `order`.
#define atomic_store_explicit(obj, val, order) \
    __extension__ ({ \
        __atomic_store(obj, &val, order); \
    })

    
// Сохраняет значение `val` в атомарном объекте `obj`.
#define atomic_store(obj, val) \
    atomic_store_explicit(obj, val, MEM_ORD_SEQ_CST)

    
// Загружает значение из атомарного объекта `obj` с использованием заданного порядка операции `order`.
#define atomic_load_explicit(obj, order) \
    __extension__ ({ \
        typeof(*(obj)) __val; \
        __atomic_load(obj, &__val, order); \
        __val; \
    })

    
// Загружает значение из атомарного объекта `obj`.
#define atomic_load(obj) \
    atomic_load_explicit(obj, MEM_ORD_SEQ_CST)

    
// Обменивает значение атомарного объекта `obj` с заданным значением `val` с использованием заданного порядка операции `order`.
#define atomic_exchange_explicit(obj, val, order) \
    __extension__ ({ \
        typeof(*(obj)) __tmp; \
        __atomic_exchange(obj, &val, &__tmp, order); \
        __tmp; \
    })

    
// Обменивает значение атомарного объекта `obj` с заданным значением `val`.
#define atomic_exchange(obj, val) \
    atomic_exchange_explicit(obj, val, MEM_ORD_SEQ_CST)

    
// Сравнивает значение атомарного объекта `obj` с заданным значением `expected` и обновляет его на `desired` с использованием заданного порядка операции `order`.
#define atomic_compare_exchange_explicit(obj, expected, desired, order) \
    __extension__ ({ \
        __atomic_compare_exchange(obj, &expected, &desired, false, order, order); \
    })

    
// Сравнивает значение атомарного объекта `obj` с заданным значением `expected` и обновляет его на `desired`.
#define atomic_compare_exchange(obj, expected, desired) \
    atomic_compare_exchange_explicit(obj, expected, desired, MEM_ORD_SEQ_CST)

    
// Сравнивает значение атомарного объекта `obj` с заданным значением `expected` и обновляет его на `desired` с использованием заданного порядка операции `order`.
#define atomic_compare_strong_explicit(obj, expected, desired, order) \
    __extension__ ({ \
        __atomic_compare_exchange(obj, &expected, &desired, false, order, order); \
    })

    
// Сравнивает значение атомарного объекта `obj` с заданным значением `expected` и обновляет его на `desired`.
#define atomic_compare_strong(obj, expected, desired) \
    atomic_compare_strong_explicit(obj, expected, desired, MEM_ORD_SEQ_CST)

    
// Сравнивает значение атомарного объекта `obj` с заданным значением `expected` и обновляет его на `desired` с использованием заданного порядка операции `order`.
#define atomic_compare_exchange_weak_explicit(obj, expected, desired, success_order, failure_order) \
    __extension__ ({ \
        __atomic_compare_exchange(obj, &expected, &desired, true, success_order, failure_order); \
    })

    
// Сравнивает значение атомарного объекта `obj` с заданным значением `expected` и обновляет его на `desired`.
#define atomic_compare_exchange_weak(obj, expected, desired) \
    atomic_compare_exchange_weak_explicit(obj, expected, desired, MEM_ORD_SEQ_CST, MEM_ORD_SEQ_CST)

    
// Складывает значение атомарного объекта `obj` с заданным значением `value` с использованием заданного порядка операции `order`.
#define atomic_fetch_add_explicit(obj, value, order) \
    __extension__ ({ \
        __atomic_fetch_add(obj, value, order); \
    })

    
// Складывает значение атомарного объекта `obj` с заданным значением `value`.
#define atomic_fetch_add(obj, value) \
    atomic_fetch_add_explicit(obj, value, MEM_ORD_SEQ_CST)

    
// Вычитает значение атомарного объекта `obj` с заданным значением `value` с использованием заданного порядка операции `order`.
#define atomic_fetch_sub_explicit(obj, value, order) \
    __extension__ ({ \
        __atomic_fetch_sub(obj, value, order); \
    })

    
// Вычитает значение атомарного объекта `obj` с заданным значением `value`.
#define atomic_fetch_sub(obj, value) \
    atomic_fetch_sub_explicit(obj, value, MEM_ORD_SEQ_CST)

    
// Выполняет побитовое ИЛИ значения атомарного объекта `obj` с заданным значением `value` с использованием заданного порядка операции `order`.
#define atomic_fetch_or_explicit(obj, value, order) \
    __extension__ ({ \
        __atomic_fetch_or(obj, value, order); \
    })

    
// Выполняет побитовое ИЛИ значения атомарного объекта `obj` с заданным значением `value`.
#define atomic_fetch_or(obj, value) \
    atomic_fetch_or_explicit(obj, value, MEM_ORD_SEQ_CST)

    
// Выполняет побитовое И с значениями атомарного объекта `obj` и заданным значением `value` с использованием заданного порядка операции `order`.
#define atomic_fetch_and_explicit(obj, value, order) \
    __extension__ ({ \
        __atomic_fetch_and(obj, value, order); \
    })

    
// Выполняет побитовое И с значениями атомарного объекта `obj` и заданным значением `value`.
#define atomic_fetch_and(obj, value) \
    atomic_fetch_and_explicit(obj, value, MEM_ORD_SEQ_CST)
    

// Выполняет побитовое исключающее ИЛИ значения атомарного объекта `obj` с заданным значением `value` с использованием заданного порядка операции `order`.
#define atomic_fetch_xor_explicit(obj, value, order) \
    __extension__ ({ \
        __atomic_fetch_xor(obj, value, order); \
    })

    
// Выполняет побитовое исключающее ИЛИ значения атомарного объекта `obj` с заданным значением `value`.
#define atomic_fetch_xor(obj, value) \
    atomic_fetch_xor_explicit(obj, value, MEM_ORD_SEQ_CST)

    
// `atomic_flag` - это тип, схожий с `atomic_bool`, но никогда не блокирующий.
typedef _Atomic struct {
    _Bool value;        // Значение флага
} atomic_flag;


// Очищает атомарный флаг `obj` с использованием заданного порядка операции `order`.
#define atomic_flag_clear_explicit(obj, order) \
    __extension__ ({ \
        __atomic_clear(obj, order); \
    })


// Очищает атомарный флаг `obj`.
#define atomic_flag_clear(obj) \
    atomic_flag_clear_explicit(obj, MEM_ORD_SEQ_CST)
    
    
// Устанавливает атомарный флаг `obj` с использованием заданного порядка операции `order`.
#define atomic_flag_set_explicit(obj, order) \
    __extension__ ({ \
        __atomic_test_and_set(obj, order); \
    })


// Устанавливает атомарный флаг `obj`.
#define atomic_flag_set(obj) \
    atomic_flag_set_explicit(obj, MEM_ORD_SEQ_CST)
    

// Устанавливает атомарный флаг `obj` с использованием заданного порядка операции `order` и возвращает его предыдущее значение.
#define atomic_flag_test_and_set_explicit(obj, order) \
    __extension__ ({ \
        __atomic_test_and_set(obj, order); \
    })


// Устанавливает атомарный флаг `obj` и возвращает его предыдущее значение.
#define atomic_flag_test_and_set(obj) \
    atomic_flag_test_and_set_explicit(obj, MEM_ORD_SEQ_CST)


// Инициализирует атомарный объект `obj` значением `val`.
#define atomic_init(obj, val) \
    __extension__ ({ \
        __atomic_store(obj, val, MEM_ORD_RELAXED); \
    })


// Этот макрос был удалён в C23, но не в C++.
#if defined __STDC_VERSION__ && __STDC_VERSION__ <= 202300L || defined __cplusplus

// Забавно, но факт: glibc (libc от GNU, та, что на линуксе) использует ровно такое же определение.
// Те ребята наверняка знали, что они делали, поэтому я определю так же.

// Инициализирует новый атомарный объект.
#define ATOMIC_VAR_INIT(value) (value)

#endif

// Вот тут glibc использует "{ 0 }". А я выделюсь.

// Инициализирует новый атомарный флаг.
#define ATOMIC_FLAG_INIT(value) (value)


// Останавливает цепочку зависимостей на `dep` и возвращает `dep`.
#define kill_dependency(dep) \
    __extension__ ({ \
        dep;         \
    })

// FIXME: Разобраться, что это такое
#define atomic_thread_fence(order) \
    __atomic_thread_fence(order)

// FIXME: Разобраться, что это такое тоже
#define atomic_signal_fence(order) \
    __atomic_signal_fence(order)

/* Атомарные типы */

typedef _Atomic _Bool                   atomic_bool;
typedef _Atomic char                    atomic_char;
typedef _Atomic signed char             atomic_schar;
typedef _Atomic unsigned char           atomic_uchar;
typedef _Atomic short                   atomic_short;
typedef _Atomic signed short            atomic_sshort;
typedef _Atomic unsigned short          atomic_ushort;
typedef _Atomic int                     atomic_int;
typedef _Atomic unsigned int            atomic_uint;
typedef _Atomic long                    atomic_long;
typedef _Atomic unsigned long           atomic_ulong;
typedef _Atomic long long               atomic_llong;
typedef _Atomic unsigned long long      atomic_ullong;

#ifdef __CHAR8_TYPE__
typedef _Atomic __CHAR8_TYPE__          atomic_char8_t;
#endif
typedef _Atomic __CHAR16_TYPE__         atomic_char16_t;
typedef _Atomic __CHAR32_TYPE__         atomic_char32_t;
typedef _Atomic __WCHAR_TYPE__          atomic_wchar_t;

typedef _Atomic __INT_LEAST8_TYPE__     atomic_int_least8_t;
typedef _Atomic __INT_FAST8_TYPE__      atomic_int_fast8_t;

typedef _Atomic __INT_LEAST16_TYPE__    atomic_int_least16_t;
typedef _Atomic __INT_FAST16_TYPE__     atomic_int_fast16_t;

typedef _Atomic __INT_LEAST32_TYPE__    atomic_int_least32_t;
typedef _Atomic __INT_FAST32_TYPE__     atomic_int_fast32_t;

typedef _Atomic __INT_LEAST64_TYPE__    atomic_int_least64_t;
typedef _Atomic __INT_FAST64_TYPE__     atomic_int_fast64_t;

typedef _Atomic __INTPTR_TYPE__         atomic_intptr_t;
typedef _Atomic __UINTPTR_TYPE__        atomic_uintptr_t;
typedef _Atomic __SIZE_TYPE__           atomic_size_t;
typedef _Atomic __PTRDIFF_TYPE__        atomic_ptrdiff_t;
typedef _Atomic __INTMAX_TYPE__         atomic_intmax_t;
typedef _Atomic __UINTMAX_TYPE__        atomic_uintmax_t;

END_DECLS

#endif