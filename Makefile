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

TARGET		:=$(if $(filter ${ARCH},i386),i686,${ARCH})-sops
ELFTARGET	:=$(if $(filter ${ARCH},i386),elf32,elf64)

SOPSTOOLS			:=${SOPSTOOLS}
CCROSSCOMPILER		:=$(SOPSTOOLS)/$(TARGET)-gcc
CXXCROSSCOMPILER	:=$(SOPSTOOLS)/$(TARGET)-g++
ASSEMBLER			:=nasm
ARCHIVER			:=$(SOPSTOOLS)/$(TARGET)-ar
LINKERSCRIPT    	:=linker.ld
PYTHON				:=python

BUILDDIR		:=build
SRCDIR			:=src
DISKFILE		:=$(BUILDDIR)/sops.img
BINSDIR			:=$(BUILDDIR)/bins
OBJSDIR			:=$(BUILDDIR)/objs

BOOTSRC			:=$(sort $(wildcard $(SRCDIR)/boot/*.asm))
BOOTBINS		:=$(patsubst $(SRCDIR)/boot/%.asm, $(BUILDDIR)/bins/%.bin, $(BOOTSRC))

OBJSDIR_KERNEL  :=$(OBJSDIR)/kernel
KERNEL_SRC_DIR	:=$(SRCDIR)/kernel
KERNEL_CPP_SRC 	:=$(KERNEL_SRC_DIR)/kernel.cpp $(wildcard $(KERNEL_SRC_DIR)/*/*.cpp) $(wildcard $(KERNEL_SRC_DIR)/*/*/*.cpp)
KERNEL_ASM_SRC	:=$(wildcard $(KERNEL_SRC_DIR)/*/${ARCH}/*.asm)
KERNELSRC		:=$(KERNEL_ASM_SRC) $(KERNEL_CPP_SRC)
KERNEL_CPP_OBJ  :=$(foreach cpp, $(KERNEL_CPP_SRC), $(OBJSDIR_KERNEL)/$(patsubst %.cpp,%.o,$(notdir $(cpp))))
KERNEL_ASM_OBJ	:=$(foreach asm, $(KERNEL_ASM_SRC), $(OBJSDIR_KERNEL)/$(patsubst %.asm,%.o,$(notdir $(asm))))
KERNEL_ASM_OBJ	:=$(filter-out $(OBJSDIR_KERNEL)/crti.o,$(KERNEL_ASM_OBJ))
KERNEL_ASM_OBJ	:=$(filter-out $(OBJSDIR_KERNEL)/crtn.o,$(KERNEL_ASM_OBJ))
KERNEL_ASM_OBJ	:=$(filter-out $(OBJSDIR_KERNEL)/crt0.o,$(KERNEL_ASM_OBJ))
KERNEL_CRTBEGIN	:=$(shell $(CXXCROSSCOMPILER) -print-file-name=crtbegin.o)
KERNEL_CRTEND	:=$(shell $(CXXCROSSCOMPILER) -print-file-name=crtend.o)
KERNELOBJ		:=$(OBJSDIR_KERNEL)/crt0.o $(OBJSDIR_KERNEL)/crti.o $(KERNEL_CRTBEGIN) $(KERNEL_CPP_OBJ) $(KERNEL_ASM_OBJ) $(KERNEL_CRTEND) $(OBJSDIR_KERNEL)/crtn.o
KERNELBIN		:=$(BINSDIR)/kernel.bin
KERNELMAP		:=$(BUILDDIR)/kernel.map

LIBC_SRC_DIR	:=$(SRCDIR)/libc
LIBC_ASM_SRC  	:=$(wildcard $(LIBC_SRC_DIR)/${ARCH}/*.asm)
LIBC_C_SRC		:=$(wildcard $(LIBC_SRC_DIR)/etc/*.c) $(wildcard $(LIBC_SRC_DIR)/formatters/*.c) $(wildcard $(LIBC_SRC_DIR)/*.c)
LIBCINCLUDE		:=$(SRCDIR)/libc/include
LIBC_HEADERS    :=$(wildcard $(LIBCINCLUDE)/*/*.h) $(wildcard $(LIBCINCLUDE)/*.h)
OBJSDIR_LIBC	:=$(OBJSDIR)/libc
LIBC_ASM_OBJ	:=$(foreach asm, $(LIBC_ASM_SRC), $(OBJSDIR_LIBC)/$(patsubst %.asm,%.o,$(notdir $(asm))))
LIBC_C_OBJ		:=$(foreach src,$(LIBC_C_SRC),$(OBJSDIR_LIBC)/$(patsubst %.c,%.o,$(notdir $(src))))
LIBC_SHARED_FILE:=$(BUILDDIR)/libc.a

SYSROOT			:=sysroot
SYSROOTINCLUDE	:=$(SYSROOT)/resources/include
SYSROOTLIBS		:=$(SYSROOT)/resources/lib
BUILDDIR_ETC	:=$(BUILDDIR)/etc

BUILDUTILS			:=buildutils
SYSCALLMACRO_SCRIPT	:=$(BUILDUTILS)/make_syscall_macros.py
SYSCALLMACRO_INFILES:=$(KERNEL_SRC_DIR)/syscall/syscall.hpp $(wildcard $(KERNELSRCDIR)/syscall/syscalls/incl/*.hpp)
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
	mcopy -pmso -i $(DISKFILE) $(SYSROOT)/* :: ;

#
#	Полная сборка libc
#
all-libc: $(LIBC_SHARED_FILE) ;
	
#
#	Установка libc в системный корень
#
install-libc: $(LIBCINCLUDE) $(LIBC_SHARED_FILE) install-headers
	cp $(LIBC_SHARED_FILE) $(SYSROOTLIBS)
	cp $(LIBC_ASM_OBJ) $(SYSROOTLIBS)

install-headers: $(LIBC_HEADERS)
	cp -r $(LIBCINCLUDE)/* $(SYSROOTINCLUDE)
	
#
#	Установка системного корня
#
install-sysroot: $(SYSROOT)
	mcopy -pmso -i $(DISKFILE) $(SYSROOT)/* :: ;

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
$(BINSDIR): | $(BUILDDIR)
	mkdir -p $(BUILDDIR)/bins

#
#	Создание папки с объектными файлами
#
$(OBJSDIR): | $(BUILDDIR)
	mkdir -p $(BUILDDIR)/objs
	
#
#	Папка с объектными файлами для ядра
#
$(OBJSDIR_KERNEL): | $(OBJSDIR)
	mkdir -p $(OBJSDIR_KERNEL)

#
#	Папка с объектными файлами для libc
#
$(OBJSDIR_LIBC): | $(OBJSDIR)
	mkdir -p $(OBJSDIR_LIBC)

#
#	Папка с дополнительными файлами для сборки
#
$(BUILDDIR_ETC): | $(BUILDDIR)
	mkdir -p $(BUILDDIR_ETC)

#
#	Папка системного корня
#
$(SYSROOT):
	mkdir -p $(SYSROOT)

#================================================== Загрузчик ==================================================

assemble_boot_source =$(ASSEMBLER) -f bin $(1) -o $(patsubst src/boot/%.asm, $(BUILDDIR)/bins/%.bin, $(1))	\
						-w-zeroing

#
#	Сборка исходных файлов загрузчика
#
$(BINSDIR)/%.bin : $(SRCDIR)/boot/%.asm | $(BINSDIR) ; \
    $(ASSEMBLER) -f bin $< -o $@ ;

#==================================================== Ядро ========================================================

#
#	Компиляция файлов ядра
#

$(OBJSDIR_KERNEL)/%.o : $(KERNEL_SRC_DIR)/%.cpp | $(OBJSDIR_KERNEL) ; \
    $(CXXCROSSCOMPILER) -c $< -o $@ -ffreestanding -masm=intel \
	-O2 -lgc++ -Wall -Wextra -Wno-write-strings \
	$(if $(filter-out src/kernel/graphics/text.cpp,$<),,-Wno-multichar) -fno-exceptions -fno-rtti \
	$(if $(filter-out src/kernel/int/int.cpp,$<),,-mgeneral-regs-only) \
	$(if $(filter-out src/kernel/acpi/sci.cpp,$<),,-mgeneral-regs-only)  \
	$(if $(filter ${ARCH},x86_64),-mcmodel=large) \
	-I$(KERNEL_SRC_DIR) ;

$(OBJSDIR_KERNEL)/%.o : $(KERNEL_SRC_DIR)/*/%.cpp | $(OBJSDIR_KERNEL) ; \
    $(CXXCROSSCOMPILER) -c $< -o $@ -ffreestanding -masm=intel \
	-O2 -lgc++ -Wall -Wextra -Wno-write-strings \
	$(if $(filter-out src/kernel/graphics/text.cpp,$<),,-Wno-multichar) -fno-exceptions -fno-rtti \
	$(if $(filter-out src/kernel/int/int.cpp,$<),,-mgeneral-regs-only) \
	$(if $(filter-out src/kernel/acpi/sci.cpp,$<),,-mgeneral-regs-only)  \
	$(if $(filter ${ARCH},x86_64),-mcmodel=large) \
	-I$(KERNEL_SRC_DIR) ;

$(OBJSDIR_KERNEL)/%.o : $(KERNEL_SRC_DIR)/*/*/%.cpp | $(OBJSDIR_KERNEL) ; \
    $(CXXCROSSCOMPILER) -c $< -o $@ -ffreestanding -masm=intel \
	-O2 -lgc++ -Wall -Wextra -Wno-write-strings \
	$(if $(filter-out src/kernel/graphics/text.cpp,$<),,-Wno-multichar) -fno-exceptions -fno-rtti \
	$(if $(filter-out src/kernel/int/handles/%.cpp,$<),,-mgeneral-regs-only) \
	$(if $(filter ${ARCH},x86_64),-mcmodel=large) \
	-I$(KERNEL_SRC_DIR) ;

$(OBJSDIR_KERNEL)/%.o : $(KERNEL_SRC_DIR)/%.asm | $(OBJSDIR_KERNEL) ; \
    $(ASSEMBLER) -f $(ELFTARGET) $< -o $@

$(OBJSDIR_KERNEL)/%.o : $(KERNEL_SRC_DIR)/*/%.asm | $(OBJSDIR_KERNEL) ; \
    $(ASSEMBLER) -f $(ELFTARGET) $< -o $@

#
#	Компоновка ядра
#
$(KERNELBIN): $(KERNELOBJ) | $(BUILDDIR_ETC)
	$(CXXCROSSCOMPILER) -T $(LINKERSCRIPT) -o $(KERNELBIN) -lgcc -ffreestanding -O2 -nostdlib -Xlinker \
 	-Map=$(BUILDDIR_ETC)/kernel.map $(if $(filter ${ARCH},x86_64),-mcmodel=large) \
 	$(KERNELOBJ)

#===================================================== Libc =========================================================

#
#	Файл с макросами для системных вызовов
#
$(SYSCALLMACRO_OUTFILE): $(SYSCALLMACRO_INFILES)
	$(PYTHON) $(SYSCALLMACRO_SCRIPT) $(SYSCALLMACRO_OUTFILE) $(SYSCALLMACRO_INFILES)

#
#	Сборка ассемблерных файлов libc
#
$(OBJSDIR_LIBC)/%.o : $(LIBC_SRC_DIR)/${ARCH}/%.asm | $(OBJSDIR_LIBC) ; \
    $(ASSEMBLER) -f $(ELFTARGET) $< -o $@

#
#	Сборка C-файлов libc
#
$(OBJSDIR_LIBC)/%.o : $(LIBC_SRC_DIR)/%.c | $(SYSCALLMACRO_OUTFILE) $(OBJSDIR_LIBC) ; \
	$(CCROSSCOMPILER) -c $< -o $@ ;
	
$(OBJSDIR_LIBC)/%.o : $(LIBC_SRC_DIR)/etc/%.c | $(SYSCALLMACRO_OUTFILE) $(OBJSDIR_LIBC) ; \
    $(CCROSSCOMPILER) -c $< -o $@ ;

$(OBJSDIR_LIBC)/%.o : $(LIBC_SRC_DIR)/formatters/%.c | $(SYSCALLMACRO_OUTFILE) $(OBJSDIR_LIBC) ; \
    $(CCROSSCOMPILER) -c $< -o $@ ;
    
#
#   Сборка файла библиотеки libc
#
$(LIBC_SHARED_FILE): $(LIBC_ASM_OBJ) $(LIBC_C_OBJ)
	$(ARCHIVER) rcs $(LIBC_SHARED_FILE) $(LIBC_ASM_OBJ) $(LIBC_C_OBJ)