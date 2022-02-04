ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

SRCFILES       := $(shell find . -name '*.c')
OBJS            = $(SRCFILES:.c=.o)
TARGET          = $(SRCFILES:.c=.efi)

EFILIB          = /usr/local/lib
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

iso:
