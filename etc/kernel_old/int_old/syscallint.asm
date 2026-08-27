;
;   syscallint.asm - Обработчик системного вызова
;
;   Процедура обработки прерывания, которая обрабатывает системный вызов.
;

[bits 32]
%define hdl_name    _Z11syscall_intP8IntFrame

%define get_syscall_handle  _Z18get_syscall_handlej
%define determine_pid       _Z13determine_pidj
%define kdebug              _Z6kdebugPKcz

extern get_syscall_handle
extern determine_pid
extern kdebug

global hdl_name

section .text

; =======================================
;
;   Обработчик прерывания системного вызова
;
;   - Принимает прерывание, сигнализирующее системный вызов,
;     и вызывает его обработчик.
;
;   На вход:
;   - EAX: номер системного вызова
;   - ESI, EDI, ECX, EDX, EBX - аргументы системного вызова
;
;   Возврат:
;   - EAX - код выхода системного вызова
;
; =======================================
hdl_name :
    %define handle  dword [ebp - 4]
    %define pid     dword [ebp - 8]
    %define scall   dword [ebp - 12]

    ; Первым делом создадим стакфрейм и включим обратно
    ; прерывания.
    push ebp
    mov ebp, esp
    sti

    ; Для начала проверим, что это не выход.
    test eax, eax
    jnz .not_exit

    ; Если выход, нам нужно вернуться в процедуру
    ; запуска программы (КОСТЫЛЬ).
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pop ebp
    add esp, 20

    pop ebp
    sti
    ret

.not_exit:
    ; Теперь начнём обрабатывать системный вызов.
    sub esp, 12
    pusha
    push eax

    ; Сначала получим PID вызвавшего процесса. Для этого
    ; мы получим адрес вызова, а для него вызовем determine_pid.
    push dword [ebp + 4]
    call determine_pid
    mov pid, eax
    add esp, 4

    ; Напишем небольшое отладочное сообщение о том, что совершается
    ; системный вызов.
    push pid
    push __debug_str
    call kdebug
    add esp, 8

    ; Теперь получим обработчик системного вызова.
    ; Для этого вызовем процедуру get_syscall_handle(EAX).
    call get_syscall_handle
    mov handle, eax
    add esp, 4

    ; Мы получили обработчик.
    ; Теперь осталось его вызвать.
    ; Помещаем аргументы на стек и вызываем наш обработчик.
    popa
    push ebx
    push edx
    push ecx
    push edi
    push esi
    push pid
    call handle

    ; Системный вызов обработан! Его возвратное значение находится
    ; в регистре EAX. Теперь можем возвращаться.
    mov esp, ebp
    pop ebp
    iret

section .rodata

__debug_str db 0x0A, "Процесс %d совершил системный вызов %x.", 0x0A, 0
