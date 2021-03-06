#ifndef TH_KERN_KEYBOARD
#define TH_KERN_KEYBOARD

#define KEY_BACKSPACE   0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LEFT_CTRL   0x1D
#define KEY_LEFT_SHIFT  0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_LEFT_ALT    0x38
#define KEY_SPACE       0x39

#define KEYCODE_MAX     57

namespace THKeyboard {
    
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

};

#endif