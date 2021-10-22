#pragma once

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define DEFAULT_SERIAL_PORT COM1

class ThornhillSerial {

    private:
      static bool initialized;
      static bool isTransitEmpty();

    public:
      /**
       * @brief Initializes the serial interface and driver. Returns true if the
       * intialization was succesful, or false if there was an error.
       * 
       * @return true Initialization was successful and the driver may be used.
       * @return false Initialization failed and the driver is not ready for
       * use.
       */
      static bool initialize();
      
      static void writeCharacter(unsigned char data);
      static void write(const char* data, bool newlineChars = true);
};
