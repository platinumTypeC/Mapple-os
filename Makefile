all: compile

compile:
	@make --no-print-directory -C src/bootloader
	@make --no-print-directory -C src/kernel

kernel:
	@make --no-print-directory -C src/kernel

bootloader:
	@make --no-print-directory -C src/bootloader

install:
	sudo apt install -y gcc
	sudo apt install -y g++
	sudo apt install -y binutils
	sudo apt install -y mtools
	sudo apt install -y nasm

fix:
	rm -rf OVMFbin/
	git clone https://github.com/platinumTypeC/OVMFbin.git

iso: compile
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

run: compile iso
	$(shell bash ./run.sh)

qemu:
	$(shell bash ./run.sh)