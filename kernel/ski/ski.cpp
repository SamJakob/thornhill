#include "ski.hpp"

#include "../../drivers/graphics.hpp"
#include "../../drivers/hardware/keyboard.hpp"

#include "../../config/version.h"
#include "kernel/memory/physical.hpp"
#include "drivers/hardware/pit.hpp"
#include "cstring"

#include <utilities>
#include <thornhill>

using namespace Thornhill;

char buffer[ThornhillSKIBufferSize];

int printStart = 1;
int interpeterStart = 1;
int pos = 1;

void insertU(const char *in) {
    ThornhillSKI::insert(in);
    interpeterStart = pos;
}

void ThornhillSKI::initialize() {
    memzero(buffer, ThornhillSKIBufferSize);
    printStart = 1;
    interpeterStart = 1;
    pos = 1;

    insertU("k > ");

    Kernel::debug("SKI", "SKI initialized");
}

bool redraw = false;

int savedX = -1;
int savedY = -1;

bool lockDraw = false;

// Aquire lock draw
void ald() {
    while (lockDraw) {}
    lockDraw = true;
}

// Release lock draw
void rld() {
    lockDraw = false;
}

bool squareOn = false;

void ThornhillSKI::draw() {
    ald();
    Screen *screen = ThornhillGraphicsDriver::getScreen();

    unsigned int termWidth = screen->width - 240;
    unsigned int termHeight = screen->height - 240;

    // window content
    if (redraw) {
        ThornhillGraphicsDriver::drawRect(rgb(0, 0, 0), 120, 140, termWidth, termHeight);
	redraw = false;
    } else if (savedX > 0 && savedY > 0) {
        ThornhillGraphicsDriver::drawRect(rgb(0, 0, 0), savedX, savedY, 10, 10);
	squareOn = true;
    }
    ThornhillGraphicsDriver::drawTextFuncky(buffer, printStart, ThornhillSKIBufferSize, 120, 140, 1, 0, termWidth, &savedX, &savedY);

    ThornhillGraphicsDriver::drawRect(rgb(255, 255, 255), savedX, savedY, 10, 10);
    rld();
}

void ThornhillSKI::handleTimer(ThornhillSystemTime*) {
    ald();
    if (squareOn)
        ThornhillGraphicsDriver::drawRect(rgb(0, 0, 0), savedX, savedY, 10, 10);
    else
        ThornhillGraphicsDriver::drawRect(rgb(255, 255, 255), savedX, savedY, 10, 10);
    squareOn = !squareOn;
    rld();
}

char triggerChar = '\n';

void ThornhillSKI::insertCharAd(char a, bool check, bool draw) {
    if (triggerChar == a && check) {
	buffer[pos] = 0;
	ThornhillSKI::process();
	return;
    }
    buffer[pos] = a;
    pos = (pos + 1) % ThornhillSKIBufferSize;
 
    if (pos == printStart) {
	//Make it so that it goes at least 2
	if (buffer[printStart] != 0 && buffer[printStart] != '\n')
	    printStart = (printStart + 1) % ThornhillSKIBufferSize;
	if (buffer[printStart] != 0 && buffer[printStart] != '\n')
	    printStart = (printStart + 1) % ThornhillSKIBufferSize;
	while (buffer[printStart] != 0 && buffer[printStart] != '\n' && printStart != pos + 1) {
	    printStart = (printStart + 1) % ThornhillSKIBufferSize;
	}
	buffer[printStart] = 0;
	printStart++;
	redraw = true;
    }
    if (draw) ThornhillSKI::draw();
}

void ThornhillSKI::insertChar(char a) {
    ThornhillSKI::insertCharAd(a, true, true);
}

bool checkEq(const char* str, size_t pos) {
    size_t pointer = 0;
    size_t pointerB = pos;
    while(str[pointer] != 0 && str[pointer] == buffer[pointerB]) {
	pointer++;
	pointerB = (pointerB + 1) % ThornhillSKIBufferSize;
    }
    return str[pointer] == buffer[pointerB];
}

