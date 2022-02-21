KERNEL_SRCFILES         := $(shell find . -name "*.cpp")
KERNEL_OBJS             := $(KERNEL_SRCFILES:.cpp=.o)

KERNEL_LDFLAGS  		 = -nostdlib -z max-page-size=0x1000

KERNEL_LIBS    			:= -lgcc

CFLAGS          		 = -Iinclude -fno-stack-protector -fpic \
		  		  		   -fshort-wchar -mno-red-zone -Wall

CC              		 = g++

all: ../kernel.elf
	@echo done compiling kernel.

%.o: %.cpp $(shell find . -name "*.h")
	${CC} ${CFLAGS} -c $< -o $@;

../kernel.elf: ${KERNEL_OBJS}
	${CC} -T kernel.ld -g3 -o $@ ${KERNEL_LDFLAGS} $^ ${KERNEL_LIBS}