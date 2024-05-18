DISKFILE		=build/sops.img
BOOTSRC			=$(sort $(wildcard ./src/boot/*.asm))
BOOTBINS		=$(patsubst ./src/boot/%.asm, ./build/bins/%.bin, $(BOOTSRC))
BUILDDIR		=build
BINSDIR			=build/bins
OBJSDIR			=build/objs
KERNELSRC   	=$(sort $(wildcard ./src/kernel/*.cpp))
KERNELOBJ       =$(patsubst ./src/kernel/%.cpp, ./build/objs/%.o, $(KERNELSRC))
KERNELBIN		=$(BINSDIR)/kernel.bin
CROSSCOMPILER	=/usr/local/cross/bin/i686-elf-g++
LINKERSCRIPT    =linker.ld

#
#	Полная сборка СОПС
#
all: $(BUILDDIR) $(DISKFILE) $(BOOTBINS) $(KERNELBIN)
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

assemble_source = nasm -f bin $(1) -o $(patsubst ./src/boot/%.asm, ./build/bins/%.bin, $(1)) -w-zeroing

#
#	Сборка исходных файлов загрузчика
#
$(BOOTBINS): $(BOOTSRC) 
	$(foreach bsrc, $(BOOTSRC), $(shell $(call assemble_source, $(bsrc))))
#
#	Создание файла дискеты
#
$(DISKFILE):
	dd if=/dev/zero of=$(DISKFILE) count=4000 bs=512

#
#	Компиляция файлов ядра
#
compile_cpp_source = $(CROSSCOMPILER) -c $(1) -o $(patsubst ./src/kernel/%.cpp, ./build/objs/%.o, $(1)) -ffreestanding -O2 -lgc++ -Wall -Wextra -fno-exceptions -fno-rtti

$(KERNELOBJ): $(KERNELSRC) $(OBJSDIR)
	$(foreach cpp, $(KERNELSRC), $(shell $(call compile_cpp_source, $(cpp))))

#
#	Компоновка ядра
#
$(KERNELBIN): $(KERNELOBJ)
	$(CROSSCOMPILER) -T $(LINKERSCRIPT) -o $(KERNELBIN) -ffreestanding -O2 -nostdlib $(patsubst ./src/kernel/%.cpp, ./build/objs/%.o, $(KERNELSRC))

#
#	Очистка сборки
#
clean:
	rm $(DISKFILE)
	rm -r $(BUILDDIR)