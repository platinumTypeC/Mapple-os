KERNEL_SRCFILES         := $(shell find . -name "*.c")
KERNEL_OBJS              = $(KERNEL_SRCFILES:.c=.o)

KERNEL_LDFLAGS  = -ffreestanding -O2 -nostdlib -z max-page-size=0x1000

KERNEL_LIBS    := -lgcc

CFLAGS          		 = -Iinclude -fno-stack-protector -fpic \
		  		  		   -fshort-wchar -mno-red-zone -Wall

all: ../kernel.elf
	@echo done compiling kernel.

%.o: %.c $(shell find . -name "*.h")
	gcc ${CFLAGS} -c $< -o $@;

../kernel.elf: ${KERNEL_OBJS}
	gcc -T kernel.ld -g3 -o $@ ${KERNEL_LDFLAGS} ${KERNEL_OBJS} ${KERNEL_LIBS}