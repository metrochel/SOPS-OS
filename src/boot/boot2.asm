;
;   Загрузчик 2 стадии
;
;   - Готовит систему к загрузке ядра.
;
org 0x7000
section .code

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

;============================
;
;   Вывод шестнадцатеричного числа на экран
;
;   - Выводит на экран число в шестнадцатеричной системе счисления.
;
;   На вход:
;       - DX - число
;
printhexnum:
    pusha
    mov ah, 0x0E
    mov bx, 0
    mov al, '0'
    int 0x10
    mov al, 'x'
    int 0x10
    mov cx, 0xF000
.loop:
    mov ax, dx
    and ax, cx
.digitloop:
    cmp ax, 16
    jb .digitloopout
    shr ax, 4
    jmp .digitloop
.digitloopout:
    call gethexdigit
    mov ah, 0x0E
    int 0x10
    shr cx, 4
    cmp cx, 0
    jnz .loop
.out:
    mov al, CR
    int 0x10
    mov al, NEWL
    int 0x10
    popa
    ret

gethexdigit:
    cmp al, 10
    jae .strlet
    add al, 0x30
    ret
.strlet:
    add al, 0x41
    sub al, 10
    ret
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
;  Активация адресной линии А20
;
;  - Без неё не получится использовать всю ОЗУ в ПК.
;
enable_a20:
    cli
    call .check_a20
    jc .a20_on
    call .enable_a20_bios
    jc .a20_on
    call .check_a20
    jc .a20_on
    call .enable_a20_ps2
    call .check_a20
    jc .a20_on
    sti
    jnc .a20_fail
    ret
; Активация линии A20 через БИОС
.enable_a20_bios:
    pusha
    ; Пробуем прерывание БИОСа
    mov ax, 0x2403
    int 0x15

    ; Если не сработало, выходим
    jb ._bios_fail
    cmp ah, 0
    jb ._bios_fail

    ; Проверяем, включена ли линия
    mov ax, 0x2402
    int 0x15

    ; Если не сработало, выходим
    jb ._bios_fail
    cmp ah, 0
    jnz ._bios_fail

    ; Если AL = 1, то линия включена
    cmp al, 1
    jz ._bios_active

    ; Пробуем включить
    mov ax, 0x2401
    int 0x15

    ; Если не сработало, выходим
    jb ._bios_fail
    cmp ah, 0
    jnz ._bios_fail

._bios_active:
    ; Если мы здесь, то всё сработало!
    popa
    stc
    ret

._bios_fail:
    ; А если здесь, то не сработало
    popa
    clc
    ret


; Активация линии А20 через контроллер PS/2
.enable_a20_ps2:
    pusha
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
    clc
    ret
; A20 не включена
.a20_fail:
    stc
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
    clc
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
    add bp, 20
    stc
    ret
;==========================================

;====================================
;
;   Получение доступных видеорежимов
;
;   - Извлекает доступные видеокарте режимы.
;
MAXSCREENSURFACE    dd 0
MAXBPP              db 0
MAXMODE             dw 0
BUFSCREENSURFACE    dd 0
BUFBPP              db 0
BUFMODE             dw 0
getvideomodes:
    pusha

    ; Получаем доступные режимы
    mov di, bp
    mov ax, 0x4F00
    int 0x10
    cmp ax, 0x004F
    mov dh, 0x01
    jne .video_fail

    ; Настраиваемся на обработку режимов
    mov si, 0x6015 + 14
    lodsw
    push ax
    lodsw
    mov ds, ax
    pop si
    mov ax, 0
    mov es, ax
    mov di, 0x6500

    ; Перебираем все режимы
