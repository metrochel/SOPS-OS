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
; Имя такое странное, потому что нам нужно,
; чтобы C++-код мог состыковаться с нашим обработчиком.
hdl_name :
    %define handle  dword [ebp - 4]
    %define pid     dword [ebp - 8]
    %define scall   dword [ebp - 12]

    push ebp
    mov ebp, esp

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
    sub esp, 12

    ; Теперь начнём обрабатывать системный вызов.
    ; Начнём с получения обработчика.

    ; В соответствии со стандартной схемой вызова нам нужно поместить
    ; номер вызова на стек, т.е. EAX.
    mov scall, eax
    push eax

    ; Теперь вызываем get_syscall_handle.
    call get_syscall_handle
    mov handle, eax
    add esp, 4

    ; Теперь нужно получить PID вызвавшего процесса. Для этого
    ; мы получим адрес вызова, а для него вызовем determine_pid.
    push dword [ebp + 4]
    call determine_pid
    mov pid, eax
    add esp, 4

    ; Напишем небольшое отладочное сообщение о том, что совершается
    ; системный вызов.
    push scall
    push pid
    push __debug_str
    call kdebug

    ; Мы получили обработчик и PID!
    ; Теперь осталось его вызвать.
    ; Помещаем аргументы на стек и вызываем наш обработчик.
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
