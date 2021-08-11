#include <stdint.h>

#ifndef TH_KERNEL_TIME_TIMEZONE
#define TH_KERNEL_TIME_TIMEZONE

struct ThornhillTimeOffset {
    int8_t hours;
    int8_t minutes;
};

struct ThornhillTimeZone {
    // The human-readable time zone name.
    const char* timeZoneName;

    // The country the time zone belongs to.
    const char* country;

    // The UTC offset of that time zone.
    ThornhillTimeOffset utcOffset;

    // The Daylight Savings Time (DST) offset of the time zone.
    // Note that the DST offset, unlike the UTC offset is relative to the already offset time.
    // So if the timezone is UTC-6 in standard time and UTC-5 in DST, this would be +1 hour ({1, 0}).
    ThornhillTimeOffset dstOffset;
};

extern const ThornhillTimeZone TH_TIMEZONE_LONDON;
extern const ThornhillTimeZone TH_TIMEZONE_NEW_BRAUNFELS;

#endif