.videoloop:
    ; Считываем номер режима
    lodsw
    cmp ax, 0xFFFF
    je .videoend

    ; Достаём его параметры
    mov cx, ax
    mov ax, 0x4F01
    mov di, 0x6500
    int 10h

    ; Проверяем на успех
    cmp ax, 0x004F
    mov dh, 2
    jne .video_fail

    ; Проверяем на наличие буфера кадров
    mov ax, word [0x6500]
    test ax, 0x80
    jz .videoloop

    ; Считаем площадь экрана и биты на пиксел
    xor eax, eax
    mov ax, word [0x6512]
    xor ebx, ebx
    mov bx, word [0x6514]
    mul ebx
    mov dword [BUFSCREENSURFACE], eax
    push si
    mov si, 0x6519
    mov di, BUFBPP
    movsb
    pop si
    mov word [BUFMODE], cx

    ; Проверяем, лучше ли этот режим
    mov eax, dword [BUFSCREENSURFACE]
    cmp eax, dword [MAXSCREENSURFACE]
    ja  .newmax
    jb  ._cont
    mov al,  byte  [BUFBPP]
    cmp al,  byte  [MAXBPP]
    ja  .newmax

._cont:
    jmp .videoloop

.newmax:
    push si
    mov si, BUFSCREENSURFACE
    mov di, MAXSCREENSURFACE
    movsd
    movsb
    movsw
    pop si
    jmp .videoloop

.videocheck:
    ; Проверяем, что нужный режим вообще есть
    cmp word [MAXMODE], 0
    mov dh, 3
    jz  .video_fail

.videoend:
    ; Устанавливаем видеорежим
    mov ax, 0x4F02
    mov bx, word [MAXMODE]
    and bx, 0x7FFF
    or  bx, 0x4000
    int 10h
    cmp ax, 0x004F
    jne .video_fail

    ; Получаем его параметры
    mov ax, 0x4F01
    mov cx, word [MAXMODE]
    mov di, bp
    int 10h

    ; Выходим
    popa
    add bp, 258
    clc
    ret

.video_fail:
    mov ah, 0x0E
    mov al, dh
    add al, 0x40
    mov bx, 0
    int 0x10
    mov dx, cx
    cmp al, 0x42
    jne ._notprint
    call printhexnum
._notprint:
    popa
    stc
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
BLOCKS_AMOUNT dw 0
getram:
    pusha
    mov eax, 0xE820
    mov di, 0xA000              ; 0xA000 - место временного буфера
    xor ebx, ebx
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15                    ; Первый сегмент карты памяти
    jc .ramerror
.ramloop:
    or ebx, ebx
    je .ramend
    inc word [BLOCKS_AMOUNT]    ; Увеличиваем число блоков
    add di, cx
    cmp cx, 20                  ; Если возможность, что размер блока - не 20 Б, а 24. (Это говорит спецификация.)   
    jne ._dontadd               ; Чтобы было проще работать с блоками, если размер блока 20 Б,
    xor eax, eax                ; добавим в конец блока двойное слово с значением 0,
    stosd                       ; чтобы добить размер блока до 24 Б.
._dontadd:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15                    ; Извлекаем оставшиеся блоки
    jc .ramerror
    jmp .ramloop

; Вся память прочитана
.ramend:
    mov si, 0xA000              ; Начинаем сортировать
    mov di, 0xA018
    mov bx, 0                   ; В обычном цикле "for i... for j..." регистр BX - это i,
    mov cx, 1                   ; а регистр CX - это j.

    ; Сортировка выполнена обычным методом пузырька.
.ramsort1:
    lea di, byte [si + 24]
.ramsort2:
    mov eax, dword [ds:si + 4]      ; Так как основания 8-байтные, то сначала
    mov edx, dword [es:di + 4]      ; сверяем верхние 4 байта.
    cmp eax, edx
    jbe ._swap1                     ; Если основание 1-го блока больше, чем 2-го,
    call .swap                      ; то меняем их местами.
._swap1:
    mov eax, dword [ds:si]          ; Аналогично сверяем
    mov edx, dword [es:di]          ; нижние 4 байта.
    cmp eax, edx
    jbe ._swap2                     ; Если основание 1-го блока больше, чем 2-го,
    call .swap                      ; то меняем их местами.
._swap2:
    add di, 24
    inc cx
    cmp cx, word [BLOCKS_AMOUNT]
    jae .endreached                 ; Если внутренний цикл дошёл до конца, то сбрасываем его.
    jmp .ramsort2

