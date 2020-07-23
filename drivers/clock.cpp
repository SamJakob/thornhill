#include "io.cpp"
#include "clock.hpp"

ThornhillSystemTime ThornhillClock::_performOfflineTimeRead() {

    uint8_t statusRegisterB = ThornhillIO::readCMOSRegister(0x0B);

    bool b24HourFormat  =   (statusRegisterB & 0b010);
    bool isBcdEncoded   =  !(statusRegisterB & 0b100);

    uint16_t century = ThornhillIO::readCMOSRegister(0x32, isBcdEncoded);
    
    ThornhillSystemTime time;

    time.year        =       ThornhillIO::readCMOSRegister(0x09, isBcdEncoded);
    time.fullYear    =       (century != 0) ? (century * 100) + time.year : 0;

    time.month       =       ThornhillIO::readCMOSRegister(0x08, isBcdEncoded);
    time.day         =       ThornhillIO::readCMOSRegister(0x07, isBcdEncoded);

    time.hours       =       ThornhillIO::readCMOSRegister(0x04, isBcdEncoded);
    time.minutes     =       ThornhillIO::readCMOSRegister(0x02, isBcdEncoded);
    time.seconds     =       ThornhillIO::readCMOSRegister(0x00, isBcdEncoded);

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

bool ThornhillClock::isRTCUpdateInProgress() {
    uint8_t statusRegisterA = ThornhillIO::readCMOSRegister(0x0A);
    return statusRegisterA &= 0x40;
}

ThornhillSystemTime ThornhillClock::readOfflineTime() {
    return _performOfflineTimeRead();
    /*ThornhillSystemTime time, time2;

    while (isRTCUpdateInProgress()) {}
    time = _performOfflineTimeRead();
    while (isRTCUpdateInProgress()) {}
    time2 = _performOfflineTimeRead();

    if (compareSystemTime(&time, &time2)) return time;
    else return readOfflineTime();*/
}