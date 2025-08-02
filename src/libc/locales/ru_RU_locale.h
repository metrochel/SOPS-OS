/*
 * ru_RU_locale.h
 *
 * Локаль "ru_RU.utf8" (русский, Россия, кодировка UTF-8)
 */

#ifndef _RU_RU_LOCALE
#define _RU_RU_LOCALE

#include "ext_lconv.h"

const num_lconv RU_RU_NUMBER = {
        ",",
        " ",
        "\003"
};

const mon_lconv RU_RU_MONETARY = {
        ",",
        " ",
        "\003",
        "",
        "-",
        "₽",
        2,
        0,
        0,
        2,
        2,
        1,
        1,
        "RUB",
        2,
        0,
        0,
        2,
        2,
        1,
        1
};

const dat_lconv RU_RU_DATE = {
        "%W %DD.%MM.%YYYY %hh:%mm:%ss",
        "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс",
        "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье",
        "Янв", "Фев", "Мар", "Апр", "Май", "Июн", "Июл", "Авг", "Сен", "Окт", "Ноя", "Дек",
        "Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь",
        "Декабрь"
};

const ctp_lconv RU_RU_CTYPE = {
        ctp_str("абвгдеёжзийклмнопрстуфхцчшщъыьэюя"),
        ctp_str("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"),
        ctp_str(NULL),
        ctp_str(NULL),
        ctp_str(NULL),
        ctp_str(NULL)
};

char *ru_RU_order[] = {"а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й", "к", "л", "м", "н", "о", "п", "р",
                       "с", "т", "у", "ф", "х", "ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я", (char*)0};

const col_lconv RU_RU_COL = {
        ru_RU_order
};

const ext_lconv RU_RU_LOCALE = {
        RU_RU_NUMBER,
        RU_RU_MONETARY,
        RU_RU_DATE,
        RU_RU_CTYPE,
        RU_RU_COL
};

#endif