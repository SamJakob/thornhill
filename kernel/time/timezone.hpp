#ifndef TH_TIME_TIMEZONES
#define TH_TIME_TIMEZONES

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

const ThornhillTimeZone TH_TIMEZONE_LONDON                  = {
    "London, England",
    "United Kingdom",
    {  0, 0 },
    {  1, 0 }
};

const ThornhillTimeZone TH_TIMEZONE_NEW_BRAUNFELS           = {
    "New Braunfels, Texas",
    "United States of America",
    { -6, 0 },
    {  1, 0 }
};

#endif