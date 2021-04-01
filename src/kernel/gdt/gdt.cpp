#include <kernel/gdt/gdt.h>

extern "C" void install_gdt(uint32_t, uint32_t);

GDT::GDT()
{
    descriptors_count = 0;
}

void GDT::init()
{
    add_null_descriptor();
    
    // Code descriptor
    add_descriptor(0, 0xFFFFFFFF, PRIVILEGE_KERNEL | TYPE_EXECUTABLE | CODE_READ_EXECUTE);
    
    // Data descriptor
    add_descriptor(0, 0xFFFFFFFF, PRIVILEGE_KERNEL | TYPE_DATA | DATA_READ_WRITE);
}

void GDT::install()
{
    int size = sizeof(gdt_descriptor) * descriptors_count - 1;
    install_gdt((uint32_t) &gdt, size); // Calls the assembly function
}

void GDT::add_null_descriptor()
{
    gdt_descriptor descriptor;

    descriptor.limit_low = 0;
    descriptor.base_low = 0;
    descriptor.base_middle = 0;
    descriptor.flags = 0;
    descriptor.l_flags = 0;
    descriptor.base_high = 0;
    
    gdt[descriptors_count++] = descriptor;
}

void GDT::add_descriptor(uint32_t base, uint32_t limit, uint8_t flags)
{
    gdt_descriptor descriptor;

    // base
    descriptor.base_low = base & 0xFFFF; // Extract lower 16 bits
    descriptor.base_middle = (base >> 16) & 0xFF; // Extract middle 8 bits
    descriptor.base_high = (base >> 24) & 0xFF; // Extract higher 8 bits

    // limit, another flags.
    descriptor.limit_low = limit & 0xFFFF; // Extract lower 16 bits
    descriptor.l_flags = ((limit >> 16) & 0xF);
    descriptor.l_flags |=  0b11000000;

    // flags.
    descriptor.flags = flags | 0b10010000;

    gdt[descriptors_count++] = descriptor;
}