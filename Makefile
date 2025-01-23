BUILDDIR		=build
DISKFILE		=$(BUILDDIR)/sops.img
BOOTSRC			=$(sort $(wildcard src/boot/*.asm))
BOOTBINS		=$(patsubst src/boot/%.asm, build/bins/%.bin, $(BOOTSRC))
BINSDIR			=$(BUILDDIR)/bins
OBJSDIR			=$(BUILDDIR)/objs
KERNELSRC   	=src/kernel/kernel.cpp $(wildcard src/kernel/*/*.cpp)
KERNELOBJ       =$(foreach cpp, $(KERNELSRC), build/objs/$(patsubst %.cpp,%.o,$(notdir $(cpp))))
KERNELBIN		=$(BINSDIR)/kernel.bin
KERNELMAP		=$(BUILDDIR)/kernel.map
CROSSCOMPILER	=i686-elf-g++
LINKERSCRIPT    =linker.ld

#
#	Полная сборка СОпС
#
all: $(BUILDDIR) $(DISKFILE) $(BINSDIR) $(BOOTBINS) $(KERNELBIN)
	cat $(BOOTBINS) $(KERNELBIN) | mcat -i $(DISKFILE) -w;  \
	# if [ $SOPS_INCLUDE_KRNL_MAP ]; then 			\
	# 	export SOPS_PREFIX=$(DISKFILE); 			\
	# 	sudo mkdir -p /mnt/sops;					\
	# 	./etc/mnt;									\
	# 	sudo cp $(KERNELMAP) /mnt/sops/kernel.map;	\
	# 	./etc/umnt;									\
	# 	unset SOPS_PREFIX;							\
	# fi;


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

assemble_source =nasm -f bin $(1) -o $(patsubst src/boot/%.asm, $(BUILDDIR)/bins/%.bin, $(1)) -w-zeroing

#
#	Сборка исходных файлов загрузчика
#
$(BOOTBINS): $(BOOTSRC) 
	$(foreach bsrc, $(BOOTSRC), $(shell $(call assemble_source, $(bsrc))))
#
#	Создание файла дискеты
#
$(DISKFILE):
	dd if=/dev/zero of=$(DISKFILE) count=524280 bs=512

#
#	Компиляция файлов ядра
#
compile_cpp_source = $(CROSSCOMPILER) -c $(1) -o $(OBJSDIR)/$(subst .cpp,.o,$(notdir $(1))) -ffreestanding -O2 -lgc++ -Wall -Wextra -Wno-write-strings $(if $(filter-out src/kernel/graphics/text.cpp,$(1)),,-Wno-multichar) -fno-exceptions -fno-rtti $(if $(filter-out src/kernel/int/int.cpp,$(1)),,-mgeneral-regs-only) $(if $(filter-out src/kernel/acpi/sci.cpp,$(1)),,-mgeneral-regs-only)

$(KERNELOBJ): $(KERNELSRC) $(OBJSDIR)
	$(foreach cpp, $(KERNELSRC), $(shell $(call compile_cpp_source, $(cpp))))

#
#	Компоновка ядра
#
$(KERNELBIN): $(KERNELOBJ)
	$(CROSSCOMPILER) -T $(LINKERSCRIPT) -o $(KERNELBIN) -ffreestanding -O2 -nostdlib -Xlinker -Map=$(BUILDDIR)/kernel.map $(KERNELOBJ)

#
#	Очистка сборки
#
clean:
	rm $(DISKFILE)
	rm -r $(BUILDDIR)