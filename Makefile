DISKFILE	=build/sops.img
BOOTSRC		=$(sort $(wildcard ./src/boot/*.asm))
BOOTBINS	=$(patsubst ./src/boot/%.asm, ./build/bins/%.bin, $(BOOTSRC))
BUILDDIR	=build
BINSDIR		=build/bins

#
#	Полная сборка СОПС
#
all: $(BUILDDIR) $(DISKFILE) $(BOOTBINS)
	cat $(BOOTBINS) | mcat -i $(DISKFILE) -w

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

assemble_source = nasm -f bin $(1) -o $(patsubst ./src/boot/%.asm, ./build/bins/%.bin, $(1))

#
#	Сборка исходных файлов бутлоадера
#
$(BOOTBINS): $(BOOTSRC) 
	$(foreach bsrc, $(BOOTSRC), $(shell $(call assemble_source, $(bsrc))))
#
#	Создание файла дискеты
#
$(DISKFILE):
	dd if=/dev/zero of=$(DISKFILE) count=2880 bs=512

#
#	Очистка сборки
#
clean:
	rm $(DISKFILE)
	rm $(BOOTBINS)