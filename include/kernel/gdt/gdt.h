#ifndef _GDT_H
#define _GDT_H

#include <cstdint.h>

#define GDT_DESCRIPTORS_LIMIT 16 // Change if required

/* GDT Flags */
#define PRIVILEGE_USER    0b01100000
#define PRIVILEGE_KERNEL  0b00000000

#define TYPE_EXECUTABLE   0b00001000
#define TYPE_DATA         0b00000000

#define DATA_READ_ONLY    0b00000000
#define CODE_EXECUTE_ONLY 0b00000000

#define DATA_READ_WRITE   0b00000010
#define CODE_READ_EXECUTE 0b00000010

struct gdt_descriptor 
{ 
    // bits 0-15 of segment limit
    uint16_t limit_low;
 
	// bits 0-23 of base address
	uint16_t base_low;
	uint8_t	base_middle;
 
	// descriptor bit flags. Set using bit masks above
	uint8_t flags;

    // Another flags, and 4 bytes of the "limit".
    uint8_t l_flags; // l_flags == limit & flags.
 
	// bits 24-32 of base address
	uint8_t	base_high;
} __attribute__((packed));

class GDT
{
    public:
        GDT();
        void install();

    private:
        void init_descriptors();
        void add_descriptor(uint32_t base, uint32_t limit, uint8_t flags);
        void add_null_descriptor();

        gdt_descriptor gdt[GDT_DESCRIPTORS_LIMIT];
        int descriptors_count;
        
};

#endif