void ThornhillSKI::process() {
    if (checkEq("panic", interpeterStart)) {

        insertU("\npanic!");
	for (int c = 0; c < ThornhillSKIBufferSize; c++) {
	    insertCharAd('a', false, true);
	}
	Kernel::panic("You asked for it", 69, "I don't know man I am just some code!");

        return;

    } else if (checkEq("sysinfo", interpeterStart)) {

        char itoaBuffer[33];
        itoaBuffer[32] = 0;

        char padBuffer[3];
        padBuffer[2] = 0;

        insertU(
            "\n\n"
            "System Information:\n"
            "Thornhill - build " TH_GIT_REV " (built on " TH_BUILD_DATE " at " TH_BUILD_TIME ")\n"
            "\n"
            "-> Used Physical Memory: "
        );
        insertU(uitoa(
            itoaBuffer, ThornhillMemory::Physical::getUsedMemory(),
            10, 32
        ));
        insertU(" bytes");

        insertU("\n"
            "-> Total Physical Memory: "
        );

        insertU(uitoa(
            itoaBuffer, ThornhillMemory::Physical::getTotalMemory() / 1024 / 1024,
            10, 32
        ));

        insertU(" MiB (");
        insertU(uitoa(
            itoaBuffer, ThornhillMemory::Physical::getTotalMemory(),
            10, 32
        ));
        insertU(" bytes)");

        insertU("\n"
            "-> Current System Time: "
        );

        ThornhillSystemTime systemTime = ThornhillPITDriver::getCurrentTime();

        insertU(uitoa(
            itoaBuffer, systemTime.month,
            10, 32
            ));
        insertU("/");
        insertU(strpad(
            uitoa(
                itoaBuffer, systemTime.day,
                10, 32
                ),
            padBuffer,
            '0',
            2
            ));
        insertU("/");
        insertU(uitoa(
            itoaBuffer, systemTime.fullYear,
            10, 32
            ));
        insertU(", ");
        insertU(strpad(
            uitoa(
                itoaBuffer, systemTime.hours,
                10, 32
                ),
            padBuffer,
            '0',
            2
            ));
        insertU(":");
        insertU(strpad(
            uitoa(
                itoaBuffer, systemTime.minutes,
                10, 32
                ),
            padBuffer,
            '0',
            2
            ));
        insertU(":");
        insertU(strpad(
            uitoa(
                itoaBuffer, systemTime.seconds,
                10, 32
                ),
            padBuffer,
            '0',
            2
            ));
        insertU("\n\n");

    } else if (checkEq("clear", interpeterStart)) {

        memzero(buffer, ThornhillSKIBufferSize);
        printStart = 1;
        interpeterStart = 1;
        pos = 1;

        redraw = true;
        ThornhillSKI::draw();

    } else if (checkEq("help", interpeterStart)) {

        insertU(
            "\n\n"
            "Available Commands:\n"
            "- help: Display this message.\n"
            "\n"
            "- clear: Clear the screen.\n"
            "- panic: Trigger a system panic.\n"
            "- sysinfo: Show basic system information.\n"
            "\n"
        );

    } else {

        insertU("\n\nCommand not found. Try 'help' to see commands.\n\n");

    }

    insertU("k > ");
}

void ThornhillSKI::insert(const char* str) {
    size_t pointer = 0;
    while (str[pointer] != 0) {
	insertCharAd(str[pointer], false, false);
	pointer++;
    }
    ThornhillSKI::draw();
}

void ThornhillSKI::goBack() {
    if (pos == interpeterStart) return;
    if (pos == 0) pos = ThornhillSKIBufferSize;
    pos--;
    buffer[pos] = 0;
    ThornhillSKI::draw();
}

void ThornhillSKI::handleInput(unsigned int keycode) {
    if (keycode > ThornhillKeyboard::getCharacterSetSize()) return;
    else if (keycode == KEY_ENTER) ThornhillSKI::insertChar('\n');
    else if (keycode == KEY_SPACE) ThornhillSKI::insertChar(' ');
    else if (keycode == KEY_BACKSPACE) ThornhillSKI::goBack();
    else
        insertChar(ThornhillKeyboard::translateKeycodeToAscii(keycode));
}
