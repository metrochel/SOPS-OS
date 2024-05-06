;
;   Загрузчик 2 стадии
;
;   - Готовит систему к загрузке ядра.
;
org 0x6000

%define NEWL 0x0A
%define CR   0x0D

start:
    jmp main

a20_error db "Oshibka vkluchenia adresnoi linii A20!", NEWL, 0
a20_success db "Adresnaya liniya A20 uspeshno vkluchena!", NEWL, 0

no_cpuid db "Instruktsiya CPUID nedostupna!", NEWL, 0

printstr:
	mov ah, 0x0E
	mov bx, 0
.loop:
	lodsb
    cmp al, NEWL
    je .newl
	or al, al
	jz .out
	int 10h
	jmp .loop
.out:
	ret
.newl:
    mov al, CR
    int 10h
    mov al, NEWL
    int 10h
    jmp .loop

;
;  Активация линии А20
;
;  - Без неё не получится использовать всю ОЗУ в ПК.
;
enable_a20:
    cli
    call .check_a20
    jc .a20_on
    call .enable_a20
    call .check_a20
    jc .a20_on
    sti
    jnc .a20_fail
    ret
; Активация линии А20
.enable_a20:
    ; Для включения используем контроллер клавиатуры.
    ; Команда отключения клавиатуры
    call .a20_wait1
    mov al, 0xAD
    out 0x64, al
    ; Команда чтения с контроллера
    call .a20_wait1
    mov al, 0xD0
    out 0x64, al
    ; Непосредственно чтение
    call .a20_wait2
    in al, 0x60
    push ax
    ; Команда записи на контроллер
    call .a20_wait1
    mov al, 0xD1
    out 0x64, al
    ; Непосредственно запись
    call .a20_wait1
    pop ax
    or al, 2
    out 0x60, al
    ; Обратное включение клавиатуры
    call .a20_wait1
    mov al, 0xAE
    out 0x64, al
    ; Всё!
    call .a20_wait1
    sti
    ret
.a20_wait1:
    in al, 0x64
    test al, 2
    jnz .a20_wait1
    ret
.a20_wait2:
    in al, 0x64
    test al, 1
    jz .a20_wait2
    ret

; Проверка линии А20
.check_a20:
    pusha
    ; Установка DS:SI на адрес 0х0000:0x7DFE
    mov ax, 0x0000
    mov ds, ax
    mov si, 0x7DFE
    ; Установка ES:DI на адрес 0хFFFF:0x7E0E (1 МиБ выше DS:SI)
    mov ax, 0xFFFF
    mov es, ax
    mov di, 0x7E0E
    ; Сравниваем два адреса
    mov ah, byte [ds:si]
    mov bh, byte [es:di]
    cmp ah, bh
    jz .a20_check_fail1
    ; Если различны, то А20 включена
    stc
    popa
    ret
.a20_check_fail1:
    ; Если нет, то пробуем изменить значения
    mov byte [ds:si], 0x00
    ; и снова сравниваем
    mov ah, byte [ds:si]
    mov bh, byte [es:di]
    cmp ah, bh
    jz .a20_check_fail2
    ; Если различны, то А20 включена
    stc
    popa
    ret
.a20_check_fail2:
    ; Если всё равно совпали, то А20 выключена
    clc
    popa
    ret

; A20 включена
.a20_on:
    mov si, a20_success
    call printstr
    ret

; A20 не включена
.a20_fail:
    mov si, a20_error
    call printstr
    ret

;
;   Получение информации о ЦП
;  
;   - Достаёт возможности ЦП, такие, как SSE, например.
;
getcpuinfo:
    call .check_cpuid
    jz .no_cpuid
    call .cpuid_test
    ret

; Проверка доступности инструкции CPUID
.check_cpuid:
    pushfd
    pushfd
    xor dword [esp], 0x00200000
    popfd
    pushfd
    pop eax
    xor eax, [esp]
    popfd
    and eax, 0x00200000
    or eax, eax
    ret

; Проверка работы CPUID
.cpuid_test:
    mov eax, 0
    cpuid
    mov di, 0x5000
    mov eax, ebx
    stosd
    mov eax, edx
    stosd
    mov eax, ecx
    stosd
    mov si, di
    call printstr
    ret
; Инструкция CPUID недоступна
.no_cpuid:
    mov si, no_cpuid
    call printstr
    ret

getvideomodes:
    ret

getram:
    ret

enable_prot_mode:
    ret

main:
    ; Активация адресной линии А20
    call enable_a20

    ; Получение данных о процессоре
    call getcpuinfo

    ; Получение доступных видеорежимов
    call getvideomodes

    ; Получение разметки памяти
    call getram

    ; Активация защищённого 32-битного режима
    call enable_prot_mode

    ; TODO: Загрузка ядра
    hlt