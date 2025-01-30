// #include <U8g2lib.h>
// #include <RotaryEncoder.h>

// // OLED Display setup (I2C)
// U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D5, /* data=*/ D4);

// // Rotary Encoder setup
// #define PIN_CLK D0  // CLK pin of the rotary encoder
// #define PIN_DT D1   // DT pin of the rotary encoder
// #define PIN_SW D2   // SW (button) pin of the rotary encoder
// RotaryEncoder encoder(PIN_CLK, PIN_DT, RotaryEncoder::LatchMode::TWO03);

// // Game Constants
// #define GRID_SIZE 4
// #define WIDTH 32
// #define HEIGHT 16

// // Game Variables
// struct Position { int x, y; };
// Position snake[100], food;
// int snakeLength = 3;
// int dirX = 1, dirY = 0;
// bool gameover = false;
// int score = 0;
// int speed = 200;

// // Rotary Encoder Variables
// int lastEncoderPos = 0;
// bool horizontalMode = false;  // Default: Vertical movement

// // Directions
// enum Direction { UP, DOWN, LEFT, RIGHT };
// Direction snakeDirection = RIGHT;

// void setup() {
//     u8g2.begin();
//     pinMode(PIN_SW, INPUT_PULLUP);
//     encoder.setPosition(0);
//     randomSeed(analogRead(0));
//     resetGame();
// }

// void loop() {
//     if (!gameover) {
//         updateGame();
//         drawGame();
//         delay(speed);
//     } else {
//         drawGameOver();
//         if (digitalRead(PIN_SW) == LOW) {
//             delay(500); // Debounce delay
//             resetGame();
//         }
//     }
// }

// void updateGame() {
//     // Read rotary encoder
//     encoder.tick();

//     // **Debounce button press for mode toggle**
//     static unsigned long lastPress = 0;
//     if (digitalRead(PIN_SW) == LOW && millis() - lastPress > 200) {
//         horizontalMode = !horizontalMode;  // Toggle movement mode
//         lastPress = millis();
//     }

//     int newPos = encoder.getPosition();

//     // **Increase sensitivity by detecting changes in position**
//     if (abs(newPos - lastEncoderPos) >= 1) {  // Adjust sensitivity (2 = smoother, 1 = sensitive)
//         if (horizontalMode) {
//             snakeDirection = (newPos > lastEncoderPos) ? RIGHT : LEFT;
//         } else {
//             snakeDirection = (newPos > lastEncoderPos) ? DOWN : UP;
//         }
//         lastEncoderPos = newPos;  // Update position
//     }

//     // Update snake direction based on `snakeDirection`
//     switch (snakeDirection) {
//         case UP:    dirX = 0; dirY = -1; break;
//         case DOWN:  dirX = 0; dirY = 1;  break;
//         case LEFT:  dirX = -1; dirY = 0; break;
//         case RIGHT: dirX = 1; dirY = 0;  break;
//     }

//     // Calculate new head position
//     Position newHead = { snake[0].x + dirX, snake[0].y + dirY };

//     // Check collision with walls
//     if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
//         gameover = true;
//         return;
//     }

//     // Check collision with self
//     for (int i = 0; i < snakeLength; i++) {
//         if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
//             gameover = true;
//             return;
//         }
//     }

//     // Check collision with food
//     if (newHead.x == food.x && newHead.y == food.y) {
//         score++;
//         snakeLength++;
//         speed = max(50, speed - 10); // Increase speed
//         spawnFood();
//     }

//     // Move snake
//     for (int i = snakeLength - 1; i > 0; i--) {
//         snake[i] = snake[i - 1];
//     }
//     snake[0] = newHead;
// }

// void drawGame() {
//     u8g2.clearBuffer();
//     for (int i = 0; i < snakeLength; i++) {
//         u8g2.drawBox(snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
//     }
//     u8g2.drawBox(food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
//     u8g2.setFont(u8g2_font_ncenB08_tr);
//     u8g2.drawStr(5, 10, ("Score: " + String(score)).c_str());
//     u8g2.sendBuffer();
// }

