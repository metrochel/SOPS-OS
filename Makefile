DISKFILE		=build/sops.img
BOOTSRC			=$(sort $(wildcard src/boot/*.asm))
BOOTBINS		=$(patsubst src/boot/%.asm, build/bins/%.bin, $(BOOTSRC))
BUILDDIR		=build
BINSDIR			=build/bins
OBJSDIR			=build/objs
KERNELSRC   	=src/kernel/kernel.cpp $(wildcard src/kernel/*/*.cpp)
KERNELOBJ       =$(foreach cpp, $(KERNELSRC), build/objs/$(patsubst %.cpp,%.o,$(notdir $(cpp))))
KERNELBIN		=$(BINSDIR)/kernel.bin
CROSSCOMPILER	=i686-elf-g++
LINKERSCRIPT    =linker.ld

#
#	Полная сборка СОпС
#
all: $(BUILDDIR) $(DISKFILE) $(BINSDIR) $(BOOTBINS) $(KERNELBIN)
	cat $(BOOTBINS) $(KERNELBIN) | mcat -i $(DISKFILE) -w

#
#	Создание папки со сборкой
#
$(BUILDDIR):
	mkdir -p build

#
#	Создание папки с бинарными файлами
#
$(BINSDIR): $(BUILDDIR)
	mkdir -p build/bins

#
#	Создание папки с объектными файлами
#
$(OBJSDIR): $(BUILDDIR)
	mkdir -p build/objs

assemble_source =nasm -f bin $(1) -o $(patsubst src/boot/%.asm, build/bins/%.bin, $(1)) -w-zeroing

#
#	Сборка исходных файлов загрузчика
#
$(BOOTBINS): $(BOOTSRC) 
	$(foreach bsrc, $(BOOTSRC), $(shell $(call assemble_source, $(bsrc))))
#
#	Создание файла дискеты
#
$(DISKFILE):
	dd if=/dev/zero of=$(DISKFILE) count=16000 bs=512

#
#	Компиляция файлов ядра
#
compile_cpp_source = $(CROSSCOMPILER) -c $(1) -o $(OBJSDIR)/$(subst .cpp,.o,$(notdir $(1))) -ffreestanding -O2 -lgc++ -Wall -Wextra -fno-exceptions -fno-rtti $(if $(filter-out src/kernel/int/int.cpp,$(1)),,-mgeneral-regs-only) $(if $(filter-out src/kernel/acpi/sci.cpp,$(1)),,-mgeneral-regs-only)

$(KERNELOBJ): $(KERNELSRC) $(OBJSDIR)
	$(foreach cpp, $(KERNELSRC), $(shell $(call compile_cpp_source, $(cpp))))

#
#	Компоновка ядра
#
$(KERNELBIN): $(KERNELOBJ)
	$(CROSSCOMPILER) -T $(LINKERSCRIPT) -o $(KERNELBIN) -ffreestanding -O2 -nostdlib $(KERNELOBJ)

#
#	Очистка сборки
#
clean:
	rm $(DISKFILE)
	rm -r $(BUILDDIR)