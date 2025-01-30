// #include <U8g2lib.h>
// #include <RotaryEncoder.h>

// // OLED Display setup (I2C)
// U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D5, /* data=*/ D4);

// // Rotary Encoder setup
// #define PIN_CLK D0  // CLK pin of the rotary encoder
// #define PIN_DT D1   // DT pin of the rotary encoder
// #define PIN_SW D2   // SW (button) pin of the rotary encoder
// RotaryEncoder encoder(PIN_CLK, PIN_DT);

// // Menu variables
// const char* categories[] = {"Animal", "Plant", "Bird", "City"};
// const char* animals[] = {"Cow", "Cat", "Dog", "Ox", "Goat"};
// const char* plants[] = {"Rose", "Tulip", "Oak", "Pine", "Cactus"};
// const char* birds[] = {"Eagle", "Sparrow", "Parrot", "Crow", "Pigeon"};
// const char* cities[] = {"New York", "London", "Tokyo", "Paris", "Delhi"};

// int menuLevel = 0; // 0: Main menu, 1: Sub-menu
// int selectedCategory = 0; // Selected category index
// int selectedItem = 0; // Selected item index
// bool buttonPressed = false;
// int lastEncoderPos = 0; // Track last encoder position for smooth scrolling

// void setup() {
//   // Initialize OLED
//   u8g2.begin();

//   // Initialize rotary encoder
//   pinMode(PIN_SW, INPUT_PULLUP);
//   encoder.setPosition(0);
//   lastEncoderPos = encoder.getPosition();
// }

// void loop() {
//   // Read rotary encoder
//   encoder.tick();
//   int newPos = encoder.getPosition();

//   // Handle menu navigation
//   if (menuLevel == 0) {
//     // Main menu: Scroll through categories
//     selectedCategory = constrain(newPos, 0, 3);
//   } else {
//     // Sub-menu: Scroll through items
//     selectedItem = constrain(newPos, 0, 4);
//   }

//   // Handle button press
//   if (digitalRead(PIN_SW) == LOW && !buttonPressed) {
//     buttonPressed = true;
//     if (menuLevel == 0) {
//       // Enter sub-menu
//       menuLevel = 1;
//       encoder.setPosition(0); // Reset encoder position for sub-menu
//       selectedItem = 0; // Reset selected item
//     } else {
//       // Exit sub-menu
//       menuLevel = 0;
//       encoder.setPosition(selectedCategory); // Reset encoder position for main menu
//     }
//     delay(200); // Debounce delay
//   } else if (digitalRead(PIN_SW) == HIGH) {
//     buttonPressed = false;
//   }

//   // Only redraw the display if the encoder position has changed
//   if (newPos != lastEncoderPos) {
//     drawMenu();
//     lastEncoderPos = newPos;
//   }

//   // Small delay for smooth navigation
//   delay(10);
// }

// void drawMenu() {
//   u8g2.clearBuffer();

//   if (menuLevel == 0) {
//     // Draw main menu
//     u8g2.setFont(u8g2_font_ncenB08_tr);
//     u8g2.drawStr(10, 10, "Select Category:");
//     for (int i = 0; i < 4; i++) {
//       if (i == selectedCategory) {
//         u8g2.drawStr(20, 20 + i * 10, ("> " + String(categories[i])).c_str());
//       } else {
//         u8g2.drawStr(20, 20 + i * 10, categories[i]);
//       }
//     }
//   } else {
//     // Draw sub-menu
//     u8g2.setFont(u8g2_font_ncenB08_tr);
//     u8g2.drawStr(10, 10, "Select Item:");
//     const char** items = nullptr;
//     switch (selectedCategory) {
//       case 0: items = animals; break;
//       case 1: items = plants; break;
//       case 2: items = birds; break;
//       case 3: items = cities; break;
//     }
//     for (int i = 0; i < 5; i++) {
//       if (i == selectedItem) {
//         u8g2.drawStr(20, 20 + i * 10, ("> " + String(items[i])).c_str());
//       } else {
//         u8g2.drawStr(20, 20 + i * 10, items[i]);
//       }
//     }
//   }

//   u8g2.sendBuffer();
// }


/*                                        smooth function                                            */


#include <U8g2lib.h>
#include <RotaryEncoder.h>

// OLED Display setup (I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D5, /* data=*/ D4);

// Rotary Encoder setup
#define PIN_CLK D0  // CLK pin of the rotary encoder
#define PIN_DT D1   // DT pin of the rotary encoder
#define PIN_SW D2   // SW (button) pin of the rotary encoder
RotaryEncoder encoder(PIN_CLK, PIN_DT);

// Text items to display
const char* textItems[] = {
  "Animal", "Plant", "Bird", "City", "Fruit", "Car", "Country", "Color"
};
const int numItems = sizeof(textItems) / sizeof(textItems[0]); // Number of items

// Menu variables
int selectedItem = 0; // Currently selected item
bool buttonPressed = false; // Track button press
int lastEncoderPos = 0; // Track last encoder position for smooth scrolling

void setup() {
  // Initialize OLED
  u8g2.begin();

  // Initialize rotary encoder
  pinMode(PIN_SW, INPUT_PULLUP);
  encoder.setPosition(0);
  lastEncoderPos = encoder.getPosition();
}

void loop() {
  // Read rotary encoder
  encoder.tick();
  int newPos = encoder.getPosition();

  // Handle scrolling
  selectedItem = constrain(newPos, 0, numItems - 1); // Limit to the number of items

  // Handle button press
  if (digitalRead(PIN_SW) == LOW && !buttonPressed) {
    buttonPressed = true;
    // Perform action for selected item
    displaySelectedItem();
    delay(200); // Debounce delay
  } else if (digitalRead(PIN_SW) == HIGH) {
    buttonPressed = false;
  }

  // Only redraw the display if the encoder position has changed
  if (newPos != lastEncoderPos) {
    drawMenu();
    lastEncoderPos = newPos;
  }

  // Small delay for smooth navigation
  delay(10);
}

void drawMenu() {
  u8g2.clearBuffer();

  // Draw the list of items
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 10, "Scroll to Select:");

  // Display up to 5 items at a time (adjust based on screen size)
  for (int i = 0; i < 5; i++) {
    int itemIndex = selectedItem - 2 + i; // Center the selected item
    if (itemIndex >= 0 && itemIndex < numItems) {
      if (itemIndex == selectedItem) {
        u8g2.drawStr(20, 20 + i * 10, ("> " + String(textItems[itemIndex])).c_str()); // Highlight selected item
      } else {
        u8g2.drawStr(20, 20 + i * 10, textItems[itemIndex]); // Display other items
      }
    }
  }

  u8g2.sendBuffer();
}

void displaySelectedItem() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Selected:");
  u8g2.drawStr(10, 50, textItems[selectedItem]); // Display the selected item
  u8g2.sendBuffer();
  delay(1000); // Show the selected item for 1 second
}