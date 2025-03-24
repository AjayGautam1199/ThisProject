// #define LEFT_LED PC0
// #define RIGHT_LED PC1
// #define BRAKE_LED PB0
// #define LEFT_BUTTON PC2
// #define RIGHT_BUTTON PC3

// void setup() {
//     // Configure LEDs as OUTPUT
//     DDRC |= (1 << LEFT_LED) | (1 << RIGHT_LED);  
//     DDRB |= (1 << BRAKE_LED);  

//     // Configure buttons as INPUT with pull-up resistors
//     DDRC &= ~((1 << LEFT_BUTTON) | (1 << RIGHT_BUTTON)); 
//     PORTC |= (1 << LEFT_BUTTON) | (1 << RIGHT_BUTTON);  // Enable pull-ups

//     // Initialize Serial Monitor
//     Serial.begin(9600);
// }

// void loop() {
//     static uint8_t left_state = 0, right_state = 0, hazard_mode = 0;
//     static uint32_t last_toggle = 0, button_press_time = 0;
//     uint32_t now = millis();

//     // Read button states
//     uint8_t left_pressed = !(PINC & (1 << LEFT_BUTTON));  
//     uint8_t right_pressed = !(PINC & (1 << RIGHT_BUTTON));

//     // Handle button press logic
//     if (left_pressed && right_pressed) {
//         if (button_press_time == 0) button_press_time = now;
//         if (now - button_press_time >= 1000) {
//             hazard_mode = 1;
//             left_state = right_state = 0;  // Turn off individual indicators
//         }
//     } else if (left_pressed) {
//         if (button_press_time == 0) button_press_time = now;
//         if (now - button_press_time >= 1000) {
//             if (hazard_mode) hazard_mode = 0;
//             else {
//                 left_state = !left_state;
//                 right_state = 0;
//             }
//         }
//     } else if (right_pressed) {
//         if (button_press_time == 0) button_press_time = now;
//         if (now - button_press_time >= 1000) {
//             if (hazard_mode) hazard_mode = 0;
//             else {
//                 right_state = !right_state;
//                 left_state = 0;
//             }
//         }
//     } else {
//         button_press_time = 0;  // Reset if button is released
//     }

//     // LED Blinking logic every 300ms
//     if (now - last_toggle >= 300) {
//         last_toggle = now;
//         if (hazard_mode) {
//             PORTC ^= (1 << LEFT_LED) | (1 << RIGHT_LED);
//         } else {
//             if (left_state) PORTC ^= (1 << LEFT_LED);
//             else PORTC &= ~(1 << LEFT_LED);

//             if (right_state) PORTC ^= (1 << RIGHT_LED);
//             else PORTC &= ~(1 << RIGHT_LED);
//         }
//     }

//     // Brake LED control (ON if indicators or hazard mode active)
//     if (left_state || right_state || hazard_mode) PORTB |= (1 << BRAKE_LED);
//     else PORTB &= ~(1 << BRAKE_LED);

//     // Debugging output
//     Serial.print("Left: "); Serial.print(left_state);
//     Serial.print(" | Right: "); Serial.print(right_state);
//     Serial.print(" | Hazard: "); Serial.println(hazard_mode);

//     delay(50);
// }

// Define pins
#define LEFT_LED A0
#define RIGHT_LED A1
#define BRAKE_LED 8
#define LEFT_BUTTON A2
#define RIGHT_BUTTON A4

void setup() {
    pinMode(LEFT_LED, OUTPUT);
    pinMode(RIGHT_LED, OUTPUT);
    pinMode(BRAKE_LED, OUTPUT);
    
    pinMode(LEFT_BUTTON, INPUT_PULLUP);  
    pinMode(RIGHT_BUTTON, INPUT_PULLUP);
}

void loop() {
    // Read button states
    bool leftPressed = digitalRead(LEFT_BUTTON) == LOW;
    bool rightPressed = digitalRead(RIGHT_BUTTON) == LOW;

    // Left LED control
    if (leftPressed) {
        digitalWrite(LEFT_LED, HIGH);
        digitalWrite(RIGHT_LED, LOW);  // Turn off right if left is pressed
    } 
    
    // Right LED control
    else if (rightPressed) {
        digitalWrite(RIGHT_LED, HIGH);
        digitalWrite(LEFT_LED, LOW);  // Turn off left if right is pressed
    } 
    
    // Turn off both if no button is pressed
    else {
        digitalWrite(LEFT_LED, LOW);
        digitalWrite(RIGHT_LED, LOW);
    }

    // Brake LED turns ON if any indicator is active
    if (leftPressed || rightPressed) {
        digitalWrite(BRAKE_LED, HIGH);
    } else {
        digitalWrite(BRAKE_LED, LOW);
    }
}
