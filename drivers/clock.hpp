#include <stdint.h>

#include "../kernel/time/timezone.hpp"

#ifndef TH_DRIVERS_CLOCK
#define TH_DRIVERS_CLOCK

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

bool THCompareSystemTime(ThornhillSystemTime* timeA, ThornhillSystemTime* timeB);

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

    /**
     * @brief  Calculates the offset from a given time.
     * @note
     * @param  time: A pointer to the time to offset.
     * @param  offset: A pointer to the offset to apply to the time.
     * @retval The updated time.
     */
    static void applyTimeOffset(ThornhillSystemTime* time, ThornhillTimeOffset offset);

    /**
     * @brief  Calculates the time in a given time zone, using that time zone's given time offset.
     * @note
     * @param  time: A pointer to the time to offset.
     * @param  zone: A pointer to the Time Zone to derive offset information from.
     * @param  applyDST: Whether or not to also apply that Time Zone's DST (Daylight Savings Time)
     * offset.
     * @retval The updated time.
     */
    static void applyTimeZone(ThornhillSystemTime* time, const ThornhillTimeZone* zone,
                              bool applyDST = false);
};

extern const char* MONTHS[];

#endif