.endreached:
    add si, 24                      ; Дошли до конца внутреннего цикла,
    inc bx                          ; увеличиваем BX (i)
    lea cx, [bx + 1]                ; <=> CX = BX + 1
    cmp bx, word [BLOCKS_AMOUNT]    ; Если и внешний цикл дошёл до конца,
    je .ramsorted                   ; то сортировка закончена.
    jmp .ramsort1

    ; Функция смены блоков местами
.swap:
    pusha
    xor cx, cx
    ; Размер одного блока - 24 Б, поэтому достаточно
    ; поменять местами 6 двойных слов (dword).
.swaploop:
    mov eax, dword [si]             ; Берём первое слово
    mov edx, dword [di]             ; Берём второе слово
    xchg eax, edx                   ; Меняем местами
    mov dword [si], eax             ; Кладём на место первое слово
    mov dword [di], edx             ; Кладём на место второе слово
    inc cx
    add si, 4
    add di, 4
    cmp cx, 6                       ; Если сделали это 6 раз,
    jne .swaploop                   ; то выходим
.swapend:
    popa
    ret

    ; После того как мы закончили сортировать блоки,
    ; надо записать их в Данные загрузчика.
.ramsorted:
    mov si, 0xA000                  ; 0xA000 - наш буфер
    mov di, bp                      ; Цель - Данные загрузчика, то есть BP
    xor eax, eax
    mov ax, word [BLOCKS_AMOUNT]
    stosw                           ; Сначала сохраним, сколько блоков мы насчитали
    mov cx, 24 
    mul cx                          ; Число блоков * Размер одного блока = Суммарная длина списка в байтах
    xchg dx, ax                     ; В x86 умножение 16-битного регистра на 16-битный регистр
    shl eax, 16                     ; сохранит результат в DX:AX, поэтому переносим его в
    mov ax, dx                      ; EAX.
    mov ecx, eax                    ; Переносим EAX в ECX
    rep movsb                       ; Копируем байт с SI на DI ECX раз
    mov word [0xA000], di
    popa                            ; Восстанавливаем все регистры
    mov bp, word [0xA000]           ; Сохраняем новое значение BP
    clc
    ret                             ; И всё!

    ; Если что-то пойдёт не так,
.ramerror:
    popa
    stc                             ; устанавливаем флаг CF
    ret                             ; и выходим

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
    mov edi, 0x1008
    call .encode_gdt

    ; Сегмент данных уровня 0
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10010011
    mov ch,  0b1100
    mov edi, 0x1010
    call .encode_gdt

    ; Сегмент кода уровня 1
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10111011
    mov ch,  0b1100
    mov edi, 0x1018
    call .encode_gdt

    ; Сегмент данных уровня 1
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b10110011
    mov ch,  0b1100
    mov edi, 0x1020
    call .encode_gdt

    ; Сегмент кода уровня 2
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11011011
    mov ch,  0b1100
    mov edi, 0x1028
    call .encode_gdt

    ; Сегмент данных уровня 2
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11010011
    mov ch,  0b1100
    mov edi, 0x1030
    call .encode_gdt

    ; Сегмент кода уровня 3
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11111011
    mov ch,  0b1100
    mov edi, 0x1038
    call .encode_gdt

    ; Сегмент данных уровня 3
    mov eax, 0xFFFFF
    mov ebx, 0x00000000
    mov cl,  0b11110011
    mov ch,  0b1100
    mov edi, 0x1040
    call .encode_gdt

    ; Регистрация GDT
    mov eax, 0x1000
    mov dword [0x1502], eax         ; Кладём в структуру GDTR начало таблицы GDT
    mov ax, 0x48
    mov word [0x1500], ax           ; Кладём в структуру GDTR длину таблицы GDT
    lgdt [0x1500]                   ; Загружаем структуру GDTR в процессор
    add bp, 6                       ; Перемещаем указатель памяти в свободное место

    ; Активация защищённого режима
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Переход к коду защищённого режима
    jmp 0x08:prot_mode_entry_point
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
    
    ; Запись номера диска в резервированную память
    mov byte [ds:bp], dl
    inc bp

    ; Активация адресной линии А20
    call enable_a20

    ; Получение данных о процессоре
    call getcpuinfo

    ; Получение доступных видеорежимов
    call getvideomodes

    ; Получение разметки памяти
    call getram

    ; Инициализация x87-сопроцессора
    finit

    ; Активация защищённого 32-битного режима
    jmp enable_prot_mode

