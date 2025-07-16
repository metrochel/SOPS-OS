/*
 * locale.c
 *
 *
 */

#include "include/locale.h"
#include "include/string.h"
#include "include/stddef.h"

#include "locales/c_locale.c"
#include "locales/ru_RU_locale.c"

#define SYS_DEFAULT_LOCALE RU_RU_LOCALE
#define SYS_DEFAULT_LOCALE_NAME "ru_RU.UTF-8"

ext_lconv locale;
const char *cur_locale;

char *setlocale(int category, const char *locale_name) {
    ext_lconv new_locale;
    char *set_locale;
    if (locale_name == NULL) {
        return (char*)cur_locale;
    }
    if (strcmp(locale_name, "C")) {
        new_locale = C_LOCALE;
        set_locale = "C";
    }
    else if (strcmp(locale_name, "ru_RU.UTF-8") || strcmp(locale_name, "ru_RU.utf8")) {
        new_locale = RU_RU_LOCALE;
        set_locale = (char*)locale_name;
    }
    else if (*locale_name == 0) {
        new_locale = SYS_DEFAULT_LOCALE;
        set_locale = SYS_DEFAULT_LOCALE_NAME;
    }
    else
        return NULL;

    if (category == LC_ALL) {
        locale = new_locale;
        cur_locale = set_locale;
        return set_locale;
    }

    if (category == LC_NUMERIC) {
        locale.nums = new_locale.nums;
        cur_locale = set_locale;
        return set_locale;
    }

    if (category == LC_MONETARY) {
        locale.money = new_locale.money;
        cur_locale = set_locale;
        return set_locale;
    }

    if (category == LC_TIME) {
        locale.dates = new_locale.dates;
        cur_locale = set_locale;
        return set_locale;
    }

    if (category == LC_COLLATE) {
        locale.coll = new_locale.coll;
        cur_locale = set_locale;
        return set_locale;
    }

    if (category == LC_CTYPE) {
        locale.ctype = new_locale.ctype;
        cur_locale = set_locale;
        return set_locale;
    }

    return NULL;
}