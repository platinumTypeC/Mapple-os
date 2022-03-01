CC                       = gcc

ARCH                    := $(shell uname -m | sed s,i[3456789]86,ia32,)

BOOTLOADER_SRCFILES     := $(shell find . -name '*.c')
BOOTLOADER_OBJS          = $(BOOTLOADER_SRCFILES:.c=.o)

EFILIB          		 = ../../efilib
EFI_CRT_OBJS    		 = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         		 = $(EFILIB)/elf_$(ARCH)_efi.lds


ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
				  -Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS) 

CFLAGS          		 = -I../kernel/include -Iefinclude -I. -fno-stack-protector -fpic \
		  		  		   -fshort-wchar -mno-red-zone -Wall

all: ../boot.efi
	@echo done compiling bootloader.

%.o: %.c $(shell find . -name "*.h") ../kernel/include/mapple/config.h ../kernel/include/mapple/types.h
	gcc ${CFLAGS} -c $< -o $@

../boot.so: ${BOOTLOADER_OBJS}
	ld $(LDFLAGS) $(BOOTLOADER_OBJS) -o $@ -lefi -lgnuefi

../boot.efi: ../boot.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@