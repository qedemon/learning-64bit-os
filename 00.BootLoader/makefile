all: BootLoader.bin

NASM = nasm
NDISASM = ndisasm
ifeq ($(OS), Windows_NT)
	NASM = C:/msys64/usr/bin/nasm.exe
	NDISASM = C:/msys64/usr/bin/ndisasm.exe
endif

BootLoader.bin: BootLoader.asm
	$(NASM) -o $@ $<
	$(NDISASM) -b16 $@ > $@.asm

clean:
	rm -f BootLoader.bin