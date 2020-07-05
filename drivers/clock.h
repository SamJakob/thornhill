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

bool compareSystemTime(struct ThornhillSystemTime timeA, struct ThornhillSystemTime timeB) {
    return timeA.year == timeB.year &&
        timeA.fullYear == timeB.fullYear &&
        timeA.month == timeB.month &&
        timeA.day == timeB.day &&
        timeA.hours == timeB.hours &&
        timeA.minutes == timeB.minutes &&
        timeA.seconds == timeB.seconds &&
        timeA.isPM == timeB.isPM;
}

#endif