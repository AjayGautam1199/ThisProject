// #include <Arduino.h>
// #include <USBComposite.h>

// // Pin definitions for rows and columns
// const int ROWS[2] = {PB3, PB4};   // Row pins
// const int COLS[6] = {PA0, PA1, PA2, PA3, PA4, PA5};  // Column pins

// // USB HID Keyboard configuration
// USBHID HID;
// HIDKeyboard Keyboard(HID);

// // Keymap definition with specific shortcut functions
// const uint16_t KEYMAP[2][6] = {
//     // First row - productivity and window management shortcuts
//     {
//         KEY_COPY,           // Copy
//         KEY_PASTE,          // Paste
//         KEY_CUT,            // Cud:\ThisProject\stm32\STM32_Ntc_agt
//         KEY_UNDO,           // Undo
//         KEY_SAVE,           // Save
//         KEY_NEW_TAB         // New Tab
//     },
//     // Second row - advanced shortcuts
//     {
//         KEY_SCREENSHOT,     // Screenshot
//         KEY_LOCK_SCREEN,    // Lock Screen
//         KEY_TASK_MANAGER,   // Task Manager
//         KEY_EXPLORER,       // Open File Explorer
//         KEY_BROWSER,        // Open Default Browser
//         KEY_CALCULATOR      // Open Calculator
//     }
// };

// // Keyboard state tracking
// bool currentKeyState[2][6] = {false};
// bool previousKeyState[2][6] = {false};

// void setup() {
//     // Initialize USB HID
//     HID.begin(HID_KEYBOARD);
//     Keyboard.begin();

//     // Configure row pins as outputs (default low)
//     for (int i = 0; i < 2; i++) {
//         pinMode(ROWS[i], OUTPUT);
//         digitalWrite(ROWS[i], LOW);
//     }

//     // Configure column pins as inputs with pull-up resistors
//     for (int j = 0; j < 6; j++) {
//         pinMode(COLS[j], INPUT_PULLUP);
//     }

//     // Small delay to ensure USB connection is stable
//     delay(1000);
// }

// void loop() {
//     // Scan the matrix
//     scanMatrix();

//     // Small delay to prevent bouncing
//     delay(20);
// }

// void scanMatrix() {
//     // Scan each row
//     for (int row = 0; row < 2; row++) {
//         // Activate the current row
//         digitalWrite(ROWS[row], LOW);

//         // Check each column
//         for (int col = 0; col < 6; col++) {
//             // Read the column state (inverted due to pull-up)
//             currentKeyState[row][col] = !digitalRead(COLS[col]);

//             // Detect key press events
//             if (currentKeyState[row][col] && !previousKeyState[row][col]) {
//                 executeShortcut(row, col);
//             }

//             // Update previous state
//             previousKeyState[row][col] = currentKeyState[row][col];
//         }

//         // Deactivate the row
//         digitalWrite(ROWS[row], HIGH);
//     }
// }

// // Custom shortcut execution function
// void executeShortcut(int row, int col) {
//     uint16_t shortcut = KEYMAP[row][col];

//     switch(shortcut) {
//         case KEY_COPY:
//             // Ctrl + C
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press('c');
//             delay(50);
//             Keyboard.release('c');
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_PASTE:
//             // Ctrl + V
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press('v');
//             delay(50);
//             Keyboard.release('v');
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_CUT:
//             // Ctrl + X
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press('x');
//             delay(50);
//             Keyboard.release('x');
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_UNDO:
//             // Ctrl + Z
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press('z');
//             delay(50);
//             Keyboard.release('z');
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_SAVE:
//             // Ctrl + S
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press('s');
//             delay(50);
//             Keyboard.release('s');
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_NEW_TAB:
//             // Ctrl + T
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press('t');
//             delay(50);
//             Keyboard.release('t');
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_SCREENSHOT:
//             // Windows + Print Screen
//             Keyboard.press(KEY_LEFT_GUI);
//             Keyboard.press(KEY_PRINT_SCREEN);
//             delay(50);
//             Keyboard.release(KEY_PRINT_SCREEN);
//             Keyboard.release(KEY_LEFT_GUI);
//             break;

//         case KEY_LOCK_SCREEN:
//             // Windows + L
//             Keyboard.press(KEY_LEFT_GUI);
//             Keyboard.press('l');
//             delay(50);
//             Keyboard.release('l');
//             Keyboard.release(KEY_LEFT_GUI);
//             break;

//         case KEY_TASK_MANAGER:
//             // Ctrl + Shift + Esc
//             Keyboard.press(KEY_LEFT_CTRL);
//             Keyboard.press(KEY_LEFT_SHIFT);
//             Keyboard.press(KEY_ESC);
//             delay(50);
//             Keyboard.release(KEY_ESC);
//             Keyboard.release(KEY_LEFT_SHIFT);
//             Keyboard.release(KEY_LEFT_CTRL);
//             break;

//         case KEY_EXPLORER:
//             // Windows + E
//             Keyboard.press(KEY_LEFT_GUI);
//             Keyboard.press('e');
//             delay(50);
//             Keyboard.release('e');
//             Keyboard.release(KEY_LEFT_GUI);
//             break;

//         case KEY_BROWSER:
//             // Windows + R, then type "chrome" or "firefox"
//             Keyboard.press(KEY_LEFT_GUI);
//             Keyboard.press('r');
//             delay(50);
//             Keyboard.release('r');
//             Keyboard.release(KEY_LEFT_GUI);
            
//             // Small delay to ensure Run dialog opens
//             delay(200);
            
//             // Type browser executable (adjust as needed)
//             Keyboard.print("chrome");
//             Keyboard.press(KEY_RETURN);
//             Keyboard.release(KEY_RETURN);
//             break;

