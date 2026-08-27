/*
 * cpu_exceptions.cpp - Обработчики исключений ЦП
 *
 */

#include "cpu_exceptions.hpp"

#include "libk/objs.hpp"
#include "graphics/graphics.hpp"
#include "dbg/dbgutil.hpp"

NAMESPACE_BEGIN(interrupt::inline handles)

dword get_cpu_err_code() {
    dword code;

    __asm__ volatile (
        "mov %d0, dword [ebp + 4];"
        "mov edx, dword [ebp + 8];"
        "mov dword [ebp + 4], edx;"
        :   "=a"(code)
        :
        :   "edx"
    );

    return code;
}

struct pf_error_code {
    dword present           : 1;
    dword write             : 1;
    dword user              : 1;
    dword res_write         : 1;
    dword instruction_fetch : 1;
    dword prot_key          : 1;
    dword shadow_stack      : 1;
    dword __reserved1       : 8;
    dword soft_guard_ex     : 1;
    dword __reserved2       : 15;
};

isr(div_error) {
    cerr << "\nОШИБКА: Деление на ноль\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(debug_error) {
    cerr << "\nОШИБКА: Отладочное исключение\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(nmi_handle) {
    cerr << "\nОШИБКА: Немаскируемое исключение\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(breakpoint_handle) {
    cwrn << "\nВНИМАНИЕ: Точка останова\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(overflow_error) {
    cerr << "\nОШИБКА: Переполнение\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(bound_error) {
    cerr << "\nОШИБКА: Выход за пределы ограничений\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(ud_opcode_error) {
    cerr << "\nОШИБКА: Невозможная инструкция\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(dev_not_available_error) {
    cerr << "\nОШИБКА: Сопроцессор x87 недоступен\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(double_fault) {
    cerr << "\nОШИБКА: Двойной сбой\n";
    cwrn << "ВНИМАНИЕ: Возврат невозможен!\n";

    dword error_code = get_cpu_err_code();

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(coproc_seg_overrun) {
    cerr << "\nОШИБКА: Переполнение сегмента сопроцессора\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(invalid_tss_handle) {
    cerr << "\nОШИБКА: Невозможный селектор TSS\n";

    dword error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << error_code << "\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(seg_not_present_error) {
    cerr << "\nОШИБКА: Сегмент недоступен\n";

    dword error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << error_code << "\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(stack_seg_fault) {
    cerr << "\nОШИБКА: Сбой сегмента стека\n";

    dword error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << error_code << "\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(gpf_handle) {
    cerr << "\nОШИБКА: Общий сбой защиты\n";

    dword error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << error_code << "\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(pf_handle) {
    debug::magic_breakpoint();
    cerr << "\nОШИБКА: Страничный сбой\n";

    dword dw_error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << dw_error_code << "\n";

    pf_error_code error_code = *(pf_error_code*)dw_error_code;
    cerr << "Ошибка вызвана " << (error_code.present ? "сбоем защиты\n" : "несуществующей страницей\n");
    cerr << "Ошибка вызвана попыткой " << (error_code.write ? "записи\n" : "чтения\n");
    cerr << "Ошибка вызвана при CPL = " << (error_code.user ? "3\n" : "0\n");

    void *address;
    __asm__ volatile (
        "mov eax, cr2; mov %d0, eax;"
        :   "=m"(address)
        :
        :   "eax"
    );
    cerr << "Адрес сбоя: " << address << "\n";

    debug::print_stack_trace();
}

isr(x87_fpu_error) {
    cerr << "\nОШИБКА: Сбой сопроцессора x87\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(align_check_error) {
    cerr << "\nОШИБКА: Сбой ровнения\n";

    dword error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << error_code << "\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(machine_check_error) {
    cerr << "\nОШИБКА: Машинный сбой\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(simd_fp_error) {
    cerr << "\nОШИБКА: Сбой инструкции SIMD\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(virt_error) {
    cerr << "\nОШИБКА: Сбой виртуализации\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

isr(control_prot_error) {
    cerr << "\nОШИБКА: Сбой защиты управления\n";

    dword error_code = get_cpu_err_code();
    cerr << "Код ошибки: " << intmod::bin << error_code << "\n";

    debug::print_stack_trace();

    debug::magic_breakpoint();
}

NAMESPACE_END()