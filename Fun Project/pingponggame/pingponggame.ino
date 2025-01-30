#include <U8g2lib.h>
#include <RotaryEncoder.h>

// OLED Display setup (I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D5, /* data=*/ D4);

// Rotary Encoder setup
#define PIN_CLK D0  // CLK pin of the rotary encoder
#define PIN_DT D1   // DT pin of the rotary encoder
#define PIN_SW D2   // SW (button) pin of the rotary encoder
RotaryEncoder encoder(PIN_CLK, PIN_DT, RotaryEncoder::LatchMode::TWO03);

// Game variables
int paddleX = 4; // Paddle position (X-axis)
int paddleY = 24;
int ballX, ballY, ballDirX, ballDirY;
int score = 0;
int speed = 2;
bool gameover = false;

void setup() {
    u8g2.begin();
    pinMode(PIN_SW, INPUT_PULLUP);
    randomSeed(analogRead(0)); // Ensure randomness
    resetGame();
}

void loop() {
    encoder.tick(); // Ensure encoder updates every loop
    if (!gameover) {
        updateGame();
        drawGame();
        delay(10);
    } else {
        drawGameOver();
        if (digitalRead(PIN_SW) == LOW) {
            resetGame();
            gameover = false;
        }
    }
}

void updateGame() {
    int newPos = encoder.getPosition();
    paddleY = constrain(map(newPos, -10, 10, 0, 48), 0, 48); // Adjusted range for better movement

    ballX += ballDirX * speed;
    ballY += ballDirY * speed;

    // Ball collision with walls
    if (ballX >= 127) ballDirX *= -1; // Right wall
    if (ballY <= 0 || ballY >= 63) ballDirY *= -1; // Top and bottom walls

    // Ball collision with paddle
    if (ballX <= paddleX + 4 && ballY >= paddleY && ballY <= paddleY + 16) {
        ballDirX = -ballDirX;
        ballDirY = random(-1, 2); // Randomize bounce direction
        score++;
        if (score % 5 == 0 && speed < 5) speed++; // Gradual speed increase
    }

    // Ball out of bounds
    if (ballX <= 0) gameover = true; // If ball goes beyond paddle
}

void drawGame() {
    u8g2.clearBuffer();
    u8g2.drawBox(paddleX, paddleY, 4, 16); // Paddle on the left side
    u8g2.drawDisc(ballX, ballY, 2);
    u8g2.drawFrame(0, 0, 128, 64);
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
    ballX = 64;
    ballY = 32;
    ballDirX = random(0, 2) ? 1 : -1;
    ballDirY = random(-1, 2);
    paddleY = 24;
    encoder.setPosition(0); // Reset encoder to prevent unexpected jumps
    score = 0;
    speed = 2;
}
