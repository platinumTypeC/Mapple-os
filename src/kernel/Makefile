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
ECHO					 = echo "`expr " [\`expr $C '*' 1000 / $T\`" : '.*\(....\)$$'`%]"
endif

KERNEL_SRCFILES         := $(shell find . -name "*.cpp")
DEVICE_DRIVER           := $(shell find ../Drivers/Device -name "*.cpp")
KERNEL_ASMSRC           := $(shell find . -name "*.asm")

KERNEL_OBJS             := $(KERNEL_SRCFILES:.cpp=.o)
DEVICE_DRIVER_OBJ       := $(DEVICE_DRIVER:.cpp=.o)
KERNEL_ASMBIN           := $(KERNEL_ASMSRC:.asm=_asm.o)

KERNEL_LDFLAGS  		 = -nostdlib -z max-page-size=0x1000

KERNEL_LIBS    			:= -lgcc

CFLAGS          		 =  -Iinclude -fno-stack-protector -fpic \
		  		  		   	-fshort-wchar -mno-red-zone -Wall -fno-exceptions
CC              		 = g++

all: ../kernel.elf
	@${ECHO} done compiling kernel.

Memory/DT/Interrupts.o: Memory/DT/Interrupts.cpp
	@${ECHO} $@
	@${CC} ${CFLAGS} -mgeneral-regs-only -c $< -o $@

%.o: %.cpp $(shell find . -name "*.h**")
	@${ECHO} $@
	@${CC} ${CFLAGS} -c $< -o $@;

%_asm.o: %.asm
	@${ECHO} $@
	@nasm $^ -f elf64 -o $@

../kernel.elf: ${KERNEL_OBJS} ${KERNEL_ASMBIN} ${DEVICE_DRIVER_OBJ}
	@${ECHO} Linking
	@${CC} -T kernel.ld -g3 -o $@ ${KERNEL_LDFLAGS} $^ ${KERNEL_LIBS}
endif