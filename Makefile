ARCH                := $(shell uname -m | sed s,i[3456789]86,ia32,)

BOOT_SRCFILES       := $(shell find src/bootloader -name '*.c')
BOOT_OBJS            = $(BOOT_SRCFILES:.c=.o)
BOOT_TARGET          = $(BOOT_SRCFILES:.c=.efi)

KERNEL_SRCFILES     := $(shell find src/kernel -name '*.c')
KERNEL_OBJS          = $(KERNEL_SRCFILES:.c=.o)
KERNEL_TARGET        = $(KERNEL_SRCFILES:.c=.efi)

EFILIB          = ./lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

INPROGRESS      = no

CFLAGS          = -Isrc/include -fno-stack-protector -fpic \
		  		  -fshort-wchar -mno-red-zone -Wall
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

ifeq ($(INPROGRESS),yes)
  CFLAGS += -DINPROGRESS
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
				  -Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS) 

all: src/boot.efi src/kernel.efi

recom: all iso run
recomwsl: all iso runwsl

%.o: %.c
	gcc $(CFLAGS) -c $^ -o $@

src/boot.so: $(BOOT_OBJS)
	ld $(LDFLAGS) $(BOOT_OBJS) -o $@ -lefi -lgnuefi

src/boot.efi: src/boot.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
	rm -rf src/bootloader/*.o
	rm -rf src/boot.so

src/kernel.so: ${KERNEL_OBJS}
	ld $(LDFLAGS) $(KERNEL_OBJS) -o $@ -lefi -lgnuefi

src/kernel.efi: src/kernel.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
	rm -rf src/kernel/*.o
	rm -rf src/kernel.so

install:
	sudo apt install -y gcc
	sudo apt install -y binutils
	sudo apt install -y iat

iso:
	@rm -rf dist
	@mkdir -p dist/iso/EFI/Boot/
	cp src/boot.efi dist/iso/EFI/Boot/boot.efi
	cp src/kernel.efi dist/iso/kernel.efi
	cp src/*.nsh dist/iso/
	dd if=/dev/zero of=dist/Mapple.img bs=1M count=200
	mformat -i dist/Mapple.img ::
	mcopy -si dist/Mapple.img dist/iso/* ::
#	mdir -i dist/Mapple.img ::
#   mdir just shows inside Mapple.img

clean:
	@rm -rf src/*.o src/*.efi src/*.so
	@rm -rf dist

run:
	qemu-system-x86_64 -drive file=dist/Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"
runwsl:
	qemu-system-x86_64.exe -drive file=dist/Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"