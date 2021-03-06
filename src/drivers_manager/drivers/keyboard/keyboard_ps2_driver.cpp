#include "drivers_manager/drivers/keyboard/keyboard_ps2_driver.h"
#include "kernel/interrupts/interrupts_manager.h"
#include "utils/io.h"

#define PS2_INTERRUPT_NUBMER 33

#define KE_PORT 0x60 // Keyboard Encoder
#define KC_PORT 0x64 // Keyboard Controller

#define MAKE_SCAN_CODES_START 0x1
#define MAKE_SCAN_CODES_END 0x58

#define BREAK_SCAN_CODE_START (MAKE_SCAN_CODES_START | 0x80)
#define BREAK_SCAN_CODE_END (MAKE_SCAN_CODES_END | 0x80)

#define SCAN_CODE_WAIT_FOR_SECOND 0xE0
#define SCAN_CODE_WAIT_FOR_SECOND_AND_THIRD 0xE1

void KeyboardPS2Driver::setup_driver_and_device()
{
    // Register the IRQ handler.
    InterruptsManager::get_instance()->set_isr(
        PS2_INTERRUPT_NUBMER, [](void* context, [[gnu::unused]] uint32_t unused) {
            ((KeyboardPS2Driver*)context)->on_ke_data();
        },
        this);
}

void KeyboardPS2Driver::on_ke_data()
{
    uint8_t data = ke_read_data();
    bool is_break = false;

    if (data == SCAN_CODE_WAIT_FOR_SECOND) {
        wait_for_second_scan_code = true;
        return; /* No need more checking */
    }
    if (data == SCAN_CODE_WAIT_FOR_SECOND_AND_THIRD) {
        wait_for_second_and_third_scan_code = true;
        return; /* No need more checking */
    }

    if (ignore(data))
        return;

    // Is it a break scan code?
    if (data >= BREAK_SCAN_CODE_START && data <= BREAK_SCAN_CODE_END) {
        is_break = true;
        data &= ~0x80; // Convert it to a regular scan code.
    }

    // Is it a scan code?
    if ((data >= MAKE_SCAN_CODES_START && data <= MAKE_SCAN_CODES_END) || data == SCAN_CODE_WAIT_FOR_SECOND) {
        if (is_break)
            on_break_scan_code(data);
        else
            on_make_scan_code(data);
    }
}

bool KeyboardPS2Driver::ignore(uint8_t scan_code)
{
    // Ignore Print Screen
    if (wait_for_second_scan_code && (scan_code == 0x2A || scan_code == 0xB7))
        return true;
    if (wait_for_second_scan_code && (scan_code == 0x37 || scan_code == 0xAA)) {
        wait_for_second_scan_code = false;
        return true;
    }

    // Ignore PAUSE
    if (wait_for_second_and_third_scan_code && (scan_code == 0x1D || scan_code == 0x9D))
        return true;
    if (wait_for_second_and_third_scan_code && (scan_code == 0x45 || scan_code == 0xC5)) {
        wait_for_second_and_third_scan_code = false;
        return true;
    }

    return false;
}

void KeyboardPS2Driver::on_make_scan_code(uint8_t scan_code)
{
    if (wait_for_second_scan_code) {
        ExtendedChar ec(SCAN_CODE_WAIT_FOR_SECOND, scan_code);
        wait_for_second_scan_code = false;
        if (press_listener)
            press_listener(ec);
    } else {
        ExtendedChar ec(scan_code);
        if (press_listener)
            press_listener(ec);
    }
}

void KeyboardPS2Driver::on_break_scan_code(uint8_t scan_code)
{
    if (wait_for_second_scan_code) {
        ExtendedChar ec(SCAN_CODE_WAIT_FOR_SECOND, scan_code);
        wait_for_second_scan_code = false;
        if (release_listener)
            release_listener(ec);
    } else {
        ExtendedChar ec(scan_code);
        if (release_listener)
            release_listener(ec);
    }
}

uint8_t KeyboardPS2Driver::ke_read_data()
{
    return IO::inb(KE_PORT);
}

bool KeyboardPS2Driver::exist()
{
    // TODO: Implement it!
    return true;
}
