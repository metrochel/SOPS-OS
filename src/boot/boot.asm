org 0x7C00
bits 16

; Дефайны
%define NEWL 0x0A
%define CR   0x0D


; Заглушка для того чтобы BIOS не исполнял код, который на деле есть данные FAT
start:
	jmp main
	nop

;
; 	FAT12 - Блок параметров BIOS (BPB)
;
BPB_OEM_IDENTIFIER 			db "SOPS..OS"		; OEM-идентификатор (необязателен)
BPB_BYTES_PER_SECTOR 		dw 512				; Количество байтов на сектор
BPB_SECTORS_PER_CLUSTER		db 8				; Количество секторов на кластер
BPB_RESERVED_SECTORS 		dw 64				; Количество зарезервированных секторов (+MBR)
BPB_FAT_TABLES				db 2				; Количество FAT-таблиц на диске
BPB_ROOT_DIR_ENTRIES		dw 240				; Количество входов в корневой каталог (пока хз)
BPB_SECTOR_COUNT			dw 5760				; Количество секторов на носителе
BPB_MEDIA_DESCRIPTOR_TYPE	db 0xF0				; Дескриптор типа носителя (для дискеты 2.88М 0xF0)
BPB_SECTORS_PER_FAT			dw ?				; Количество секторов на таблицу
BPB_SECTORS_PER_TRACK		dw ?				; Количество секторов на дорожку
BPB_NUM_HEADS				dw ?				; Количество головок
BPB_HIDDEN_SECTORS			dd 64				; Количество скрытых секторов (после них начинается раздел)
BPB_LARGE_SECTOR_COUNT		dd 0				; Количество секторов на носителе (если больше 65 535, то
												; вместо поля ранее используется это)
;
;	FAT12 - Расширенная загрузочная запись (EBR)
;
EBR_DRIVE_NUMBER			db 0x00				; Номер диска (0х00 для дискет, 0х80 для дисков)
EBR_RESERVED				db 0x00				; Флаги для Windows NT (то есть резервировано)
EBR_SIGNATURE				db 0x29				; Сигнатура (0х28 либо 0х29)
EBR_SERIAL_VOLUMEID			dd 0x13371337		; Серийный номер тома (необязателен)
EBR_VOLUME_LABEL			db "SOPS       "	; Имя тома
EBR_SYSTEM_IDENTIFIER		db "SOPS    "		; Системный идентификатор (лучше не использовать)



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
	and ch, 0xFF
	mov dl, 0x00
	int 13h
	jnc .done
	jmp diskreset
	int 13h
	jnc .done
	int 13h
	jnc .done
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
	mov sp, 0x5FFF

	; Чтение с диска
	mov al, 2
	mov cx, ((0 & 255) << 8) | ((0 & 768) >> 2) | 2
	mov dh, 0
	mov bx, 0x6000
	call diskread

	; Переход на загрузчик 2 стадии
	jmp 0x0000:0x6000

diskerrstring db "Oshibka chteniya s diska!", 0
diskreadsuccess db "Uspeshno schitani sectori! Molodets!", CR, NEWL, 0
;	Для того чтобы БИОС считал сектор, надо в его конце сделать 
;	сигнатуру 0xAA55, что есть сигнатура загрузочного сектора.
times 510-($-$$) db 0
bootSignature dw 0xAA55