/*
 * cpu_exceptions.hpp - Обработчики исключений ЦП
 *
 * Объявляет обработчики для аппаратных исключений, вызываемых ЦП.
 */

#ifndef _CPU_EXCEPTIONS_INCL
#define _CPU_EXCEPTIONS_INCL

#include "../macros.hpp"
#include "../typedefs.hpp"

namespace interrupt::inline handles {
    isr(div_error);

    isr(debug_error);

    isr(nmi_handle);

    isr(breakpoint_handle);

    isr(overflow_error);

    isr(bound_error);

    isr(ud_opcode_error);

    isr(dev_not_available_error);

    isr(double_fault);

    isr(coproc_seg_overrun);

    isr(invalid_tss_handle);

    isr(seg_not_present_error);

    isr(stack_seg_fault);

    isr(gpf_handle);

    isr(pf_handle);

    isr(x87_fpu_error);

    isr(align_check_error);

    isr(machine_check_error);

    isr(simd_fp_error);

    isr(virt_error);

    isr(control_prot_error);
}


#endif //_CPU_EXCEPTIONS_INCL
