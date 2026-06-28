/*
 * globcon.cpp - Поддержка глобальных конструкторов
 */

#define MAX_DTORS 128

extern "C" {
    struct __cxa_dtor {
        void (*dtor)(void*);
        void *arg;
        void *dso;
    };

    /// Зарегистрированные деструкторы
    __cxa_dtor __cxa_dtors[MAX_DTORS];
    /// Количество зарегистрированных деструкторов
    int __active_dtors;

    void __cxa_remove_entry(int entry) {
        if (entry >= __active_dtors || entry < 0)
            return;

        for (int i = entry; i < __active_dtors - 1; i++) {
            __cxa_dtors[i] = __cxa_dtors[i + 1];
        }
        __active_dtors--;
    }

    /// @brief Регистрирует глобальный деструктор для объекта.
    /// @param dtor Указатель на деструктор
    /// @param arg Аргумент деструктора
    /// @param dso Динамический разделяемый объект™ (Dynamic Shared Object)
    /// @return 0 при успехе; не 0 при провале
    /// @note Судя по всему, DSO - это и есть разрушаемый объект.
    int __cxa_atexit(void (*dtor)(void*), void *arg, void *dso) {
        // Если достигнут предел по деструкторам...
        if (__active_dtors >= MAX_DTORS)
            // то выходим и говорим, что деструктор не зареган.
            return -1;

        // Иначе спокойно регистрируем деструктор и возвращаем 0.
        __cxa_dtors[__active_dtors++] = { dtor, arg, dso };
        return 0;
    }

    /// @brief Вызывает деструкторы глобальных объектов.
    /// @param dtor Деструкторы, которые стоит вызвать
    /// @note Если @c dtor @c == @c 0, то вызываются @b все деструкторы.
    void __cxa_finalize(void *dtor) {
        for (int i = 0; i < __active_dtors; i++) {
            __cxa_dtor& i_dtor = __cxa_dtors[i];
            if (!dtor || (i_dtor.dtor && i_dtor.dtor == dtor)) {
                i_dtor.dtor(i_dtor.arg);
                __cxa_remove_entry(i);
                i--;
            }
        }
    }
}