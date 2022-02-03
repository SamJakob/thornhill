#include "ski.hpp"

#include <cstring>

#include "../../drivers/graphics.hpp"
#include "../../drivers/hardware/keyboard.hpp"
#include "../keyboard/keyboard.hpp"

#include <utilities>
#include <thornhill>

using namespace Thornhill;

char buffer[ThornHillSKIBufferSize];

int printStart = 1;
int interpeterStart = 1;
int pos = 1;


void ThornhillSKI::initialize() {
    for (int i = 0; i < ThornHillSKIBufferSize; i++) {
        buffer[i] = 0;
    }
    printStart = 1;
    interpeterStart = 1;
    pos = 1;

    insert("k > ");
    interpeterStart = pos;

    Kernel::debug("SKI", "SKI initialized");
}

bool redraw = false;

void ThornhillSKI::draw() {

    Screen *screen = ThornhillGraphics::getScreen();

    unsigned int termWidth = screen->width - 240;
    unsigned int termHeight = screen->height - 240;

    // window content
    if (redraw) {
	ThornhillGraphics::drawRect(rgb(0, 0, 0), 120, 140, termWidth, termHeight);
	redraw = false;
    }
    //ThornhillGraphics::drawText(const char *characters, uint16_t x, uint16_t y, int scale = 1, int padding = 0)(rgb(0, 0, 0), 120, 140, termWidth, termHeight);
    ThornhillGraphics::drawTextFuncky(buffer, printStart, ThornHillSKIBufferSize, 120, 140, 1, 0, termWidth);


}

char triggerChar = '\n';

void ThornhillSKI::insertCharAd(char a, bool check, bool draw) {
    if (triggerChar == a && check) {
	buffer[pos] = 0;
	ThornhillSKI::process();
	return;
    }
    buffer[pos] = a;
    pos = (pos + 1) % ThornHillSKIBufferSize;
 
    if (pos == printStart) {
	//Make it so that it goes at least 2
	if (buffer[printStart] != 0 && buffer[printStart] != '\n')
	    printStart = (printStart + 1) % ThornHillSKIBufferSize;
	if (buffer[printStart] != 0 && buffer[printStart] != '\n')
	    printStart = (printStart + 1) % ThornHillSKIBufferSize;
	while (buffer[printStart] != 0 && buffer[printStart] != '\n' && printStart != pos + 1) {
	    printStart = (printStart + 1) % ThornHillSKIBufferSize;
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
	pointerB = (pointerB + 1) % ThornHillSKIBufferSize;
    }
    return str[pointer] == buffer[pointerB];
}

void ThornhillSKI::process() {
    if (checkEq("panic", interpeterStart)) {
	ThornhillSKI::insert("\npanic!");
	for (int c = 0; c < ThornHillSKIBufferSize; c++) {
	    insertCharAd('a', false, true);
	}
	Kernel::panic("You asked for it", 69, "I don't know man I am just some code!");
	return;
    } else {
	ThornhillSKI::insert("\nCommand not found\n");
    }
    ThornhillSKI::insert("k > ");
    interpeterStart = pos;
}

void ThornhillSKI::insert(const char* str) {
    size_t pointer = 0;
    while (str[pointer] != 0) {
	insertCharAd(str[pointer], false, false);
	pointer++;
    }
    ThornhillSKI:draw();
}

void ThornhillSKI::goBack() {
    if (pos == interpeterStart) return;
    if (pos == 0) pos = ThornHillSKIBufferSize;
    pos--;
    buffer[pos] = 0;
    //Improve this
    redraw = true;
    ThornhillSKI::draw();
}

void ThornhillSKI::handleInput(uint8_t keycode) {
    if (keycode > KEYCODE_MAX) return;
    else if (keycode == KEY_ENTER) ThornhillSKI::insertChar('\n');
    else if (keycode == KEY_SPACE) ThornhillSKI::insertChar(' ');
    else if (keycode == KEY_BACKSPACE) ThornhillSKI::goBack();
    else insertChar(THKeyboard::keycode_ascii[(int)keycode]);
}
