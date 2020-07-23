#ifndef TH_DRIVER_IO
#define TH_DRIVER_IO

class ThornhillIO {

    private:
        /**
         * @brief  Converts a BCD (binary-coded-decimal) number to system binary.
         * @note   This is intended for reading time values from the CMOS.
         * @param  bcd: The BCD number to convert to system binary.
         * @retval The binary form of the BCD number.
         */
        static uint8_t _bcdToBinary(uint8_t bcd) {
            return ((bcd & 0xF0) >> 1) + ( (bcd & 0xF0) >> 3) + (bcd & 0xf);
        }

    public:
        /**
         * @brief  Reads a single byte from an IO port.
         * @param  port: The IO port to read data from.
         * @retval The byte that was read.
         */
        static uint8_t readByteFromPort (uint16_t port) {
            uint8_t result;

            __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
            return result;
        }

        /**
         * @brief  Writes a single byte to an IO port.
         * @param  port: The IO port to write to.
         * @param  data: The byte to write to the port.
         * @retval None
         */
        static void writeByteToPort (uint16_t port, uint8_t data) {
            __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
        }

        /**
         * @brief  Reads a WORD (two bytes) from an IO port.
         * @param  port: The IO port to read data from.
         * @retval The bytes that were read (in the form of a short).
         */
        static uint16_t readWordFromPort (uint16_t port) {
            uint16_t result;
            __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
            return result;
        }

        /**
         * @brief  Writes a WORD (two bytes) to an IO port.
         * @param  port: The IO port to write to.
         * @param  data: The bytes to write to the port (in the form of a short).
         * @retval None
         */
        static void writeWordToPort (uint16_t port, uint16_t data) {
            __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
        }

        /**
         * @brief  Reads a value from the CMOS register.
         * @note   If the value is a binary-coded decimal, setting isBCDEncodedBinaryDecimal to true
         *         will cause the value to be automatically decoded.
         * @param  cmosRegister: The register to obtain a value from.
         * @param  isBCDEncodedBinaryDecimal: Whether or not the value is BCD encoded.
         * @retval The value of the specified CMOS register, optionally having been converted from BCD format.
         */
        static uint8_t readCMOSRegister (uint16_t cmosRegister, bool isBCDEncodedBinaryDecimal = false) {
            
            const bool NMIDisableBit = 0b1;
            writeByteToPort(0x70, (NMIDisableBit << 7) | cmosRegister);

            if (isBCDEncodedBinaryDecimal) return _bcdToBinary(readByteFromPort(0x71));
            else return readByteFromPort(0x71);

        }

};

#endif