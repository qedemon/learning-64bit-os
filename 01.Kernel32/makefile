ASSEMBLER = nasm
GCC32 = /opt/local/bin/i386-elf-gcc -m32 -ffreestanding
LD32 = /opt/local/bin/i386-elf-ld -e main -melf_i386 -Ttext 0x10200 -nostdlib
OBJCOPY32 = /opt/local/bin/i386-elf-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY = Source

all: prepare Kernel32.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

$(OBJECTDIRECTORY)/EntryPoint.bin: $(SOURCEDIRECTORY)/EntryPoint.s
	$(ASSEMBLER) -o $@ $<

VirtualOS.bin: VirtualOS.asm
	$(ASSEMBLER) -o $@ $<

dep:
	@echo === Make Dependency File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependency Search Complete ===

executeInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel32.elf

$(OBJECTDIRECTORY)/Kernel32.elf.bin: executeInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/Kernel32.elf $@

Kernel32.bin: $(OBJECTDIRECTORY)/EntryPoint.bin $(OBJECTDIRECTORY)/Kernel32.elf.bin
	cat $^ > $@ 

clean:
	rm -rf $(OBJECTDIRECTORY)
	rm -f *.bin

####################################################
CENTRYPOINTOBJECTFILE = main.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
COBJECTFILES = $(subst main.o, ,$(notdir $(patsubst %.c, %.o, $(CSOURCEFILES))))
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm, %.o, $(ASSEMBLYSOURCEFILES)))

InternalDependency: $(CSOURCEFILES)
	$(GCC32) -MM $^ > Dependency.dep

%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC32) -c $<

%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(ASSEMBLER) -f elf32 -o $@ $<

Kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif