#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef TH_DRIVER_CLOCK
#define TH_DRIVER_CLOCK

struct ThornhillSystemTime {

    uint8_t year;
    uint16_t fullYear;
    uint8_t month;
    uint8_t day;

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    bool isPM;

};

struct ThornhillTimeOffset {
    uint8_t hours;
    uint8_t minutes;
};

bool compareSystemTime(ThornhillSystemTime* timeA, ThornhillSystemTime* timeB) {
    return timeA->year == timeB->year &&
        timeA->month == timeB->month &&
        timeA->day == timeB->day &&
        timeA->hours == timeB->hours &&
        timeA->minutes == timeB->minutes &&
        timeA->seconds == timeB->seconds &&
        timeA->isPM == timeB->isPM;
}

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
        static ThornhillSystemTime _performOfflineTimeRead();

    public:
        /**
         * @brief  Determines whether or not the RTC circuit is currently updating.
         * @note   This is used by the readOfflineTime method to prevent the clock from being read
         *          whilst it is being updated - which could potentially result in erroneous values.
         * @retval true if the circuit is updating, otherwise false.
         */
        static bool isRTCUpdateInProgress();

        /**
         * @brief  Reads the current system time from the RTC.
         * @note   See: https://wiki.osdev.org/CMOS#RTC_Update_In_Progress
         * @retval ThornhillSystemTime
         */
        static ThornhillSystemTime readOfflineTime();

};

#endif