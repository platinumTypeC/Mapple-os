ARCH                := $(shell uname -m | sed s,i[3456789]86,ia32,)

SRCFILES     := $(shell find src/kernel -name '*.c')
OBJS          = $(SRCFILES:.c=.o)

EFILIB          = ./lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS          = -Isrc/include -fno-stack-protector -fpic \
		  		  -fshort-wchar -mno-red-zone -Wall
ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
				  -Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS) 

compile: src/kernel.efi

linux: compile iso run
wsl: compile iso runwsl

%.o: %.c
	gcc ${CFLAGS} -c $^ -o $@

src/kernel.so: ${OBJS}
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

src/kernel.efi: src/kernel.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
	rm src/kernel.so

install:
	sudo apt install -y gcc
	sudo apt install -y binutils
	sudo apt install -y iat

iso:
	dd if=/dev/zero of=Mapple.img bs=1M count=500
	mformat -i Mapple.img ::
	mcopy -si Mapple.img data/* ::
	mcopy -i Mapple.img src/startup.nsh ::
	mcopy -i Mapple.img src/kernel.efi ::

clean:
	@rm $(shell find src -name '*.o') src/kernel.so src/kernel.efi

run:
	qemu-system-x86_64 -drive file=Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"
runwsl:
	qemu-system-x86_64.exe -drive file=Mapple.img,format=raw -m 100M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd"