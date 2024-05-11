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

gdt_load_success db "GDT uspeshno zagruzhena!", NEWL, 0

gdt_desc:
    .limit      dw 0
    .base       dd 0

gdt_start:
    null            dq 0
    lvl_0_code      dq 0
    lvl_0_data      dq 0
    lvl_1_code      dq 0
    lvl_1_data      dq 0
    lvl_2_code      dq 0
    lvl_2_data      dq 0
    lvl_3_code      dq 0
    lvl_3_data      dq 0
    task_state      dq 0
gdt_end:

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
    ret
; A20 не включена
.a20_fail:
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
    stosd
    mov eax, ecx
    stosd
    mov bp, di
    pop di
    ret

; Инструкция CPUID недоступна
.no_cpuid:
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
    mov ax, 0x4F00
    mov di, bp
    int 0x10                    ; Извлекаем все доступные видеорежимы
    cmp ax, 0x004F              ; Проверка на успех
    jnz .video_fail
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
    popa
    add bp, 4
    ret
.video_fail:
    popa
    ret
; Оххххххххххх...
; Авось сработает
; (Сработало, кстати!)
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
    popa
    ret

; Ошибка при чтении памяти
.ramerror:
    popa
    ret
;=================================

;====================================
;
;   Переход в защищённый режим
;
;   - Активация 32-битной адресной шины, что позволит нам адресовать
;     куда больше памяти и немножко защитит наш код.
;
enable_prot_mode:
    cli                             ; Отключаем прерывания (если не выключить, будет риск казуса)
    ; Кодирование строк GDT
    ; Для C++ надо будет использовать страницы, так что просто ставим
    ; все секторы на всю память
    ; Нулевой сегмент кодировать не нужно - там всё равно всё по нулям
    ; Сегмент кода уровня 0
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10011011
    mov ch,  0b1100
    mov di,  lvl_0_code
    call .encode_gdt

    ; Сегмент данных уровня 0
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10010011
    mov ch,  0b1100
    mov di,  lvl_0_data
    call .encode_gdt

    ; Сегмент кода уровня 1
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10111011
    mov ch,  0b1100
    mov di,  lvl_1_code
    call .encode_gdt

    ; Сегмент данных уровня 1
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10110011
    mov ch,  0b1100
    mov di,  lvl_1_data
    call .encode_gdt

    ; Сегмент кода уровня 2
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11011011
    mov ch,  0b1100
    mov di,  lvl_2_code
    call .encode_gdt

    ; Сегмент данных уровня 2
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11010011
    mov ch,  0b1100
    mov di,  lvl_2_data
    call .encode_gdt

    ; Сегмент кода уровня 3
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11111011
    mov ch,  0b1100
    mov di,  lvl_3_code
    call .encode_gdt

    ; Сегмент данных уровня 3
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11110111
    mov ch,  0b1100
    mov di,  lvl_3_data
    call .encode_gdt

    ; Регистрация GDT
    mov eax, gdt_start
    mov dword [es:bp + 2], eax      ; Кладём в структуру GDTR начало таблицы GDT
    mov eax, gdt_end
    sub eax, gdt_start              ; Длина = Конец - Начало
    mov word [es:bp], ax            ; Кладём в структуру GDTR длину таблицы GDT
    lgdt [es:bp]                    ; Загружаем структуру GDTR в процессор
    add bp, 6                       ; Перемещаем указатель памяти в свободное место

    ; Активация защищённого режима
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Переход к коду защищённого режима
    jmp 0x08:clean
;
;   Кодирование структуры GDT
;
;   На вход:
;      - EAX    - предельный размер сегмента (используются только нижние 20 бит)
;      - EBX    - начальный адрес сегмента
;      - CL     - байт доступа сегмента
;      - CH     - байт флагов сегмента (используются только нижние 4 бита)
;      - ES:DI  - место кодирования сегмента
;
.encode_gdt:
    ; В начале DI = 0 (относительно начала, равно как и далее)
    mov byte [es:di], al    ; ES:DI[0] = EAX & 0xFF
    inc di                  ; DI = 1
    shr eax, 8              
    mov byte [es:di], al    ; ES:DI[1] = (EAX >> 8) & 0xFF
    shr eax, 8
    and eax, 0x0F
    add di, 5               ; DI = 6
    mov byte [es:di], al    ; ES:DI[6] = (EAX >> 16) & 0x0F
    sub di, 4               ; DI = 2
    mov byte [es:di], bl    ; ES:DI[2] = EBX & 0xFF
    shr ebx, 8
    inc di                  ; DI = 3
    mov byte [es:di], bl    ; ES:DI[3] = (EBX >> 8) & 0xFF
    shr ebx, 8
    inc di                  ; DI = 4
    mov byte [es:di], bl    ; ES:DI[4] = (EBX >> 16) & 0xFF
    shr ebx, 8
    add di, 3               ; DI = 7
    mov byte [es:di], bl    ; ES:DI[7] = (EBX >> 24) & 0xFF
    sub di, 2               ; DI = 5
    mov byte [es:di], cl    ; ES:DI[5] = CL
    inc di                  ; DI = 6
    shl ch, 4
    or  byte [es:di], ch    ; ES:DI[6] |= CH
    ret                     ; Готово!
; Трындец, я знаю. Но такова реальность: если верить источникам,
; структура намеренно настолько сложная - всё дело в обратной совместимости.
; Теперь я понимаю, почему сложно развивать обратную совместимость.
;=============================================

;========================================================
;
;   Точка входа
;
;   - Место, где вызываются все ранее описанные функции.
;   Совершаем последние приготовления перед развёртыванием
;   ядра.
;
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
    jmp enable_prot_mode

    ; TODO: Загрузка ядра
.halt:
    jmp .halt
; Ядро, кстати, будем писать на C (а лучше на C++).
; Наконец-то мы перейдём к высокоуровнему программированию!
; Ура!
;=====================================================

;
;   Код защищённого режима
;
;   - После перехода в защищённый режим весь код сверху использоваться
;   более не может, потому мы его стираем полностью. Только копируем
;   сведения, раздобытые БИОСом в новый сегмент для данных ядра.
;
[bits 32]
;==========================================
;
;   Копирование резервированной памяти
;
;   - Копирует 1 КиБ резервированной памяти в новое место.
;
copyres:
    push esi
    push edi
    push cx
    mov esi, 0x6000
    mov edi, 0x100000
    mov cx, 0x400
    rep movsd
    pop cx
    pop edi
    pop esi
    ret
;==============================================

clean:
    ; Инициализация сегментов
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x9000

    ; Копирование 1 КиБ памяти в новое место для памяти
    call copyres
.halt:
    jmp .halt