// void drawGameOver() {
//     u8g2.clearBuffer();
//     u8g2.setFont(u8g2_font_ncenB08_tr);
//     u8g2.drawStr(20, 30, "Game Over!");
//     u8g2.drawStr(20, 40, ("Score: " + String(score)).c_str());
//     u8g2.drawStr(20, 50, "Press Button");
//     u8g2.sendBuffer();
// }

// void resetGame() {
//     snake[0] = { WIDTH / 2, HEIGHT / 2 };
//     snakeLength = 3;
//     dirX = 1; dirY = 0;
//     score = 0;
//     speed = 200;
//     spawnFood();
//     gameover = false;
// }

// void spawnFood() {
//     food.x = random(1, WIDTH - 2);
//     food.y = random(1, HEIGHT - 2);
// }

/*     Snake Game            */ 

#include <U8g2lib.h>
#include <RotaryEncoder.h>

// OLED Display setup (I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D5, /* data=*/ D4);

// Rotary Encoder setup
#define PIN_CLK D0  // CLK pin of the rotary encoder
#define PIN_DT D1   // DT pin of the rotary encoder
#define PIN_SW D2   // SW (button) pin of the rotary encoder
RotaryEncoder encoder(PIN_CLK, PIN_DT, RotaryEncoder::LatchMode::TWO03);

// Game Constants
#define GRID_SIZE 4
#define WIDTH 32
#define HEIGHT 16

// Game Variables
struct Position { int x, y; };
Position snake[100], food;
int snakeLength = 3;
int dirX = 1, dirY = 0;  // Initial direction: right
bool gameover = false;
int score = 0;
int speed = 200;

// Rotary Encoder Variables
int lastEncoderPos = 0;

void setup() {
    u8g2.begin();
    pinMode(PIN_SW, INPUT_PULLUP);
    encoder.setPosition(0);
    randomSeed(analogRead(0));
    resetGame();
}

void loop() {
    if (!gameover) {
        updateGame();
        drawGame();
        delay(speed);
    } else {
        drawGameOver();
        if (digitalRead(PIN_SW) == LOW) {
            delay(500); // Debounce delay
            resetGame();
        }
    }
}

void updateGame() {
    // Read rotary encoder
    encoder.tick();
    int newPos = encoder.getPosition();

    // Update snake direction based on rotary encoder movement
    if (newPos > lastEncoderPos) {
        dirX = 1; dirY = 0;  // Move right (clockwise)
    } else if (newPos < lastEncoderPos) {
        dirX = -1; dirY = 0; // Move left (anti-clockwise)
    }
    lastEncoderPos = newPos;

    // Calculate new head position
    Position newHead = { snake[0].x + dirX, snake[0].y + dirY };

    // Check collision with walls
    if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
        gameover = true;
        return;
    }

    // Check collision with self
    for (int i = 0; i < snakeLength; i++) {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
            gameover = true;
            return;
        }
    }

    // Check collision with food
    if (newHead.x == food.x && newHead.y == food.y) {
        score++;
        snakeLength++;
        speed = max(50, speed - 10); // Increase speed
        spawnFood();
    }

    // Move snake
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = newHead;
}

void drawGame() {
    u8g2.clearBuffer();
    for (int i = 0; i < snakeLength; i++) {
        u8g2.drawBox(snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
    }
    u8g2.drawBox(food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 10, ("Score: " + String(score)).c_str());
    u8g2.sendBuffer();
}

void drawGameOver() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(20, 30, "Game Over!");
    u8g2.drawStr(20, 40, ("Score: " + String(score)).c_str());
    u8g2.drawStr(20, 50, "Press Button");
    u8g2.sendBuffer();
}

void resetGame() {
    snake[0] = { WIDTH / 2, HEIGHT / 2 };
    snakeLength = 3;
    dirX = 1; dirY = 0;  // Initial direction: right
    score = 0;
    speed = 200;
    spawnFood();
    gameover = false;
}

void spawnFood() {
    food.x = random(1, WIDTH - 2);
    food.y = random(1, HEIGHT - 2);
}