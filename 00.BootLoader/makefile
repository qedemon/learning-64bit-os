
all: BootLoader.bin

BootLoader.bin: BootLoader.asm
	nasm -o $@ $<
	ndisasm -b16 $@ > $@.asm

clean:
	rm -f BootLoader.bin