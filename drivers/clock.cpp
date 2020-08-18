#include "io.cpp"
#include "../kernel/time/timezone.hpp"

#include "clock.hpp"

ThornhillSystemTime ThornhillClock::_performOfflineTimeRead() {

    uint8_t statusRegisterB = ThornhillIO::readCMOSRegister(0x0B);

    bool b24HourFormat  =   (statusRegisterB & 0b010);
    bool isBcdEncoded   =  !(statusRegisterB & 0b100);

    int8_t century = ThornhillIO::readCMOSRegister(0x32, isBcdEncoded);
    
    ThornhillSystemTime time;

    time.year        =       ThornhillIO::readCMOSRegister(0x09, isBcdEncoded);
    time.fullYear    =       (century != 0) ? (century * 100) + time.year : 0;

    time.month       =       ThornhillIO::readCMOSRegister(0x08, isBcdEncoded);
    time.day         =       ThornhillIO::readCMOSRegister(0x07, isBcdEncoded);

    time.hours       =       ThornhillIO::readCMOSRegister(0x04, isBcdEncoded);
    time.minutes     =       ThornhillIO::readCMOSRegister(0x02, isBcdEncoded);
    time.seconds     =       ThornhillIO::readCMOSRegister(0x00, isBcdEncoded);

    time.isPM        =       false;

    // Handle 24-hour time.
    if (b24HourFormat && time.hours >= 12) {
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

bool ThornhillClock::isRTCUpdateInProgress() {
    uint8_t statusRegisterA = ThornhillIO::readCMOSRegister(0x0A);
    return statusRegisterA &= 0x40;
}

ThornhillSystemTime ThornhillClock::readOfflineTime() {
    ThornhillSystemTime time = _performOfflineTimeRead();

    // Apply time offset.
    // ThornhillClock::applyTimeZone(&time, &TH_TIMEZONE_NEW_BRAUNFELS, true);
    ThornhillClock::applyTimeZone(&time, &TH_TIMEZONE_LONDON, true);

    return time;

    // TODO: Fix RTCUpdateInProgress check.
    /*ThornhillSystemTime time, time2;

    while (isRTCUpdateInProgress()) {}
    time = _performOfflineTimeRead();
    while (isRTCUpdateInProgress()) {}
    time2 = _performOfflineTimeRead();

    if (compareSystemTime(&time, &time2)) return time;
    else return readOfflineTime();*/
}

void ThornhillClock::applyTimeOffset(ThornhillSystemTime* time, ThornhillTimeOffset offset) {
    time->minutes += offset.minutes;
    while (time->minutes > 59) {
        time->hours++;
        time->minutes -= 60;
    }
    while (time->minutes < 0) {
        time->hours--;
        time->minutes += 60;
    }

    time->hours += offset.hours;
    while (time->hours > 12) {
        if (time->isPM) {
            time->isPM = false;
            time->day++;
            time->hours -= 12;
        } else {
            time->isPM = true;
            time->hours -= 12;
        }
    }
    while (time->hours < 0) {
        if (time->isPM) {
            //time->isPM = false;
            time->hours += 12;
        } else {
            time->isPM = true;
            time->day--;
            time->hours += 12;
        }
    }
}

void ThornhillClock::applyTimeZone(ThornhillSystemTime* time, const ThornhillTimeZone* zone, bool applyDST) {
    applyTimeOffset(time, zone->utcOffset);
    if (applyDST) applyTimeOffset(time, zone->dstOffset);
}