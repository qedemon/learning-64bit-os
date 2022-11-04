all: BootLoader.bin

NASM = nasm
NDISASM = ndisasm

BootLoader.bin: BootLoader.asm
	$(NASM) -o $@ $<
	$(NDISASM) -b16 $@ > $@.asm

clean:
	rm -f BootLoader.bin