.halt:
    jmp .halt
; Ядро, кстати, будем писать на C (а лучше на C++).
; Наконец-то мы перейдём к высокоуровнему программированию!
; Ура!
; (Знал ли он, что его ждёт...)
;=====================================================

;
;   Код защищённого режима
;
;   - После перехода в защищённый режим весь код сверху использоваться
;   более не может, потому мы его стираем полностью. Только копируем
;   сведения, раздобытые БИОСом в новый сегмент для данных ядра.
;
[bits 32]
; LBA-адрес ядра
%define KERNEL_POS      10
; Физический адрес, на который следует загрузить ядро в память
%define KERNEL_PHYSADDR 0x2000000
; Виртуальный адрес, на котором будет находиться ядро
%define KERNEL_VIRTADDR 0xC0000000
; Длина ядра в секторах диска
%define KERNEL_LEN      1024
; ELF-подпись файла
%define ELF_SIGNATURE   0x464C457F

;
;   Таблица прерываний
;
;   - Заглушка, чтобы отслеживать страничные ошибки.
;
idt_r:
    size   dw (50*8) - 1
    offset dd int_dt

int_dt: resd 50 * 2 

load_idt:
    lidt [idt_r]

    mov eax, int_06
    mov word [int_dt + 0x06*8], ax
    mov word [int_dt + 0x06*8 + 2], 0x08
    mov word [int_dt + 0x06*8 + 4], 0x8F00
    shr eax, 16
    mov word [int_dt + 0x06*8 + 6], ax

    mov eax, int_0E
    mov word [int_dt + 0x0E*8], ax
    mov word [int_dt + 0x0E*8 + 2], 0x08
    mov word [int_dt + 0x0E*8 + 4], 0x8F00
    shr eax, 16
    mov word [int_dt + 0x0E*8 + 6], ax

    mov eax, int_0D
    mov word [int_dt + 0x0D*8], ax
    mov word [int_dt + 0x0D*8 + 2], 0x08
    mov word [int_dt + 0x0D*8 + 4], 0x8F00
    shr eax, 16
    mov word [int_dt + 0x0D*8 + 6], ax


    ret

; Прерывание 0x06 - невозможная инструкция
int_06:
    mov eax, '#UD '
    pop ebx
    jmp boot_error

; Прерывание 0x0E - страничный сбой
int_0E:
    mov eax, '#PF '
    pop ebx
    mov ebx, cr2
    jmp boot_error

; Прерывание 0x0D - общий сбой защиты
int_0D:
    mov eax, '#GPF'
    pop ebx
    jmp boot_error


;==========================================
;
;   Копирование резервированной памяти
;
;   - Копирует 4 КиБ резервированной памяти в новое место.
;
copyres:
    push esi
    push edi
    push cx
    mov esi, 0x6000
    mov edi, 0x100000
    mov cx, 0x1000
    rep movsd
    pop cx
    pop edi
    pop esi
    ret
;==============================================

;
;   Маленький драйвер для дисков
;
;   - Драйвер-заглушка для загрузки
;     ядра
;
tinydiskdriver:
.disk_doesnt_exist:
    mov esi, 0x100500
    mov dword [ds:esi], "NDSK"
    popa
    stc
    ret
.disk_not_ata:
    mov esi, 0x100500
    mov dword [ds:esi], "NATA"
    popa
    stc
    ret
.disk_error:
    mov esi, 0x100500
    mov dword [ds:esi], "DERR"
    popa
    stc
    ret
.init:
    ; Проверка диска на существование
    mov dx, 0x1F7
    in  al, dx
    cmp al, 0xFF
    je  .disk_doesnt_exist

    ; Команда IDENTIFY
    pusha
    xor eax, eax
    mov dx, 0x1F6
    in  al, dx
    or  al, 0x40
    out dx, al
    test al, 0x10
    jz  ._init2
    cmp byte [0x100000], 0x81
    jne ._init1
    inc byte [0x100000]
