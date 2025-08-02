/*
 * c_locale.c
 *
 * Локаль "C" (встроенная локаль)
 */

#ifndef _C_LOCALE
#define _C_LOCALE

#include "../include/limits.h"
#include "ext_lconv.h"

const num_lconv C_NUM = {
        ".",
        "",
        ""
};

const mon_lconv C_MONEY = {
    "",
    "",
    "",
    "",
    "",
    "",
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    "",
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX
};

const dat_lconv C_DATES = {
        "%a %b %e %H:%M:%S  %Y",
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun",
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday",
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
        "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November",
            "December"
};

const ctp_lconv C_CTYPE = {
        ctp_str(NULL),
        ctp_str(NULL),
        ctp_str(NULL),
        ctp_str(NULL),
        ctp_str(NULL),
        ctp_str(NULL)
};

char *c_letters[] = {};

const col_lconv C_COLLATE = {
        c_letters
};

const ext_lconv C_LOCALE = {
    C_NUM,
    C_MONEY,
    C_DATES,
    C_CTYPE,
    C_COLLATE
};

#endif