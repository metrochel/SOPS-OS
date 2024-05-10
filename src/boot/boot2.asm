;
;   Загрузчик 2 стадии
;
;   - Готовит систему к загрузке ядра.
;
org 0x7000

%define NEWL 0x0A
%define CR   0x0D

start:
    jmp main

a20_error db "Oshibka vkluchenia adresnoi linii A20!", NEWL, 0
a20_success db "Adresnaya liniya A20 uspeshno vkluchena!", NEWL, 0

no_cpuid db "Instruktsiya CPUID nedostupna!", NEWL, 0
cpuid_success db "Informatsiya o processore uspeshno poluchena!", NEWL, 0

ram_error_msg db "Oshibka sozdaniya razmetki pamyati!", NEWL, 0
ram_success_msg db "Razmetka pamyati uspeshno sdelana!", NEWL, 0

video_fail_msg db "Oshibka poluchenia videorezhimov!", NEWL, 0
video_success_msg db "Videorezhimi uspeshno polucheni!", NEWL, 0

;
;======================================
;   
;   Вывод строчки на экран
;
;   - Аналогична функции в загрузчике 1 ступени.
;
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
;============================


;======================================================
;
;   Сброс резервированной памяти
;
;   - Нужна, чтобы обнулить всю резервированную память.
;     Так проще будет читать дампы.
;
resetmem:
    push eax
    push cx
    push di
    pushf
    cld
    mov di, 0x6000
    xor eax, eax
    mov cx, 256
    rep stosd
    popf
    pop di
    pop cx
    pop eax
    ret
;======================================================


;===================================================
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
    pusha
    ; Для включения используем контроллер клавиатуры.
    ; Команда отключения клавиатуры
    call .a20_wait1
    mov al, 0xAD
    out 0x64, al
    ; Команда чтения с контроллера
    call .a20_wait1
    mov al, 0xD0
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
    popa
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
    ; Установка DS:SI на адрес 0х0000:0x61FE
    mov ax, 0x0000
    mov ds, ax
    mov si, 0x61FE
    ; Установка ES:DI на адрес 0хFFFF:0x620E (1 МиБ выше DS:SI)
    mov ax, 0xFFFF
    mov es, ax
    mov di, 0x620E
    ; Сравниваем два адреса
    mov ah, byte [ds:si]
    mov bh, byte [es:di]
    cmp ah, bh
    jz .a20_check_fail1
    ; Если различны, то А20 включена
    stc
    mov ax, 0
    mov es, ax
    popa
    ret
.a20_check_fail1:
    ; Если нет, то пробуем изменить значения
    mov byte [ds:si], 0xFF
    ; и снова сравниваем
    mov ah, byte [ds:si]
    mov bh, byte [es:di]
    cmp ah, bh
    jz .a20_check_fail2
    ; Если различны, то А20 включена
    stc
    mov ax, 0
    mov es, ax
    mov byte [ds:si], 0x00
    popa
    ret
.a20_check_fail2:
    ; Если всё равно совпали, то А20 выключена
    clc
    mov ax, 0
    mov es, ax
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
;==========================================================

;
;======================================================
;
;   Получение информации о ЦП
;  
;   - Достаёт возможности ЦП, такие, как SSE, например.
;
getcpuinfo:
    call .check_cpuid
    jz .no_cpuid
    call .cpuid_getvendor
    call .cpuid_getdata
    mov si, cpuid_success
    call printstr
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
; Получение строки вендора
.cpuid_getvendor:
    push di
    push eax
    xor eax, eax
    cpuid
    mov di, bp
    mov eax, ebx
    stosd
    mov eax, edx
    stosd
    mov eax, ecx
    stosd
    mov bp, di
    pop eax
    pop di
    ret

; Получение информации о процессоре
.cpuid_getdata:
    push di
    xor eax, eax
    mov al, 1
    cpuid
    mov di, bp
    mov eax, edx
    movsd
    mov eax, ecx
    movsd
    mov bp, di
    pop di
    ret

