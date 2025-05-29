//
//  assert.h - Макрос assert
//
//  Предоставляет только макрос assert.
//

#define __need_abort

#ifdef NDEBUG
    #define assert(X) (void(0))
#else
    #define assert(X) if (!(X)) __assert_fail(#X, __FILE__, __LINE__, __func__);
#endif

// В случае провала `assert` выводит сообщение о провале и экстренно завершает программу.
void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *func);