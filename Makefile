#
#	Makefile
#
#	Этот файл - скрипт для GNU make, отвечающий за сборку СОпС.
#   Если вызвать make в командной строке, то make соберёт СОпС
#   в папку build.
#
#=============================================== Переменные ======================================================
# Включение переменных окружения
include .env

BUILDDIR		:=build
SRCDIR			:=src
DISKFILE		:=$(BUILDDIR)/sops.img
BINSDIR			:=$(BUILDDIR)/bins
OBJSDIR			:=$(BUILDDIR)/objs

BOOTSRC			:=$(sort $(wildcard $(SRCDIR)/boot/*.asm))
BOOTBINS		:=$(patsubst $(SRCDIR)/boot/%.asm, $(BUILDDIR)/bins/%.bin, $(BOOTSRC))

OBJSDIR_KERNEL  :=$(OBJSDIR)/kernel
KERNELSRCDIR	:=$(SRCDIR)/kernel
KERNELSRC   	:=$(KERNELSRCDIR)/kernel.cpp $(wildcard $(KERNELSRCDIR)/*/*.cpp)
KERNELOBJ       :=$(foreach cpp, $(KERNELSRC), $(OBJSDIR_KERNEL)/$(patsubst %.cpp,%.o,$(notdir $(cpp))))
KERNELBIN		:=$(BINSDIR)/kernel.bin
KERNELMAP		:=$(BUILDDIR)/kernel.map

LIBC_SRC_DIR	:=$(SRCDIR)/libc
LIBC_ASM32_SRC  :=$(wildcard $(LIBC_SRC_DIR)/i386/*.asm)
LIBC_C_SRC		:=$(wildcard $(LIBC_SRC_DIR)/etc/*.c) $(wildcard $(LIBC_SRC_DIR)/formatters/*.c) $(wildcard $(LIBC_SRC_DIR)/*.c)
LIBCINCLUDE		:=$(SRCDIR)/libc/include
LIBC_HEADERS    :=$(wildcard $(LIBCINCLUDE)/*/*.h) $(wildcard $(LIBCINCLUDE)/*.h)
OBJSDIR_LIBC	:=$(OBJSDIR)/libc
LIBC_ASM32_OBJ	:=$(foreach asm32, $(LIBC_ASM32_SRC), $(OBJSDIR_LIBC)/$(patsubst %.asm,%.o,$(notdir $(asm32))))
LIBC_C_OBJ		:=$(foreach src,$(LIBC_C_SRC),$(OBJSDIR_LIBC)/$(patsubst %.c,%.o,$(notdir $(src))))
LIBC_SHARED_FILE:=$(BUILDDIR)/libc.a

SYSROOT			:=sysroot
SYSROOTINCLUDE	:=$(SYSROOT)/resources/include
SYSROOTLIBS		:=$(SYSROOT)/resources/libs
BUILDDIR_ETC	:=$(BUILDDIR)/etc

SOPSTOOLS			:=${SOPSTOOLS}
CCROSSCOMPILER		:=$(SOPSTOOLS)/i686-sops-gcc
CXXCROSSCOMPILER	:=$(SOPSTOOLS)/i686-sops-g++
ASSEMBLER			:=nasm
ARCHIVER			:=$(SOPSTOOLS)/i686-sops-ar
LINKERSCRIPT    	:=linker.ld
PYTHON				:=python

BUILDUTILS			:=buildutils
SYSCALLMACRO_SCRIPT	:=$(BUILDUTILS)/make_syscall_macros.py
SYSCALLMACRO_INFILE	:=$(KERNELSRCDIR)/run/syscalls.hpp
SYSCALLMACRO_OUTFILE:=$(LIBC_SRC_DIR)/etc/syscall_macros.h

#========================================== Основные цели ===================================================
#
#	Полная сборка СОпС
#
all: $(BUILDDIR) $(DISKFILE) $(BINSDIR) $(BOOTBINS) $(KERNELBIN) $(SYSROOT) all-libc install-libc
	mkfs.vfat -F 32 -R 2048 -s 8 -n "SOPS" -v $(DISKFILE); \
	dd if=$(BINSDIR)/boot.bin of=$(DISKFILE) bs=1 seek=90 conv=notrunc; \
	dd if=$(BINSDIR)/boot2.bin of=$(DISKFILE) bs=512 seek=2 conv=notrunc; \
	dd if=$(KERNELBIN) of=$(DISKFILE) bs=512 seek=10 conv=notrunc; \
	mcopy -pms -i $(DISKFILE) $(SYSROOT)/* :: ;
	
#
#	Полная сборка libc
#
all-libc: $(LIBC_SHARED_FILE) ;
	
#
#	Установка libc в системный корень
#
install-libc: $(LIBCINCLUDE) $(LIBC_SHARED_FILE) install-headers
	cp $(LIBC_SHARED_FILE) $(SYSROOTLIBS)
	cp $(LIBC_ASM32_OBJ) $(SYSROOTLIBS)

install-headers: $(LIBC_HEADERS)
	cp -r $(LIBCINCLUDE)/* $(SYSROOTINCLUDE)
	
#
#	Установка системного корня
#
install-sysroot: $(SYSROOT)
	mcopy -pms -i $(DISKFILE) $(SYSROOT)/* :: ;

#
#	Очистка сборки
#
clean:
	rm $(DISKFILE)
	rm -r $(BUILDDIR)

#
# 	Создание образа дискеты
#
$(DISKFILE):
	dd if=/dev/zero of=$(DISKFILE) count=524280 bs=512

#================================================ Папки =====================================================
#
#	Создание папки со сборкой
#
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

#
#	Создание папки с бинарными файлами
#
$(BINSDIR): $(BUILDDIR)
	mkdir -p $(BUILDDIR)/bins

#
#	Создание папки с объектными файлами
#
$(OBJSDIR): $(BUILDDIR)
	mkdir -p $(BUILDDIR)/objs
	
#
#	Папка с объектными файлами для ядра
#
$(OBJSDIR_KERNEL): $(OBJSDIR)
	mkdir -p $(OBJSDIR_KERNEL)

#
#	Папка с объектными файлами для libc
#
$(OBJSDIR_LIBC): $(OBJSDIR)
	mkdir -p $(OBJSDIR_LIBC)

#
#	Папка с дополнительными файлами для сборки
#
$(BUILDDIR_ETC): $(BUILDDIR)
	mkdir -p $(BUILDDIR_ETC)

#
#	Папка системного корня
#
$(SYSROOT):
	mkdir -p $(SYSROOT)

#================================================== Загрузчик ==================================================

assemble_boot_source =$(ASSEMBLER) -f bin $(1) -o $(patsubst src/boot/%.asm, $(BUILDDIR)/bins/%.bin, $(1)) -w-zeroing

#
#	Сборка исходных файлов загрузчика
#
$(BOOTBINS): $(BOOTSRC) 
	$(foreach bsrc, $(BOOTSRC), $(shell $(call assemble_boot_source, $(bsrc))))

#==================================================== Ядро ========================================================

compile_kernel_cpp_source = $(CXXCROSSCOMPILER) -c $(1) -o $(OBJSDIR_KERNEL)/$(subst .cpp,.o,$(notdir $(1))) -ffreestanding -O2 -lgc++ -Wall -Wextra -Wno-write-strings $(if $(filter-out src/kernel/graphics/text.cpp,$(1)),,-Wno-multichar) -fno-exceptions -fno-rtti $(if $(filter-out src/kernel/int/int.cpp,$(1)),,-mgeneral-regs-only) $(if $(filter-out src/kernel/acpi/sci.cpp,$(1)),,-mgeneral-regs-only)

#
#	Компиляция файлов ядра
#
$(KERNELOBJ): $(KERNELSRC) $(OBJSDIR_KERNEL)
	$(foreach cpp, $(KERNELSRC), $(shell $(call compile_kernel_cpp_source, $(cpp))))

#
#	Компоновка ядра
#
$(KERNELBIN): $(KERNELOBJ) $(BUILDDIR_ETC)
	$(CCROSSCOMPILER) -T $(LINKERSCRIPT) -o $(KERNELBIN) -ffreestanding -O2 -nostdlib -Xlinker -Map=$(BUILDDIR_ETC)/kernel.map $(KERNELOBJ)

#===================================================== Libc =========================================================

assemble_i386_libc_source = $(ASSEMBLER) $(1) -o $(OBJSDIR_LIBC)/$(subst .asm,.o,$(notdir $(1))) -f elf ;

compile_c_libc_source = $(CCROSSCOMPILER) -c $(1) -o $(OBJSDIR_LIBC)/$(subst .c,.o,$(notdir $(1))) ;

#
#	Файл с макросами для системных вызовов
#
$(SYSCALLMACRO_OUTFILE): $(SYSCALLMACRO_INFILE)
	$(PYTHON) $(SYSCALLMACRO_SCRIPT) $(SYSCALLMACRO_INFILE) $(SYSCALLMACRO_OUTFILE)

#
#	Сборка 32-битных ассемблерных файлов libc
#
$(LIBC_ASM32_OBJ): $(LIBC_ASM32_SRC) $(OBJSDIR_LIBC)
	$(foreach asm32, $(LIBC_ASM32_SRC), $(shell $(call assemble_i386_libc_source, $(asm32))))

#
#	Сборка C-файлов libc
#
$(LIBC_C_OBJ): $(LIBC_C_SRC) $(OBJSDIR_LIBC) $(SYSCALLMACRO_OUTFILE)
	$(foreach src, $(LIBC_C_SRC), $(shell $(call compile_c_libc_source, $(src))))

#
#   Сборка файла библиотеки libc
#
$(LIBC_SHARED_FILE): $(LIBC_ASM32_OBJ) $(LIBC_C_OBJ)
	$(ARCHIVER) rcs $(LIBC_SHARED_FILE) $(LIBC_ASM32_OBJ) $(LIBC_C_OBJ)