#include "keyboard.hpp"
    
const char* THKeyboard::keycode_name[] = {
    "`", "ESCAPE", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "TAB",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "ENTER",
    "LEFT_CTRL", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "#",
    "LEFT_SHIFT", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RIGHT_SHIFT",
    "ASTERISK", "LEFT_ALT", "SPACE"
};

const char THKeyboard::keycode_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '?', '?',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '?',
    '?', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '#',
    '?', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '?',
    '?', '?', '?'
};