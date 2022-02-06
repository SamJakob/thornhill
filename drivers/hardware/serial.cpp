#include "serial.hpp"

#include <thornhill>
#include "../io.hpp"

using namespace Thornhill;

bool ThornhillSerialDriver::initialized = false;

bool ThornhillSerialDriver::isTransitEmpty() {
    return ThornhillIODriver::readByteFromPort(DEFAULT_SERIAL_PORT + 5) & 0x20;
}

bool ThornhillSerialDriver::initialize() {
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 1, 0x00); // Disable interrupts
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 1, 0x00); //                  (hi byte)
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14 byte threshold
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 4, 0x1E); // Set in loopback mode, test serial chip
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if same byte returned)

    // Check if serial is faulty. If so, return false to indicate error and stop
    // initialization.
    if (ThornhillIODriver::readByteFromPort(DEFAULT_SERIAL_PORT) != 0xAE) {
        return false;
    }

    // Otherwise, set serial port in normal operation mode.
    // (not-loopback, IRQs enabled, OUT#1 and OUT#2 bits enabled)
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT + 4, 0x0F);
    ThornhillSerialDriver::initialized = true;

    Kernel::debug("Serial", "Serial driver initialized successfully!");
    return true;
}

void ThornhillSerialDriver::writeCharacter(unsigned char data) {
    if (!ThornhillSerialDriver::initialized) return;

    while (isTransitEmpty() == 0);
    ThornhillIODriver::writeByteToPort(DEFAULT_SERIAL_PORT, data);
}

void ThornhillSerialDriver::write(const char* data, bool newlineChars) {
    if (!ThornhillSerialDriver::initialized) return;

    int index = 0;
    while (data[index] != 0) {
        writeCharacter(data[index]);
        index++;
    }

    if (newlineChars) {
        writeCharacter('\r');
        writeCharacter('\n');
    }
}
