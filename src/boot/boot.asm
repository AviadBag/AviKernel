; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0                      ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1                      ; provide memory map
VIDINFO  equ  1 << 2                      ; we will be passing information about current video mode
FLAGS    equ  MBALIGN | MEMINFO | VIDINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002                  ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)             ; checksum of above, to prove we are multiboot

; Video settings
VID_MODTYPE equ 0    ; Linear graphics mode
VID_WIDTH   equ 1024 ; Prefered resolution of 1024x768
VID_HEIGHT  equ 768
VID_DEPTH   equ 0    ; Number of bits per pixel, no preference

KERNEL_VIRTUAL_BASE_ADDR equ 0xC0000000
KERNEL_VIRTUAL_BASE_PAGE equ (KERNEL_VIRTUAL_BASE_ADDR >> 22)

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	times 5 dd 0 ; We do not provide header address fields
	dd VID_MODTYPE
	dd VID_WIDTH
	dd VID_HEIGHT
	dd VID_DEPTH

 
; The multiboot standard does not define the value of the stack pointer register
; (esp) and it is up to the kernel to provide a stack. This allocates room for a
; small stack by creating a symbol at the bottom of it, then allocating 16384
; bytes for it, and finally creating a symbol at the top. The stack grows
; downwards on x86. The stack is in its own section so it can be marked nobits,
; which means the kernel file is smaller because it does not contain an
; uninitialized stack. The stack on x86 must be 16-byte aligned according to the
; System V ABI standard and de-facto extensions. The compiler will assume the
; stack is properly aligned and failure to align the stack will result in
; undefined behavior.
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

section .data:
align 0x1000
temp_page_directory:
	dd 00000000000000000000000010000011b ; 4 mb page, read-write, present. Virtual first 4 mb -> first 4 mb.
	times (KERNEL_VIRTUAL_BASE_PAGE - 1) dd 0
	dd 00000000000000000000000010000011b ; 4 mb page, read-write, present. Virtual kernel first 4 mb -> first 4 mb.
	times (1024 - KERNEL_VIRTUAL_BASE_PAGE - 1) dd 0 ; The rest of the pages.


; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.
section .text
load_os:
	cli ; The OS will treat it.
	
	push ebx ; Multiboot Data

	extern _init
	call _init ; C++ global constructors
	
	extern kernel_main
	call kernel_main
	
	extern _fini
	call _fini ; C++ global destructors

	; If the system has nothing more to do, put the computer into an
	; infinite loop. To do that:
	; 1) Disable interrupts with cli (clear interrupt enable in eflags).
	;    They are already disabled by the bootloader, so this is not needed.
	;    Mind that you might later enable interrupts and return from
	;    kernel_main (which is sort of nonsensical to do).
	; 2) Wait for the next interrupt to arrive with hlt (halt instruction).
	;    Since they are disabled, this will lock up the computer.
	; 3) Jump to the hlt instruction if it ever wakes up due to a
	;    non-maskable interrupt occurring or due to system management mode.
	cli
.hang:	hlt
	jmp .hang

global _start:function (_start.end - _start)
_start:
	; The bootloader has loaded us into 32-bit protected mode on a x86
	; machine. Interrupts are disabled. Paging is disabled. The processor
	; state is as defined in the multiboot standard. The kernel has full
	; control of the CPU. The kernel can only make use of hardware features
	; and any code it provides as part of itself. There's no printf
	; function, unless the kernel provides its own <stdio.h> header and a
	; printf implementation. There are no security restrictions, no
	; safeguards, no debugging mechanisms, only what the kernel provides
	; itself. It has absolute and complete power over the
	; machine.
 
	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems). This is necessarily done
	; in assembly as languages such as C cannot function without a stack.
	mov esp, stack_top
 
	; This is a good place to initialize crucial processor state before the
	; high-level kernel is entered. It's best to minimize the early
	; environment where crucial features are offline. Note that the
	; processor is not fully initialized yet: Features such as floating
	; point instructions and instruction set extensions are not initialized
	; yet. The GDT should be loaded here. Paging should be enabled here.
	; C++ features such as global constructors and exceptions will require
	; runtime support to work as well.
 
	; Enter the high-level kernel. The ABI requires the stack is 16-byte
	; aligned at the time of the call instruction (which afterwards pushes
	; the return pointer of size 4 bytes). The stack was originally 16-byte
	; aligned above and we've since pushed a multiple of 16 bytes to the
	; stack since (pushed 0 bytes so far) and the alignment is thus
	; preserved and the call is well defined.
    ; note, that if you are building on Windows, C functions may have "_" prefix in assembly: _kernel_main
	
	; Enable PSE
	mov eax, cr4
 	or eax, 0x00000010
 	mov cr4, eax

	; Enable paging
	mov eax, (temp_page_directory - KERNEL_VIRTUAL_BASE_ADDR)
	mov cr3, eax
	mov eax, cr0
 	or eax, 0x80000001
 	mov cr0, eax

	; Far jump to 3GB + 1MB.
	lea ecx, [load_os]
	jmp ecx

.end: