BIN     := bin
SRC     := src
INCLUDE := ${SRC}/include
CONFIG  := config
CROSS   := ~/softwares/i686-elf-tools/bin

CXX       := ${CROSS}/i686-elf-g++
CXX_FLAGS := -c -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -g -O0 -I ${INCLUDE} -I ${INCLUDE}/kernel/standard

ASM       := nasm
ASM_FLAGS := -felf32 -g -F dwarf -O0 -w-number-overflow

LINKER       := ${CROSS}/i686-elf-gcc
LINKER_FLAGS := -ffreestanding -O2 -nostdlib -lgcc

VM             := qemu-system-i386
VM_FLAGS       :=
VM_DEBUG_FLAGS := -s -S

SOURCES_ASM := $(shell find ${SRC} -type f -name '*.asm')
SOURCES_CPP := $(shell find ${SRC} -type f -name '*.cpp')

OBJECTS_ASM := $(patsubst %.asm,%.asm.o,${SOURCES_ASM})
OBJECTS_CPP := $(patsubst %.cpp,%.cpp.o,${SOURCES_CPP})

# Runtime OBJ's
CRTI_OBJ := ${SRC}/runtime/crti.o
CRTN_OBJ := ${SRC}/runtime/crtn.o
CRTBEGIN_OBJ:=$(shell $(CXX) $(CXX_FLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CXX) $(CXX_FLAGS) -print-file-name=crtend.o)

# All the object files
OBJ := $(CRTI_OBJ) $(CRTBEGIN_OBJ) ${OBJECTS_ASM} ${OBJECTS_CPP} $(CRTEND_OBJ) $(CRTN_OBJ)

KERNEL := ${BIN}/kernel.bin
ISO    := os.iso

all: ${ISO}

run: ${ISO}
	${VM} ${VM_FLAGS} -cdrom $<

debug: ${ISO}
	${VM} ${VM_FLAGS} ${VM_DEBUG_FLAGS} -cdrom $< &

${ISO}: ${KERNEL} ${CONFIG}/grub.cfg
	mkdir -p isodir/boot/grub
	cp ${KERNEL} isodir/boot/kernel.bin
	cp ${CONFIG}/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir

${KERNEL}: ${OBJ}
	mkdir -p ${BIN}
	${LINKER} ${LINKER_FLAGS} -T ${CONFIG}/linker.ld -o $@ $^

%.asm.o: %.asm
	${ASM} ${ASM_FLAGS} $^ -o $@

%.o: %.s
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
	find . -type f -name '*.o' -delete
	rm -f ${ISO}
	clear