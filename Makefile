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

%.o: %.c
	gcc $(CFLAGS) -c $^ -o $@

%.so: %.o
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

install:
	@sudo apt install -y binutils
	@sudo apt install -y gcc
	@sudo apt install -y ccd2iso
	@sudo apt install -y genisoimage

setup:
	@mkdir -p dist
	@mkdir -p dist/iso

iso: setup
	cp src/main.efi dist/iso/
	dd if=/dev/zero of=dist/iso/Mapple.img bs=1M count=50
	mformat -i dist/iso/Mapple.img ::
	mcopy -i dist/iso/Mapple.img dist/iso/main.efi ::
#   mdir just prints its contents if you would like to have a look:
#
#   mdir -i dist/iso/Mapple.img ::
#

createImage: dist/iso/Mapple.img
	dd if=/dev/zero of=dist/iso/Mapple.img bs=1M count=50

clean:
	rm -rf *.img *.o *.so *.iso *.efi
	rm -rf dist

run:
	qemu-system-x86_64 -m 1024 -drive file=dist/iso/Mapple.img,format=raw
runwsl:
	qemu-system-x86_64.exe -m 1024 -drive file=dist/iso/Mapple.img,format=raw