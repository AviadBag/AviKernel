#include "drivers_manager/drivers/storage/ide/ide_controller.h"
#include "utils/io.h"

IDEController::IDEController(uint16_t ports1_base, uint16_t ports2_base)
{
    this->ports1_base = ports1_base;
    this->ports2_base = ports2_base;
}

uint16_t IDEController::read_data_register()
{
    return IO::inw(ports1_base + 0);
}

uint8_t IDEController::read_error_register()
{
    return IO::inb(ports1_base + 1);
}

uint8_t IDEController::read_sector_count_register()
{
    return IO::inb(ports1_base + 2);
}

uint8_t IDEController::read_sector_number_register()
{
    return IO::inb(ports1_base + 3);
}

uint8_t IDEController::read_cylinder_LSB_register()
{
    return IO::inb(ports1_base + 4);
}

uint8_t IDEController::read_cylinder_MSB_register()
{
    return IO::inb(ports1_base + 5);
}

uint8_t IDEController::read_drive_head_register()
{
    return IO::inb(ports1_base + 6);
}

uint8_t IDEController::read_status_register()
{
    return IO::inb(ports1_base + 7);
}

uint8_t IDEController::read_alternate_status_register()
{
    return IO::inb(ports2_base + 0);
}

uint8_t IDEController::read_driver_address_register()
{
    return IO::inb(ports2_base + 1);
}

void IDEController::read_data_register_buffer(uint16_t* buf, unsigned int size)
{
    unsigned int count = size / sizeof(uint16_t);
    for (unsigned int i = 0; i < count; i++)
        buf[i] = read_data_register();
}

void IDEController::write_data_register_buffer(uint16_t* buf, unsigned int size)
{
    unsigned int count = size / sizeof(uint16_t);
    for (unsigned int i = 0; i < count; i++)
        write_data_register(buf[i]);
}

void IDEController::write_data_register(uint16_t data)
{
    IO::outw(ports1_base + 0, data);
}

void IDEController::write_precompensation_register(uint8_t data)
{
    IO::outb(ports1_base + 1, data);
}

void IDEController::write_sector_count_register(uint8_t data)
{
    IO::outb(ports1_base + 2, data);
}

void IDEController::write_sector_number_register(uint8_t data)
{
    IO::outb(ports1_base + 3, data);
}

void IDEController::write_cylinder_LSB_register(uint8_t data)
{
    IO::outb(ports1_base + 4, data);
}

void IDEController::write_cylinder_MSB_register(uint8_t data)
{
    IO::outb(ports1_base + 5, data);
}

void IDEController::write_drive_head_register(uint8_t data)
{
    IO::outb(ports1_base + 6, data);
}

void IDEController::write_command_register(uint8_t data)
{
    IO::outb(ports1_base + 7, data);
}

void IDEController::write_digital_output_register(uint8_t data)
{
    IO::outb(ports2_base + 0, data);
}

void IDEController::write_precompensation_register_48(uint8_t low, uint8_t high)
{
    IO::outb(ports1_base + 1, high);
    IO::outb(ports1_base + 1, low);
}

void IDEController::write_sector_count_register_48(uint8_t low, uint8_t high)
{
    IO::outb(ports1_base + 2, high);
    IO::outb(ports1_base + 2, low);
}

void IDEController::write_LBAlo_register_48(uint8_t low, uint8_t high)
{
    IO::outb(ports1_base + 3, high);
    IO::outb(ports1_base + 3, low);
}

void IDEController::write_LBAmid_register_48(uint8_t low, uint8_t high)
{
    IO::outb(ports1_base + 4, high);
    IO::outb(ports1_base + 4, low);
}

void IDEController::write_LBIhi_register_48(uint8_t low, uint8_t high)
{
    IO::outb(ports1_base + 5, high);
    IO::outb(ports1_base + 5, low);
}
