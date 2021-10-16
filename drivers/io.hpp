#include <stdint.h>

#pragma once

class ThornhillIO {

    public:
        /**
         * @brief  Reads a single byte from an IO port.
         * @param  port: The IO port to read data from.
         * @retval The byte that was read.
         */
        static uint8_t readByteFromPort (uint16_t port);

        /**
         * @brief  Writes a single byte to an IO port.
         * @param  port: The IO port to write to.
         * @param  data: The byte to write to the port.
         * @retval None
         */
        static void writeByteToPort (uint16_t port, uint8_t data);

        /**
         * @brief  Reads a WORD (two bytes) from an IO port.
         * @param  port: The IO port to read data from.
         * @retval The bytes that were read (in the form of a short).
         */
        static uint16_t readWordFromPort (uint16_t port);

        /**
         * @brief  Writes a WORD (two bytes) to an IO port.
         * @param  port: The IO port to write to.
         * @param  data: The bytes to write to the port (in the form of a short).
         * @retval None
         */
        static void writeWordToPort (uint16_t port, uint16_t data);

        /**
         * @brief  Reads a value from the CMOS register.
         * @note   If the value is a binary-coded decimal, setting isBCDEncodedBinaryDecimal to true
         *         will cause the value to be automatically decoded.
         * @param  cmosRegister: The register to obtain a value from.
         * @param  isBCDEncodedBinaryDecimal: Whether or not the value is BCD encoded.
         * @retval The value of the specified CMOS register, optionally having been converted from BCD format.
         */
        static uint8_t readCMOSRegister (uint16_t cmosRegister, bool isBCDEncodedBinaryDecimal = false);

};