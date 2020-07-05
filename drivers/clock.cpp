#include "clock.hpp"
#include "io.cpp"

class ThornhillClock {
    
    private:
        /**
         * @brief  Reads the current time from the system RTC.
         * @note   DO NOT, **DO NOT** use this method alone to read the system time.
         *         This method alone does not ensure that the RTC is being updated,
         *         meaning that reading this could result in dodgy or inconsistent
         *         value such as reading any of 8:59, 8:60, 8:00 or 9:00 at 9:00am.
         * @retval ThornhillSystemTime
         */
        static struct ThornhillSystemTime _performOfflineTimeRead() {

            uint8_t statusRegisterB = ThornhillIO::readCMOSRegister(0x0B, true);
            bool b24HourFormat = (statusRegisterB &= 0b010);
            bool bBinaryMode   = (statusRegisterB &= 0b100);

            uint16_t century = ThornhillIO::readCMOSRegister(0x32, bBinaryMode);
            
            struct ThornhillSystemTime time;

            time.year        =       ThornhillIO::readCMOSRegister(0x09, bBinaryMode);
            time.fullYear    =       (century != 0) ? (century * 100) + time.year : 0;

            time.month       =       ThornhillIO::readCMOSRegister(0x08, bBinaryMode);
            time.day         =       ThornhillIO::readCMOSRegister(0x07, bBinaryMode);

            time.hours       =       ThornhillIO::readCMOSRegister(0x04, bBinaryMode);
            time.minutes     =       ThornhillIO::readCMOSRegister(0x02, bBinaryMode);
            time.seconds     =       ThornhillIO::readCMOSRegister(0x00, bBinaryMode);

            // Handle 24-hour time.
            if (b24HourFormat && time.hours > 12) {
                time.isPM = true;
                time.hours -= 12;
            }

            if (b24HourFormat && time.hours == 0) time.hours = 12;

            // Handle 12-hour time.
            if (!b24HourFormat) {
                if (time.hours &= 0x80) time.isPM = true;
                time.hours &= 0b01111111; // (Mask pm bit off.)
            }

            return time;

        }
    
    public:
        static bool isRTCUpdateInProgress() {
            uint8_t statusRegisterA = ThornhillIO::readCMOSRegister(0x0A, false);
            bool bRTCUpdateInProgress = statusRegisterA &= 0x40;
        }

        /**
         * @brief  Reads the current system time from the RTC.
         * @note   See: https://wiki.osdev.org/CMOS#RTC_Update_In_Progress
         * @retval ThornhillSystemTime
         */
        struct ThornhillSystemTime readOfflineTime() {

            struct ThornhillSystemTime time;

            while (isRTCUpdateInProgress()) {}
            time = _performOfflineTimeRead();

            while (isRTCUpdateInProgress()) {}
            if (compareSystemTime(time, _performOfflineTimeRead())) return time;
            else return readOfflineTime();

        }

};