._init1:
    inc byte [0x100000]
._init2:
    mov al, 0x00
    mov dx, 0x1F2
    out dx, al
    inc dx
    out dx, al
    inc dx
    out dx, al
    inc dx
    out dx, al
    mov al, 0xEC
    mov dx, 0x1F7
    out dx, al
    in  al, dx
    or  al, al
    jz  .disk_doesnt_exist
    mov dx, 0x3F6
._statusloop:
    in  al, dx
    and al, 0x80
    jnz ._statusloop
    mov dx, 0x1F4
    in  al, dx
    mov bl, al
    mov dx, 0x1F5
    in  al, dx
    or  al, bl
    jnz .disk_not_ata

    mov dx, 0x3F6
._statusloop2:
    in  al, dx
    mov bl, al
    and al, 0x08
    and bl, 0x01
    or  al, bl
    jz  ._statusloop2

    test bl, 0x01
    jnz  .disk_error
    mov  edi, 0x100500
    mov  cx, 256
    mov  dx, 0x1F0
._dataloop:
    in  ax, dx
    stosw
    dec cx
    cmp cx, 0
    jne ._dataloop

    ; Отключение прерываний
    mov dx, 0x3F6
    mov al, 0b00000010
    out dx, al

    popa
    clc
    ret
;
;   Считывание ядра
;
;   - Считывает ядро. (А вы что думали?)
;
.read_kernel:
    pusha

    ; Настраиваем параметры, которые не будут меняться
    mov cx,  KERNEL_LEN     ; Число секторов
    mov ebx, KERNEL_POS     ; LBA-адрес ядра
    mov edi, 0x900000       ; Адрес, на который совать файл ядра

; Чтение
.sector_loop:
    call .read_sector       ; Читаем сектор
    dec cx
    inc ebx
    cmp cx, 0               ; Все секторы прочитали?
    jne .sector_loop        ; Если не все, продолжаем читать
; Чтение завершено
.read_done:
    clc                     ; Если всё прошло хорошо, то CF = 0
    popa
    ret

; Чтение сектора
.read_sector:
    push ebx
    push cx

    ; Количество секторов
    mov dx, 0x1F2
    mov al, 1
    out dx, al

    ; Низшие 8 бит LBA-координаты
    mov dx, 0x1F3
    mov al, bl
    out dx, al

    ; Следующие 8 бит
    mov dx, 0x1F4
    mov al, bh
    out dx, al

    ; Следующие 8 бит
    mov dx, 0x1F5
    bswap ebx
    mov al, bl
    out dx, al

    ; Команда READ SECTORS
    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; Ждём диск
    call .poll

    ; Диск готов! Работаем, братья
    mov cx, 0x100
    mov dx, 0x1F0
    rep insw

    ; Чуть-чуть ждём, пока диск выдохнет
    mov dx, 0x3F6
    in  al, dx
    in  al, dx
    in  al, dx
    in  al, dx

    ; Сектор считан! Выходим
    pop cx
    pop ebx
    ret

; Ожидание контроллера диска
.poll:
    mov dx, 0x3F6       ; Читаем статус
    in  al, dx
    test al, 0x80       ; Проверяем флаг BSY (диск занят)
    jnz  .poll
    test al, 0x21       ; Проверяем флаги ERR и DRF (ошибка команды или ошибка диска)
    jnz  .read_error
    ret

; Ошибка чтения
.read_error:
    popa
    stc                 ; CF = 1
    mov dx, 0x1F1       ; В AL байт ошибки
    in  al, dx          
    ret

;==========================================================
;
;   Настройка страниц памяти
;
;   - Настраивает страницы памяти и активирует их.
;     Кстати, в 64-битном режиме без них нельзя.
;
%define PAGING_BASE 0x101000
paging_time:
    pusha
    ; Создаём пустую директорию страниц
    mov eax, 0x00000002
    mov ecx, 1024
    mov edi, PAGING_BASE
    rep stosd
    ; Так, адрес директории страниц лежит в дефайне PAGING_BASE, и
    ; там резервировано 4 КиБ данных для таблиц страниц.
    
    ; Создаём "идентичные страницы" (т.е. адреса страниц совпадают с физическими)
    ; Этаких страничек сделаем на 8 МиБ (чтобы все страницы влезли)
    mov edi, PAGING_BASE + 0x1000
    mov ecx, 0
