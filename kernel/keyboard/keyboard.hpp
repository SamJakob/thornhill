#include <thornhill>

#pragma once

#define KEY_BACKSPACE   0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LEFT_CTRL   0x1D
#define KEY_LEFT_SHIFT  0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_LEFT_ALT    0x38
#define KEY_SPACE       0x39

typedef void (*ThornhillKeyboardHandler)(unsigned int);

/**
 * Kernel keyboard interface. This receives entered keys from a/the system keyboard driver and
 * allows the OS kernel to perform the appropriate action.
 */
class ThornhillKeyboard {

    private:
        static ThornhillKeyboardHandler onKeyPressHandler;

    public:
        /**
         * Returns the size of the character set (in other words, the keycode after the maximum
         * possible keycode).
         *
         * @return The size of the character set.
         */
        static unsigned int getCharacterSetSize();

        /**
         * Translates a given keycode to an ASCII character.
         * Returns '?' if the specified keycode is not in the system character set.
         *
         * @param keycode The keycode to translate.
         * @return The ASCII character.
         */
        static char translateKeycodeToAscii(unsigned int keycode);

        /**
         * Translates a given keycode to the key's name.
         * You shouldn't use this at all for characters that can be easily represented with
         * {@see translateKeycodeToAscii}.
         *
         * Returns a nullptr if the specified keycode is not in the system character set.
         *
         * @param keycode The keycode to translate.
         * @return The name of the key represented by the keycode.
         */
        static const char* translateKeycodeToKeyName(unsigned int keycode);

        /**
         * Replaces the currently registered handler for a keypress with the specified
         * handler. This should be a function that takes an unsigned int for the keycode.
         *
         * @param handler The new handler for a key press.
         */
        // TODO: refactor this to be 'onKeyPress', allow multiple handlers
        //  (probably need some kind of linkedlist)
        //  ...
        //  and make it return a handler 'subscription', so the handler can be cancelled if
        //  desired?
        static void setOnKeyPress(ThornhillKeyboardHandler handler);

        /**
         * Triggers the registered handler for a keypress of the specified keycode.
         * @param keycode The keycode of the key that was pressed.
         */
        static void triggerKeyPress(unsigned int keycode);

};
