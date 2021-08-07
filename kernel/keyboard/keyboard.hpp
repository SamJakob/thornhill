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
    
    extern const char* keycode_name[];
    extern const char keycode_ascii[];

};

#endif