.identity_paging:
    mov eax, ecx
    and eax, 0xFFFFF000
    or  eax, 3
    stosd
    add ecx, 0x1000
    cmp ecx, 0x400000 * 8
    jne .identity_paging

    ; Помещаем видеопамять на таблицы с
    ; виртуальным адресом на конце памяти
    mov edi, PAGING_BASE + 0x3F0000
    mov esi, 0x10003D
    lodsd
    mov ecx, 0
.map_vram:
    and eax, 0xFFFFF000
    or  eax, 3
    stosd
    add eax, 0x1000
    inc ecx
    cmp ecx, 0x400 * 0x10
    jne .map_vram

    ; Помещаем ядро на странички с
    ; виртуальным адресом 0x1000000
    mov edi, PAGING_BASE + 0x1000 + ((KERNEL_VIRTADDR & 0xFFC00000) >> 12) * 4
    mov ebx, KERNEL_PHYSADDR
    mov ecx, 0
.map_kernel:
    mov eax, ebx
    add eax, ecx
    and eax, 0xFFFFF000
    or  eax, 3
    stosd
    add ecx, 0x1000
    cmp ecx, 0x400000
    jne .map_kernel

    ; Загружаем таблицы в директорию
.load_page_tables:
    mov edi, PAGING_BASE
    mov eax, PAGING_BASE + 0x1000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x2000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x3000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x4000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x5000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x6000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x7000
    or  eax, 3
    stosd
    mov eax, PAGING_BASE + 0x8000
    or  eax, 3
    stosd
    
    mov edi, PAGING_BASE + ((KERNEL_VIRTADDR & 0xFFC00000) >> 22) * 4
    mov eax, PAGING_BASE + ((KERNEL_VIRTADDR & 0xFFC00000) >> 12) * 4 + 0x1000
    or  eax, 3
    stosd

    mov edi, PAGING_BASE + (0x3F0 * 4)
    mov eax, PAGING_BASE + 0x3F0000
.put_vram:
    or  eax, 3
    stosd
    add eax, 0x1000
    cmp eax, PAGING_BASE + 0x400000
    jb  .put_vram

    ; Загружаем директорию таблиц
    mov eax, PAGING_BASE
    mov cr3, eax
    
    ; Активируем страничную память
    mov eax, cr0
    or  eax, 0x80000001
    mov cr0, eax

    popa
    ret
;====================================
;
;   Размещение ELF-ядра в памяти
;
;   - Помещает все секции ядра (файл по адресу 0x900000)
;     в физическую память.
;
ENTRYPOINT  dd 0
PHT_OFFSET  dd 0
SHT_OFFSET  dd 0
SH_SIZE     dw 0
SH_AMOUNT   dw 0
SH_STRT_IND dw 0
TEXT_OFFSET dd 0
SEC_HEAD_BUF:
    .SH_NAME        dd 0
    .SH_TYPE        dd 0
    .SH_FLAGS       dd 0
    .SH_ADDR        dd 0
    .SH_OFF         dd 0
    .SH_SIZE        dd 0
    .SH_LINK        dd 0
    .SH_INFO        dd 0
    .SH_ADDRALIGN   dd 0
    .SH_ENTSIZE     dd 0

put_elf:
    pusha

    ; Проверяем подпись файла
    mov esi, 0x900000
    lodsd
    cmp eax, ELF_SIGNATURE
    mov ebx, 1
    jne .elf_error

    ; Достаём пару штук из заголовка
.parse_elf_header:
    mov esi, 0x900018
    lodsd
    mov dword [ENTRYPOINT],  eax
    lodsd
    mov dword [PHT_OFFSET],  eax
    lodsd
    mov dword [SHT_OFFSET],  eax
    add esi, 10
    lodsw
    mov word  [SH_SIZE],     ax
    lodsw
    mov word  [SH_AMOUNT],   ax
    lodsw
    mov word  [SH_STRT_IND], ax

    ; Обрабатываем заголовок программы