//         case KEY_CALCULATOR:
//             // Windows + R, then type "calc"
//             Keyboard.press(KEY_LEFT_GUI);
//             Keyboard.press('r');
//             delay(50);
//             Keyboard.release('r');
//             Keyboard.release(KEY_LEFT_GUI);
            
//             // Small delay to ensure Run dialog opens
//             delay(200);
            
//             Keyboard.print("calc");
//             Keyboard.press(KEY_RETURN);
//             Keyboard.release(KEY_RETURN);
//             break;

//         default:
//             break;
//     }
// }

// // Define custom key codes not in standard HID
// #define KEY_COPY 0xF0
// #define KEY_PASTE 0xF1
// #define KEY_CUT 0xF2
// #define KEY_UNDO 0xF3
// #define KEY_SAVE 0xF4
// #define KEY_NEW_TAB 0xF5
// #define KEY_SCREENSHOT 0xF6
// #define KEY_LOCK_SCREEN 0xF7
// #define KEY_TASK_MANAGER 0xF8
// #define KEY_EXPLORER 0xF9
// #define KEY_BROWSER 0xFA
// #define KEY_CALCULATOR 0xFB


#include <Keyboard.h>

// Pin definitions for rows and columns
const int ROWS[2] = {PB3, PB4};   // Row pins
const int COLS[6] = {PA0, PA1, PA2, PA3, PA4, PA5};  // Column pins

// Keymap definition with specific shortcut functions
const char KEYMAP[2][6] = {
    // First row - productivity and window management shortcuts
    {
        'c',  // Copy
        'v',  // Paste
        'x',  // Cut
        'z',  // Undo
        's',  // Save
        't'   // New Tab
    },
    // Second row - advanced shortcuts
    {
        'p',  // Screenshot
        'l',  // Lock Screen
        'k',  // Task Manager
        'e',  // File Explorer
        'b',  // Open Browser
        'a'   // Calculator
    }
};

// Keyboard state tracking
bool currentKeyState[2][6] = {false};
bool previousKeyState[2][6] = {false};

void setup() {
    // Initialize Keyboard
    Keyboard.begin();

    // Configure row pins as outputs (default low)
    for (int i = 0; i < 2; i++) {
        pinMode(ROWS[i], OUTPUT);
        digitalWrite(ROWS[i], LOW);
    }

    // Configure column pins as inputs with pull-up resistors
    for (int j = 0; j < 6; j++) {
        pinMode(COLS[j], INPUT_PULLUP);
    }

    // Small delay to ensure stability
    delay(1000);
}

void loop() {
    // Scan the matrix
    scanMatrix();

    // Small delay to prevent bouncing
    delay(20);
}

void scanMatrix() {
    // Scan each row
    for (int row = 0; row < 2; row++) {
        // Activate the current row
        digitalWrite(ROWS[row], LOW);

        // Check each column
        for (int col = 0; col < 6; col++) {
            // Read the column state (inverted due to pull-up)
            currentKeyState[row][col] = !digitalRead(COLS[col]);

            // Detect key press events
            if (currentKeyState[row][col] && !previousKeyState[row][col]) {
                executeShortcut(row, col);
            }

            // Update previous state
            previousKeyState[row][col] = currentKeyState[row][col];
        }

        // Deactivate the row
        digitalWrite(ROWS[row], HIGH);
    }
}

// Custom shortcut execution function
void executeShortcut(int row, int col) {
    char key = KEYMAP[row][col];

    switch(key) {
        case 'c':  // Copy
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('c');
            delay(50);
            Keyboard.release('c');
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 'v':  // Paste
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('v');
            delay(50);
            Keyboard.release('v');
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 'x':  // Cut
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('x');
            delay(50);
            Keyboard.release('x');
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 'z':  // Undo
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('z');
            delay(50);
            Keyboard.release('z');
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 's':  // Save
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('s');
            delay(50);
            Keyboard.release('s');
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 't':  // New Tab
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('t');
            delay(50);
            Keyboard.release('t');
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 'p':  // Screenshot
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press(KEY_PRINT_SCREEN);
            delay(50);
            Keyboard.release(KEY_PRINT_SCREEN);
            Keyboard.release(KEY_LEFT_GUI);
            break;

        case 'l':  // Lock Screen
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press('l');
            delay(50);
            Keyboard.release('l');
            Keyboard.release(KEY_LEFT_GUI);
            break;

        case 'k':  // Task Manager
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press(KEY_LEFT_SHIFT);
            Keyboard.press(KEY_ESC);
            delay(50);
            Keyboard.release(KEY_ESC);
            Keyboard.release(KEY_LEFT_SHIFT);
            Keyboard.release(KEY_LEFT_CTRL);
            break;

        case 'e':  // File Explorer
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press('e');
            delay(50);
            Keyboard.release('e');
            Keyboard.release(KEY_LEFT_GUI);
            break;

        case 'b':  // Open Browser
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press('r');
            delay(50);
            Keyboard.release('r');
            Keyboard.release(KEY_LEFT_GUI);
            
            // Small delay to ensure Run dialog opens
            delay(200);
            
            // Type browser executable (adjust as needed)
            Keyboard.print("chrome");
            Keyboard.press(KEY_RETURN);
            Keyboard.release(KEY_RETURN);
            break;

        case 'a':  // Calculator
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press('r');
            delay(50);
            Keyboard.release('r');
            Keyboard.release(KEY_LEFT_GUI);
            
            // Small delay to ensure Run dialog opens
            delay(200);
            
            Keyboard.print("calc");
            Keyboard.press(KEY_RETURN);
            Keyboard.release(KEY_RETURN);
            break;

        default:
            break;
    }
}