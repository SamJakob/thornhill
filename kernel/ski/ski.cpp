#include "ski.hpp"

#include "../../drivers/graphics.hpp"
#include "../../drivers/hardware/keyboard.hpp"

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
    for (int i = 0; i < ThornhillSKIBufferSize; i++) {
        buffer[i] = 0;
    }
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
    } else insertU("\nCommand not found\n");
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