.parse_prog_hdr:
    mov esi, 0x900000
    add esi, dword [PHT_OFFSET]
    lodsd
    mov ebx, 2
    test ax, ax
    jz .elf_error
    lodsd
    mov dword [TEXT_OFFSET], eax
    lodsd
    mov edx, eax
    add esi, 8
    lodsd
    xor ecx, ecx
    mov ecx, eax

    ; Мы достали сведения о коде! Кладём его куда надо
.put_text:
    mov esi, dword [TEXT_OFFSET]
    add esi, 0x900000
    mov edi, edx
    rep movsb

    ; Обрабатываем таблицу секций
    mov esi, dword [SHT_OFFSET]
    add esi, 0x900000
    movzx ecx, word [SH_SIZE]
    add esi,  ecx       ; Первые 3 заголовка мы уже скопировали,
                        ; так что лишний раз можно не копировать
    add esi, ecx
    add esi, ecx
    mov cx,  word  [SH_AMOUNT]
    sub cx,  3
.parse_section_table:
    ; Загружаем заголовок в буфер
    lodsd
    mov dword [SEC_HEAD_BUF.SH_NAME], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_TYPE], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_FLAGS],eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_ADDR], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_OFF],  eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_SIZE], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_LINK], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_INFO], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_ADDRALIGN], eax
    lodsd
    mov dword [SEC_HEAD_BUF.SH_ENTSIZE], eax

    ; Проверяем, загружаемый ли он
    test word [SEC_HEAD_BUF.SH_FLAGS], 0x02
    jz ._continue

    ; Ежели загружаемый, то загружаем
    push ecx
    push esi
    mov esi, dword [SEC_HEAD_BUF.SH_OFF]
    add esi, 0x900000
    mov edi, dword [SEC_HEAD_BUF.SH_ADDR]
    mov ecx, dword [SEC_HEAD_BUF.SH_SIZE]
    rep movsb
    pop esi
    pop ecx

    ; Смотрим, все ли секции загрузили
._continue:
    dec cx
    jcxz .load_complete
    jmp  .parse_section_table

    ; Загрузили все секции!
.load_complete:
    popa
    clc
    ret

.elf_error:
    popa
    mov eax, "#ELF"
    stc
    ret

;==============================================
;
;   Входная точка защищённого режима
;
;   - Здесь начинается код защищённого режима.
;     Производит последние приготовления и загружает ядро.
;
prot_mode_entry_point:
    ; Инициализация сегментов
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x800000

    mov byte [0xB8000], 'D'
    mov byte [0xB8001], 0x0C

    ; Копирование 4 КиБ памяти в новое место для памяти
    call copyres

    ; Активация таблицы прерываний
    call load_idt

    ; Инициализация драйвера диска
    call tinydiskdriver.init

    ; Считывание ядра
    call tinydiskdriver.read_kernel

    ; Настройка страниц
    call paging_time

    ; Размещение ядра в памяти
    call put_elf
    jc boot_error

    ; Переход на ядро
    ; EBP сбрасывается для трассировки стека.
    xor ebp, ebp
    ; Вместо JMP используется CALL, чтобы в случае возврата из ядра
    ; процессор не улетел Бог знает куда, а вернулся сюда
    ; и застыл.
    call dword [ENTRYPOINT]
    jmp  _halt

;
;   Обработчик ошибок
;
;   - Останавливает исполнение загрузчика
;     при возникновении ошибки.
;   На вход:
;    - EAX - вид ошибки
;    - EBX - код ошибки
;
boot_error:
    mov edi, 0x9500
    stosd
    mov eax, ebx
    stosd
    cli
_halt:
    jmp _halt


; Дополняем этот файл, чтобы не было проблем с
; положением ядра
times (8*512) - ($-$$) db 0
; Хо-хо-хо-о-о-о! Наконец-то будем писать на языке высокого уровня!
; (Спустя 4 дня наконец-то оно заработало!)
; (а на пятый перестало, но ничего, скоро поправим)
; (...правда?)