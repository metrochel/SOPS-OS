/*
 * cpuflags.hpp - Манипуляция флагами ЦП
 *
 * Позволяет управлять флагами ЦП.
 */

#ifndef _CPUFLAGS_INCL
#define _CPUFLAGS_INCL

struct cpu_flags {
    word carry          : 1;
    word __reserved1    : 1;
    word parity         : 1;
    word __reserved2    : 1;
    word aux_carry      : 1;
    word __reserved3    : 1;
    word zero           : 1;
    word sign           : 1;
    word trap           : 1;
    word interrupt      : 1;
    word direction      : 1;
    word overflow       : 1;
    word io_priv_lvl    : 1;
    word nested_task    : 1;
    word __reserved4    : 1;
} __attribute__((packed));

struct cpu_eflags {
    cpu_flags flags;
    word resume             : 1;
    word v8086              : 1;
    word align_check        : 1;
    word virt_int           : 1;
    word virt_int_pending   : 1;
    word id_flag            : 1;
    word __reserved         : 10;
};

cpu_flags get_flags() {
    union {
        cpu_flags flags;
        word num;
    } uflags;

    __asm__ volatile (
        "pushf; pop %d0"
        :   "=m"(uflags.num)
        :
        :
    );

    return uflags.flags;
}

inline bool overflow_flag() {
    return get_flags().overflow;
}

#endif //_CPUFLAGS_INCL
