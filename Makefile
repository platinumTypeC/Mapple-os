ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

SRCFILES       := $(shell find . -name '*.c')
OBJS            = $(SRCFILES:.c=.o)
TARGET          = $(SRCFILES:.c=.efi)

EFILIB          = ./lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS          = -fno-stack-protector -fpic \
		  		  -fshort-wchar -mno-red-zone -Wall 
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
				  -Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS) 

all: $(TARGET)

recom: all iso run
recomwsl: all iso runwsl

%.o: %.c
	gcc $(CFLAGS) -c $^ -o $@

%.so: %.o
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

install:
	sudo apt install -y binutils
	sudo apt install -y gcc
	sudo apt install -y iat

iso:
	@rm -rf dist
	@mkdir -p dist/iso/EFI/Boot/
	cp src/*.nsh dist/iso/
	cp src/*.efi dist/iso/EFI/Boot/
	dd if=/dev/zero of=dist/Mapple.img bs=1M count=100
	mformat -i dist/Mapple.img ::
	mcopy -si dist/Mapple.img dist/iso/* ::
#	mdir -i dist/Mapple.img ::
#   mdir just shows inside Mapple.img

clean:
	@rm -rf src/*.efi
	@rm -rf dist

run:
	@qemu-system-x86_64 -drive file=dist/Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"
runwsl:
	@qemu-system-x86_64.exe -drive file=dist/Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"