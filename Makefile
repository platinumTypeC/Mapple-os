all: compile

compile:
	@make --no-print-directory -C src/bootloader
	@make --no-print-directory -C src/kernel

kernel:
	@make --no-print-directory -C src/kernel

bootloader:
	@make --no-print-directory -C src/bootloader

linux: compile iso run
wsl: compile iso runwsl

install:
	sudo apt install -y gcc
	sudo apt install -y g++
	sudo apt install -y binutils
	sudo apt install -y mtools
	sudo apt install -y nasm

iso:
	@rm -rf dist
	@mkdir -p dist/EFI/Boot/
	cp src/boot.efi dist/EFI/Boot/
	cp src/kernel.elf dist/
	cp src/startup.nsh dist/
	cp -r data/* dist/
	dd if=/dev/zero of=Mapple.img bs=1M count=100
	mformat -Fi Mapple.img ::
	mcopy -si Mapple.img dist/* ::

clean:
	@rm -rf $(shell find src -name "*.o") $(shell find src -name "*.so") $(shell find src -name "*.efi") $(shell find src -name "*.elf") $(shell find . -name '*.img')
	@rm -rf dist/

run:
	qemu-system-x86_64 -drive file=Mapple.img,format=raw -m 200M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"
runwsl:
	qemu-system-x86_64.exe -drive file=Mapple.img,format=raw -m 200M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd" 