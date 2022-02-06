#include "keyboard.hpp"

const char* keycode_name[] = {
    "`", "ESCAPE", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "TAB",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "ENTER",
    "LEFT_CTRL", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "#",
    "LEFT_SHIFT", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RIGHT_SHIFT",
    "ASTERISK", "LEFT_ALT", "SPACE"
};

const char keycode_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '?', '?',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '?',
    '?', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '#',
    '?', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '?',
    '?', '?', '?'
};

ThornhillKeyboardHandler ThornhillKeyboard::onKeyPressHandler;

unsigned int ThornhillKeyboard::getCharacterSetSize() {
    return *(&keycode_ascii + 1) - keycode_ascii;
}

char ThornhillKeyboard::translateKeycodeToAscii(unsigned int keycode) {
    // If the key code is not contained in the character set, simply return '?'.
    if (keycode >= getCharacterSetSize()) return '?';

    // Otherwise, return the keycode.
    return keycode_ascii[keycode];
}

const char* ThornhillKeyboard::translateKeycodeToKeyName(unsigned int keycode) {
    // If the key code is not contained in the character set, simply return '?'.
    if (keycode >= getCharacterSetSize()) return nullptr;

    // Otherwise, return the name.
    return keycode_name[keycode];
}

void ThornhillKeyboard::setOnKeyPress(ThornhillKeyboardHandler handler) {
    onKeyPressHandler = handler;
}

void ThornhillKeyboard::triggerKeyPress(unsigned int keycode) {
    if (onKeyPressHandler != nullptr) onKeyPressHandler(keycode);
}
