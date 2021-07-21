BIN     := bin
SRC     := src
INCLUDE := include
CONFIG  := config
CROSS   := ~/files/programming/softwares/i686-elf/bin

HARD_DRIVA_IMAGE := drive.img

CXX       := ${CROSS}/i686-elf-g++
CXX_FLAGS := -c -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-sized-deallocation -fno-rtti -g -O0 -I ${INCLUDE} -I ${INCLUDE}/standard

ASM       := nasm
ASM_FLAGS := -felf32 -g -F dwarf -O0 -w-number-overflow

LINKER       := ${CROSS}/i686-elf-gcc
LINKER_FLAGS := -ffreestanding -O2 -nostdlib -lgcc 

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

VM             := qemu-system-i386
VM_FLAGS       := -curses -device piix3-ide,id=ide -drive id=disk,file=${ISO},format=raw,if=none -device ide-hd,drive=disk,bus=ide.0
VM_DEBUG_FLAGS := -s -S

all: ${ISO}

run: ${ISO}
	${VM} ${VM_FLAGS}

debug: ${ISO}
	${VM} ${VM_FLAGS} ${VM_DEBUG_FLAGS}

${ISO}: ${KERNEL} ${CONFIG}/grub.cfg
	@mkdir -p isodir/boot/grub
	@cp ${KERNEL} isodir/boot/kernel.bin
	@cp ${CONFIG}/grub.cfg isodir/boot/grub/grub.cfg
	$(info Creating iso "$(ISO)"...)
	@grub-mkrescue -o $@ isodir 

${KERNEL}: ${OBJ}
	@mkdir -p ${BIN}
	@# Caution: ${LINKER_FLAGS} MUST be at the end of the line!
	@echo "Running Linker..."
	@${LINKER} -T ${CONFIG}/linker.ld -o $@ $^ ${LINKER_FLAGS}

%.asm.o: %.asm
	@${ASM} ${ASM_FLAGS} $^ -o $@
	@echo ASM $@

%.o: %.s
	@${ASM} ${ASM_FLAGS} $^ -o $@
	@echo ASM $@

%.cpp.o: %.cpp
	@${CXX} ${CXX_FLAGS} $^ -o $@
	@echo CXX $@

validate: ${KERNEL}
	@if grub-file --is-x86-multiboot ${KERNEL}; then \
  		echo multiboot confirmed!; \
	else \
		echo the file is not multiboot; \
	fi

compiledb: clean
	compiledb make

format:
	find ${SRC} -iname *.h -o -iname *.cpp | xargs clang-format -i --style=Webkit
	find ${INCLUDE} -iname *.h -o -iname *.cpp | xargs clang-format -i --style=Webkit

clean:
	rm -f ${BIN}/*
	rm -rf isodir
	find . -type f -name '*.o' -delete
	rm -f ${ISO}
	clear
