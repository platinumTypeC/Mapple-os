ARCH                := $(shell uname -m | sed s,i[3456789]86,ia32,)

BOOTLOADER_SRCFILES     := $(shell find src/bootloader -name '*.c')
BOOTLOADER_OBJS          = $(BOOTLOADER_SRCFILES:.c=.o)

KERNEL_SRCFILES         := $(shell find src/kernel -name "*.c")
KERNEL_OBJS              = $(KERNEL_SRCFILES:.c=.o)

EFILIB          = ./efilib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS          = -Isrc/include -fno-stack-protector -fpic \
		  		  -fshort-wchar -mno-red-zone -Wall -DMAPPLE_DEBUG
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
				  -Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS) 

compile: src/boot.efi src/kernel.efi
	rm src/boot.so
	rm src/kernel.so
	mv $(shell find src -name '*.o') ./lib

linux: compile iso run
wsl: compile iso runwsl

%.o: %.c 
	gcc ${CFLAGS} -c $< -o $@

src/boot.so: ${BOOTLOADER_OBJS} $(shell find src -name "*.h")
	ld $(LDFLAGS) $(BOOTLOADER_OBJS) -o $@ -lefi -lgnuefi

src/boot.efi: src/boot.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

src/kernel.so: ${KERNEL_OBJS} $(shell find src -name "*.h")
	ld -nostdlib -znocombreloc -shared -Bsymbolic $(KERNEL_OBJS) -o $@

src/kernel.efi: src/kernel.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

install:
	sudo apt install -y gcc
	sudo apt install -y binutils
	sudo apt install -y iat

iso:
	@rm -r dist
	@mkdir -p dist/EFI/Boot/
	cp src/boot.efi dist/EFI/Boot/
	cp src/kernel.efi dist/
	cp src/startup.nsh dist/
	dd if=/dev/zero of=Mapple.img bs=1M count=500
	mformat -i Mapple.img ::
	mcopy -si Mapple.img dist/* ::
clean:
	@rm $(shell find src -name '*.o') src/kernel.so src/kernel.efi

run:
	qemu-system-x86_64 -drive file=Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"
runwsl:
	qemu-system-x86_64.exe -drive file=Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd" 