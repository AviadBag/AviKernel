BIN     := bin
SRC     := src
INCLUDE := include
CONFIG  := config
CROSS   := /opt/cross/bin

CXX       := ${CROSS}/i686-elf-g++
CXX_FLAGS := -c -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-sized-deallocation -fno-rtti -g -O0 -I ${INCLUDE} -I ${INCLUDE}/libc -I ${INCLUDE}/libc++
ifdef test
	CXX_FLAGS += -DTEST
endif

ASM       := nasm
ASM_FLAGS := -felf32 -g -F dwarf -O0 -w-number-overflow

LINKER       := ${CROSS}/i686-elf-gcc
LINKER_FLAGS := -ffreestanding -O2 -nostdlib -lgcc 

SOURCES_ASM := $(shell find ${SRC} -type f -name '*.asm')
SOURCES_CPP := $(shell find ${SRC} -type f -name '*.cpp')

OBJECTS_ASM := $(patsubst %.asm,%.asm.ob,${SOURCES_ASM})
OBJECTS_CPP := $(patsubst %.cpp,%.cpp.ob,${SOURCES_CPP})

# Runtime OBJ's
CRTI_OBJ := ${SRC}/runtime/crti.ob
CRTN_OBJ := ${SRC}/runtime/crtn.ob
CRTBEGIN_OBJ:=$(shell $(CXX) $(CXX_FLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CXX) $(CXX_FLAGS) -print-file-name=crtend.o)

# All the object files
OBJ := $(CRTI_OBJ) $(CRTBEGIN_OBJ) ${OBJECTS_ASM} ${OBJECTS_CPP} $(CRTEND_OBJ) $(CRTN_OBJ)

KERNEL := ${BIN}/kernel.bin
ISO    := os.iso

VM             := qemu-system-i386
VM_FLAGS       := ${ISO} -curses
VM_DEBUG_FLAGS := -s -S

all: ${ISO}

run: ${ISO}
	@${VM} ${VM_FLAGS}
	@echo "QEMU        $^"

debug: ${ISO}
	@${VM} ${VM_FLAGS} ${VM_DEBUG_FLAGS}
	@echo "QEMU        $^"

${ISO}: ${KERNEL} ${CONFIG}/grub.cfg
	@mkdir -p isodir/boot/grub
	@cp ${KERNEL} isodir/boot/kernel.bin
	@cp ${CONFIG}/grub.cfg isodir/boot/grub/grub.cfg
	@echo "PACKAGE     ${KERNEL}"
	@grub-mkrescue -o $@ isodir > /dev/null 2>&1

# Caution: ${LINKER_FLAGS} MUST be at the end of the line!
${KERNEL}: ${OBJ}
	@mkdir -p ${BIN}
	@echo "LD          *.ob"
	@${LINKER} -T ${CONFIG}/linker.ld -o $@ $^ ${LINKER_FLAGS}

%.asm.ob: %.asm
	@echo "ASM         $^"
	@${ASM} ${ASM_FLAGS} $^ -o $@

%.ob: %.s
	@echo "ASM         $^"
	@${ASM} ${ASM_FLAGS} $^ -o $@

%.cpp.ob: %.cpp
	@echo "CXX         $^"
	@${CXX} ${CXX_FLAGS} $^ -o $@

validate: ${KERNEL}
	@if grub-file --is-x86-multiboot ${KERNEL}; then \
  		echo Multiboot confirmed!; \
	else \
		echo The file is not multiboot; \
	fi

compiledb: clean
	compiledb make

format:
	@echo -n Applying Prettier on src files...
	@find ${SRC} -iname *.h -o -iname *.cpp | xargs clang-format -i --style=Webkit
	@echo " Done!"

	@echo -n Applying Prettier on header files...
	@find ${INCLUDE} -iname *.h -o -iname *.cpp | xargs clang-format -i --style=Webkit
	@echo " Done!"

clean:
	@echo "RM ${BIN}/"
	@rm -f ${BIN}/*
	@echo "RM isodir"
	@echo "RM *.ob"
	@find . -type f -name '*.ob' -delete
	@echo "RM ${ISO}"
	@rm -f ${ISO}

