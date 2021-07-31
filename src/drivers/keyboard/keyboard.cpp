#include "drivers/keyboard/keyboard.h"
#include "drivers/keyboard/extended_char.h"
#include "drivers/serial_ports/serial_ports.h"
#include "kernel/interrupts/interrupts_manager.h"

#include <cstdio.h>

#define KEYBOARD_INTERRUPT_NUBMER 33

#define KE_PORT 0x60
#define KC_PORT 0x64

#define MAKE_SCAN_CODES_START 0x1
#define MAKE_SCAN_CODES_END 0x58

#define BREAK_SCAN_CODE_START (MAKE_SCAN_CODES_START | 0x80)
#define BREAK_SCAN_CODE_END (MAKE_SCAN_CODES_END | 0x80)

#define SCAN_CODE_WAIT_FOR_SECOND 0xE0
#define SCAN_CODE_WAIT_FOR_SECOND_AND_THIRD 0xE1

new_extended_char_listener Keyboard::press_listener, Keyboard::release_listener;

bool Keyboard::wait_for_second_scan_code;
bool Keyboard::wait_for_second_and_third_scan_code;

void Keyboard::initialize()
{
    kprintf("Initializing Keyboard Driver...\n");
    InterruptsManager::get_instance()->set_isr(KEYBOARD_INTERRUPT_NUBMER, Keyboard::on_ke_data);
    wait_for_second_scan_code = wait_for_second_and_third_scan_code = false;
}

void Keyboard::set_press_extended_char_listener(new_extended_char_listener l)
{
    press_listener = l;
}

void Keyboard::set_release_extended_char_listener(new_extended_char_listener l)
{
    release_listener = l;
}

void log(const char* s)
{
    while (*s) {
        SerialPorts::outb(0x3F8, *s);
        s++;
    }
}

void Keyboard::on_ke_data([[gnu::unused]] uint32_t unused)
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

bool Keyboard::ignore(uint8_t scan_code)
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

void Keyboard::on_make_scan_code(uint8_t scan_code)
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

void Keyboard::on_break_scan_code(uint8_t scan_code)
{
    if (wait_for_second_scan_code) {
        ExtendedChar ec(SCAN_CODE_WAIT_FOR_SECOND, scan_code);
        wait_for_second_scan_code = false;
        if (press_listener)
            release_listener(ec);
    } else {
        ExtendedChar ec(scan_code);
        if (press_listener)
            release_listener(ec);
    }
}

uint8_t Keyboard::ke_read_data()
{
    return SerialPorts::inb(KE_PORT);
}