; Инструкция CPUID недоступна
.no_cpuid:
    mov si, no_cpuid
    call printstr
    ret
;==========================================

;====================================
;
;   Получение доступных видеорежимов
;
;   - Извлекает доступные видеокарте режимы.
;
getvideomodes:
    pusha
    push bp
    mov ax, 0x4F00
    mov di, bp
    int 0x10                    ; Извлекаем все доступные видеорежимы
    cmp ax, 0x004F              ; Проверка на успех
    jnz .video_fail
    mov si, video_success_msg
    call printstr
    mov si, 0x6022              ; Поехали перебирать режимы
    lodsw
    push ax
    lodsw
    mov ds, ax
    pop si
    mov ax, 0x0000
    mov es, ax
    mov di, 0x6500
    mov bp, 0x6302
.videomodesloop:
    lodsw
    mov cx, ax                          ; Номер режима
    cmp cx, 0xFFFF                      ; Если 0xFFFF, то режимов больше нет
    je .videomodesend
    mov ax, 0x4F01                      ; Извлекаем параметры режима
    int 0x10
    cmp ax, 0x004F                      ; Проверка на успех
    jne .video_fail
    mov ax, word [es:di]                ; Аттрибуты (интересует только бит 7 - поддержка линейного буфера кадров)
    and ax, 0x80
    jz .videomodesloop                  ; Если нет, то этот режим не рассматриваем
    add di, 16
    mov ax, word [es:di]                ; Количество байтов на строку
    add di, 2
    xor ebx, ebx
    mov bx, word [es:di]                ; Ширина экрана
    add di, 2
    mov ax, word [es:di]
    cwde
    mul ebx                             ; Площадь экрана
    add di, 5
    xor dx, dx
    mov dl, byte [es:di]                ; Число бит на пиксел
    cmp eax, dword [es:bp]              ; Сравниваем площадь экрана с максимальной на данный момент
    jb .videomodesloop                  ; Если меньше, то более режим не рассматриваем
    add bp, 4
    cmp dl, byte [es:bp]                ; Сравниваем б/пикс текущего режима с максимальным на данный момент
    jb .videomodesloop                  ; Если меньше, то более режим не рассматриваем
    ; Найден новый режим лучше, чем имеющийся
    ; Записываем параметры
    mov bp, 0x6300
    mov word [es:bp], cx
    add bp, 2
    mov dword [es:bp], eax
    add bp, 4
    mov byte [es:bp], dl
    mov bp, 0x6302
    mov di, 0x6500
    jmp .videomodesloop
.videomodesend:
    ; Теперь ставим новый режим
    mov bp, 0x6300
    mov ax, 0x4F02
    mov bx, word [es:bp]
    and bx, 0b0111111111111111
    int 10h
    cmp ax, 0x004F
    je .video_fail
    pop bp
    popa
    add bp, 4
    ret
.video_fail:
    mov si, video_fail_msg
    call printstr
    popa
    ret
; Оххххххххххх...
; Авось сработает
;====================================

;====================================
;
;   Получение разметки памяти
;
;   - Исследует доступную ОЗУ и показывает,
;   что можно использовать, а что - нет.
;
getram:
    pusha
    mov eax, 0xE820
    mov di, bp
    xor ebx, ebx
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15
    jc .ramerror
.ramloop:
    or ebx, ebx
    je .ramend
    add di, cx
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    jc .ramerror
    jmp .ramloop

; Вся память прочитана
.ramend:
    mov si, ram_success_msg
    call printstr
    popa
    ret

; Ошибка при чтении памяти
.ramerror:
    mov si, ram_error_msg
    call printstr
    popa
    ret
;=================================

enable_prot_mode:
    ret

main:
    ; Обнуление всей резервированной памяти (чтоб дампы удобнее читать было)
    call resetmem

    ; Настройка BP на 1 КиБ резервированной памяти
    mov bp, 0x6000

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
.halt:
    jmp .halt