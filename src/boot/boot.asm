org 0x7C5A
bits 16

;
;	===== Загрузчик 1 стадии =====
;
;	Загрузчик 1 стадии в первую очередь помещается BIOSом в память по адресу
;	0x7C00. Отсюда нам надо выйти на загрузчик 2 стадии, так как 1 стадия
;	не может превышать размер 1 сектора, то есть 512 байтов на традиционном жёстком диске.
;

; Дефайны
%define NEWL 0x0A
%define CR   0x0D

_start:
	jmp main

;
;	Вывод строки на экран
;
;	- Название говорит само за себя. Просто выводим
;	  строчку на экран.
;
printstr:
	mov ah, 0x0E
	mov bx, 0
.loop:
	lodsb
	or al, al
	jz .out
	int 10h
	jmp .loop
.out:
	ret

;
;	Сброс контроллера диска
;
;	- Если не получается считать секторы с диска, 
;	  необходимо сбросить его контроллер.
;
diskreset:
	push ax
	mov ah, 0x00
	int 13h
	pop ax
	ret


;
;	Чтение секторов с диска
;
;	- Название говорит само за себя. Считывает секторы с диска и
;	  помещает их в ОЗУ.
;
diskread:
	pusha
	mov ah, 0x02
	int 13h
	jnc .done
	call diskreset
	int 13h
	jnc .done
	call diskreset
	int 13h
	jnc .done
	call diskreset
	jmp .diskerror
.done:
	mov si, diskreadsuccess
	call printstr
	popa
	ret

.diskerror:
	mov si, diskerrstring
	call printstr
	hlt

;
;	Точка входа в загрузчик
;	
;	- Отсюда всё и начинается. Считываем секторы загрузчика 2
;	  стадии и передаём на них управление
;
main:
	; Настройка сегментов
	mov ax, 0
	mov ds, ax
	mov es, ax
	
	; Настройка стека
	mov ss, ax
	mov sp, 0x6000

	; Копирование MBR на адрес 0х6000
	mov cx, 512
	mov si, 0x7C00
	mov di, 0x6000
	rep movsb

	; Переход на копию
	jmp .mbr_after_copy - 0x1C00
.mbr_after_copy:

	; Загрузка загрузчика 2 ступени на адрес 0x7000
	mov al, 8
	mov cx, ((0 & 255) << 8) | ((0 & 768) >> 2) | 3
	mov dh, 0
	mov bx, 0x7000
	call diskread

	; Переход на загрузчик 2 ступени (сдвиг на 0x1C00 необходим, потому что иначе адрес повреждается,
	; потом выясним, почему (наверное))
	jmp 0x7000 + 0x1C00

diskerrstring db "Oshibka chteniya s diska!", 0
diskreadsuccess db "Uspeshno schitani sectori! Molodets!", CR, NEWL, 0