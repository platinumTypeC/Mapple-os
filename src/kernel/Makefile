KERNEL_SRCFILES         := $(shell find . -name "*.cpp")
KERNEL_ASMSRC           := $(shell find . -name "*.asm")
KERNEL_ASMBIN           := $(KERNEL_ASMSRC:.asm=_asm.o)

KERNEL_OBJS             := $(KERNEL_SRCFILES:.cpp=.o)

KERNEL_LDFLAGS  		 = -nostdlib -z max-page-size=0x1000

KERNEL_LIBS    			:= -lgcc

CFLAGS          		 =  -Iinclude -fno-stack-protector -fpic \
		  		  		   	-fshort-wchar -mno-red-zone -Wall -fno-exceptions
CC              		 = g++

all: ../kernel.elf
	@echo done compiling kernel.

Memory/DT/Interrupts.o: Memory/DT/Interrupts.cpp
	${CC} ${CFLAGS} -mgeneral-regs-only -c $< -o $@

%.o: %.cpp $(shell find . -name "*.h**")
	${CC} ${CFLAGS} -c $< -o $@;

%_asm.o: %.asm
	nasm $^ -f elf64 -o $@

../kernel.elf: ${KERNEL_OBJS} ${KERNEL_ASMBIN}
	${CC} -T kernel.ld -g3 -o $@ ${KERNEL_LDFLAGS} $^ ${KERNEL_LIBS}