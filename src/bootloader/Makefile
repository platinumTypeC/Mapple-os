ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL = all
%:
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else
ifndef ECHO
T 						:= $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
							-nrRf $(firstword $(MAKEFILE_LIST)) \
							ECHO="COUNTTHIS" | grep -c "COUNTTHIS")

N 						:= x
C 						 = $(words $N)$(eval N := x $N)
ECHO					 = echo "`expr " [\`expr $C '*' 100 / $T\`" : '.*\(....\)$$'`%]"
endif

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
	@${ECHO} done compiling bootloader.

%.o: %.c $(shell find . -name "*.h") ../kernel/include/mapple/config.h ../kernel/include/mapple/types.h
	@${ECHO} Compiling $@
	@gcc ${CFLAGS} -c $< -o $@

../boot.so: ${BOOTLOADER_OBJS}
	@${ECHO} Linking $@	
	@ld $(LDFLAGS) $(BOOTLOADER_OBJS) -o $@ -lefi -lgnuefi

../boot.efi: ../boot.so
	@${ECHO} Object Copy $@
	@objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
endif