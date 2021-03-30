CXX       := i686-elf-g++
CXX_FLAGS := -c -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -g -O0

ASM       := nasm
ASM_FLAGS := -felf32 -g -F dwarf -O0

LINKER       := i686-elf-gcc
LINKER_FLAGS := -ffreestanding -O2 -nostdlib -lgcc

VM             := qemu-system-i386
VM_FLAGS       :=
VM_DEBUG_FLAGS := -s -S

BIN     := bin
SRC     := src
INCLUDE := include
CONFIG  := config

SOURCES_ASM := $(shell find ${SRC} -type f -name '*.asm')
SOURCES_CPP := $(shell find ${SRC} -type f -name '*.cpp')

OBJECTS_ASM := $(patsubst %.asm,%.asm.o,${SOURCES_ASM})
OBJECTS_CPP := $(patsubst %.cpp,%.cpp.o,${SOURCES_CPP})

KERNEL := ${BIN}/kernel.bin
ISO    := os.iso

all: ${ISO}

run: ${ISO}
	${VM} ${VM_FLAGS} -cdrom $<

debug: ${ISO}
	${VM} ${VM_FLAGS} ${VM_DEBUG_FLAGS} -cdrom $<

${ISO}: ${KERNEL} ${CONFIG}/grub.cfg
	mkdir -p isodir/boot/grub
	cp ${KERNEL} isodir/boot/kernel.bin
	cp ${CONFIG}/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir

${KERNEL}: ${OBJECTS_ASM} ${OBJECTS_CPP}
	${LINKER} ${LINKER_FLAGS} -T ${CONFIG}/linker.ld -o $@ $^

%.asm.o: %.asm
	${ASM} ${ASM_FLAGS} $^ -o $@

%.cpp.o: %.cpp
	${CXX} ${CXX_FLAGS} $^ -o $@

validate: ${KERNEL}
	@if grub-file --is-x86-multiboot ${KERNEL}; then \
  		echo multiboot confirmed!; \
	else \
		echo the file is not multiboot; \
	fi

clean:
	rm -f ${BIN}/*
	rm -rf isodir
	find . -type f -name '*.cpp.o' -delete
	find . -type f -name '*.asm.o' -delete
	rm ${ISO}
	clear