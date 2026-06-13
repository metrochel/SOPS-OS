/*
 * Функции поддержки C++ - Libgcc
 *
 * Здесь реализованы некоторые функции libgcc, без которых ОС не собирается.
 *
 */

#include "../libk/nums.hpp"

/// Определение __udivmoddi4 взял отсюда:
/// https://dox.ipxe.org/____udivmoddi4_8c.html
extern "C" {
__attribute__((cdecl)) qword __udivmoddi4(qword num, qword den, qword *rem_ptr) {
    qword quot = 0, qbit = 1;

    if (den == 0) {
        /// Здесь происходит намеренное деление на 0, но такое,
        /// что не предотвратит компиляцию программы.
        return 1 / ((qword) den);
    }

    /// Left-justify denominator and count shift (это я не знаю зачем, так в источнике написано)
    while ((long long) den >= 0) {
        den <<= 1;
        qbit <<= 1;
    }

    /// Собственно само деление.
    /// Реализовано каким-то быстрым лебедевским алгоритмом ( кто понял, тот понял (: )
    while (qbit) {
        if (den <= num) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }

    /// Записываем остаток, если такой есть.
    if (rem_ptr)
        *rem_ptr = num;

    return quot;
}
}