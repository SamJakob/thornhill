#include "io.hpp"

/**
 * @brief  Converts a BCD (binary-coded-decimal) number to system binary.
 * @note   This is intended for reading time values from the CMOS.
 * @param  bcd: The BCD number to convert to system binary.
 * @retval The binary form of the BCD number.
 */
static uint8_t _bcdToBinary(uint8_t bcd) {
    return ((bcd & 0xF0) >> 1) + ( (bcd & 0xF0) >> 3) + (bcd & 0xf);
}

uint8_t ThornhillIO::readByteFromPort(uint16_t port) {
    uint8_t result;

    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void ThornhillIO::writeByteToPort (uint16_t port, uint8_t data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t ThornhillIO::readWordFromPort (uint16_t port) {
    uint16_t result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void ThornhillIO::writeWordToPort (uint16_t port, uint16_t data) {
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

uint8_t ThornhillIO::readCMOSRegister (uint16_t cmosRegister, bool isBCDEncodedBinaryDecimal) {

    const bool NMIDisableBit = 0b1;

    // this is a rather disgusting way of introducing a delay after
    // selecting the CMOS register on port 0x70 - as not doing this may
    // result in reading the previously selected CMOS register from port
    // 0x71.
    for (int i = 0; i < 100; i++) 
    // perform the write.
    writeByteToPort(0x70, (NMIDisableBit << 7) | cmosRegister);

    if (isBCDEncodedBinaryDecimal) return _bcdToBinary(readByteFromPort(0x71));
    else return readByteFromPort(0x71);

}