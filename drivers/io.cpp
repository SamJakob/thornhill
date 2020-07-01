#ifndef TH_DRIVER_IO
#define TH_DRIVER_IO

class ThornhillIO {

    public:
        /**
         * @brief  Reads a single byte from an IO port.
         * @param  port: The IO port to read data from.
         * @retval The byte that was read.
         */
        static char readByteFromPort (unsigned short port) {
            unsigned char result;

            __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
            return result;
        }

        /**
         * @brief  Writes a single byte to an IO port.
         * @param  port: The IO port to write to.
         * @param  data: The byte to write to the port.
         * @retval None
         */
        static void writeByteToPort (unsigned short port, unsigned char data) {
            __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
        }

        /**
         * @brief  Reads a WORD (two bytes) from an IO port.
         * @param  port: The IO port to read data from.
         * @retval The bytes that were read (in the form of a short).
         */
        static unsigned short readWordFromPort (unsigned short port) {
            unsigned short result;
            __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
            return result;
        }

        /**
         * @brief  Writes a WORD (two bytes) to an IO port.
         * @param  port: The IO port to write to.
         * @param  data: The bytes to write to the port (in the form of a short).
         * @retval None
         */
        static void writeWordToPort (unsigned short port, unsigned short data) {
            __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
        }

};

#endif