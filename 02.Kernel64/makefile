NASM64 = nasm -f elf64

UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S), Linux)
	GCC64 = /usr/bin/gcc -m64 -ffreestanding
	LD64 = /usr/bin/ld -melf_x86_64 -Ttext 0x200000 -e main -nostdlib
	OBJCOPY64 = /usr/bin/objcopy -j .text -j .data -j .rodata -j .bss -O binary
endif
ifeq ($(UNAME_S), Darwin)
	GCC64 = /opt/local/bin/x86_64-elf-gcc -m64 -ffreestanding
	LD64 = /opt/local/bin/x86_64-elf-ld -melf_x86_64 -T ../elf_x86_64.x -Ttext 0x200000 -e main -nostdlib
	OBJCOPY64 = /opt/local/bin/x86_64-elf-objcopy -j .text -j .data -j .rodata -j .bss -O binary
endif

OBJECTDIRECTORY = Temp
SOURCEDIRECTORY = Source

all: prepare Kernel64.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

dep:
	@echo ========= Make Dependency File ===========
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo ========= Dependency File Complete =======

executeInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel64.elf

Kernel64.bin: executeInternalBuild
	$(OBJCOPY64) $(OBJECTDIRECTORY)/Kernel64.elf $@
	ndisasm -b64 $@ > $@.asm

clean:
	rm -rf $(OBJECTDIRECTORY)
	rm -f *.bin

ENTRYSOURCEFILE = ../$(SOURCEDIRECTORY)/EntryPoint.s
ENTRYOBJECTFILE = EntryPoint.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
COBJECTFILES = $(notdir $(patsubst %.c, %.o, $(CSOURCEFILES)))
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm, %.o, $(ASSEMBLYSOURCEFILES)))

InternalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > Dependency.dep

$(ENTRYOBJECTFILE) : $(ENTRYSOURCEFILE)
	$(NASM64) -o $@ $<

%.o : ../$(SOURCEDIRECTORY)/%.asm
	$(NASM64) -o $@ $<

%.o : ../$(SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<

Kernel64.elf : $(ENTRYOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD64) -o $@ $^

ifeq ($(wildcard Dependency.dep), Dependency.dep)
include Dependency.dep
endif
