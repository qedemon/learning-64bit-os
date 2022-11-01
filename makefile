all: ImageMaker BootLoader Kernel32 Kernel64 Disk.img

BootLoader:
	@echo
	@echo ========== Build Boot Loader ==========
	@echo
	make -C 00.BootLoader

Kernel32:
	@echo
	@echo ========== Build Kernel32 ==========
	@echo
	make -C 01.Kernel32

Kernel64:
	@echo
	@echo ========== Build Kernel64 ==========
	@echo
	make -C 02.Kernel64

ImageMaker:
	@echo
	@echo ========== Build Utility Program ==========
	@echo
	make -C 04.Utility

Disk.img: 00.BootLoader/BootLoader.bin 01.Kernel32/Kernel32.bin 02.Kernel64/Kernel64.bin
	python ImageMaker.py $@ $^

clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	make -C 02.Kernel64 clean
	rm -f ImageMaker.py
	rm -f Disk.img
