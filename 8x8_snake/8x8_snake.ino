#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices (1 MAX7219 matrix)
#define MAX_DEVICES 1

// Pin connections for the MAX7219
#define DATA_IN 12   // Data in (DIN)
#define CLK 11       // Clock (CLK)
#define CS 10        // Chip select (LOAD/CS)

// Create an instance of the MD_MAX72XX library with GENERIC_HW
MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::GENERIC_HW, DATA_IN, CLK, CS, MAX_DEVICES);

// Joystick pins
#define JOY_X A0
#define JOY_Y A1

// Game variables
const int matrixWidth = 8;
const int matrixHeight = 8;
int snakeLength = 2;           // Start with length 2
int snakeX[matrixWidth * matrixHeight]; // Snake's X positions
int snakeY[matrixWidth * matrixHeight]; // Snake's Y positions
int foodX, foodY;             // Food position
int direction = 0;            // Direction of the snake (0=right, 1=down, 2=left, 3=up)
int lastDirection = 0;        // Last valid direction (to prevent moving backward)
bool gameOver = false;        // Game over flag
bool nextMovement[2] = {false, false}; // Tracks next movement (up, right)

void setup() {
  mx.begin();                     // Initialize the display
  mx.control(MD_MAX72XX::INTENSITY, 8); // Set brightness
  mx.clear();                    // Clear the display
  randomSeed(analogRead(0));    // Seed random generator for food position

  // Initialize the snake's starting positions
  snakeX[0] = 3; // First block
  snakeY[0] = 3; // First block
  snakeX[1] = 2; // Second block (initially to the left of the first block)
  snakeY[1] = 3; // Second block

  // Show start animation
  flashingAnimation();
  
  // Place food at a random location
  placeFood();
}

void loop() {
  if (!gameOver) {
    readJoystick(); // Read joystick input
    updateGame();  // Update the game state
    delay(200);    // Game speed (adjust for difficulty)
  } else {
    showGameOver(); // Show game over screen
    delay(2000);    // Delay before restarting
    resetGame();    // Reset the game state
    flashingAnimation(); // Show animation on restart
  }
}
// Animation variables
const int flashCount = 3; // Reduced number of flashes
const int flashCount2 = 2; // Reduced number of flashes
const int flashDelay = 400; // Increased delay between flashes
const int suspenseDelay = 1000; // Game over "kill screen"

// Function to create a flashing animation
void flashingAnimation() {
  for (int i = 0; i < flashCount; i++) {
    mx.clear();  // Turn off the display
    delay(flashDelay);
    
    // Turn on the display (all pixels)
    for (int x = 0; x < matrixWidth; x++) {
      for (int y = 0; y < matrixHeight; y++) {
        mx.setPoint(y, x, true); // Turn on all points
      }
    }
    
    delay(flashDelay);
  }
}

// Game over flashing animation
void flashingAnimation2() {
  for (int i = 0; i < flashCount2; i++) {
    mx.clear();  // Turn off the display
    delay(flashDelay);
    
    // Turn on the display (all pixels)
    for (int x = 0; x < matrixWidth; x++) {
      for (int y = 0; y < matrixHeight; y++) {
        mx.setPoint(y, x, true); // Turn on all points
      }
    }
    
    delay(flashDelay);
  }
  // Turn on the display (all pixels)
  for (int x = 0; x < matrixWidth; x++) {
    for (int y = 0; y < matrixHeight; y++) {
      mx.setPoint(y, x, true); // Turn on all points
    }
  }
}


// Function to read joystick input and update direction
void readJoystick() {
  int xReading = analogRead(JOY_X);
  int yReading = analogRead(JOY_Y);

  // Simple joystick threshold for direction control
  if (xReading < 400 && lastDirection != 0) {
    direction = 2; // Move left
    nextMovement[0] = false; // Reset next movements
    nextMovement[1] = false;
  } else if (xReading > 600 && lastDirection != 2) {
    direction = 0; // Move right
    nextMovement[0] = false; // Reset next movements
    nextMovement[1] = false;
  } else if (yReading < 400 && lastDirection != 1) {
    direction = 3; // Move up
    nextMovement[0] = false; // Reset next movements
    nextMovement[1] = false;
  } else if (yReading > 600 && lastDirection != 3) {
    direction = 1; // Move down
    nextMovement[0] = false; // Reset next movements
    nextMovement[1] = false;
  }

  // Check for diagonal movements
  if (xReading < 400 && yReading < 400 && lastDirection != 2 && lastDirection != 3) {
    // Move up-left diagonally
    nextMovement[0] = true; // next move up
    nextMovement[1] = true; // next move left
    direction = 3; // Move up
  } else if (xReading < 400 && yReading > 600 && lastDirection != 2 && lastDirection != 1) {
    // Move down-left diagonally
    nextMovement[0] = true; // next move down
    nextMovement[1] = true; // next move left
    direction = 1; // Move down
  } else if (xReading > 600 && yReading < 400 && lastDirection != 0 && lastDirection != 3) {
    // Move up-right diagonally
    nextMovement[0] = true; // next move up
    nextMovement[1] = true; // next move right
    direction = 3; // Move up
  } else if (xReading > 600 && yReading > 600 && lastDirection != 0 && lastDirection != 1) {
    // Move down-right diagonally
    nextMovement[0] = true; // next move down
    nextMovement[1] = true; // next move right
    direction = 1; // Move down
  }

  // Update the last direction if the movement is valid
  if (direction != lastDirection) {
    lastDirection = direction; 
  }
}

// Function to update the game state
void updateGame() {
  // Move the snake by shifting positions
  for (int i = snakeLength; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Check if next movement flags are set
  if (nextMovement[0] && nextMovement[1]) {
    // Move in two steps based on the current direction and flags
    if (nextMovement[0]) {
      switch (lastDirection) {
        case 0: snakeX[0]++; break; // Move right
        case 1: snakeY[0]++; break; // Move down
        case 2: snakeX[0]--; break; // Move left
        case 3: snakeY[0]--; break; // Move up
      }
      // After moving once, reset the first flag
      nextMovement[0] = false;
    } else {
      // Move in the second direction
      switch (direction) {
        case 0: snakeX[0]++; break; // Move right
        case 1: snakeY[0]++; break; // Move down
        case 2: snakeX[0]--; break; // Move left
        case 3: snakeY[0]--; break; // Move up
      }
      // After moving the second time, reset the second flag
      nextMovement[1] = false;
    }
  } else {
    // Update the snake's head position based on direction
    switch (direction) {
      case 0: snakeX[0]++; break; // Move right
      case 1: snakeY[0]++; break; // Move down
      case 2: snakeX[0]--; break; // Move left
      case 3: snakeY[0]--; break; // Move up
    }
  }

  // Wrap the snake around the screen
  if (snakeX[0] < 0) {
    snakeX[0] = matrixWidth - 1; // Wrap around to the right
  } else if (snakeX[0] >= matrixWidth) {
    snakeX[0] = 0; // Wrap around to the left
  }
  
  if (snakeY[0] < 0) {
    snakeY[0] = matrixHeight - 1; // Wrap around to the bottom
  } else if (snakeY[0] >= matrixHeight) {
    snakeY[0] = 0; // Wrap around to the top
  }

  // Check for collisions with itself
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true; // Game over
    }
  }

  // Check if the snake has eaten the food
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++; // Increase snake length
    placeFood();   // Place new food
  }

  // Clear the display
  mx.clear();
  // Draw the snake
  for (int i = 0; i < snakeLength; i++) {
    mx.setPoint(snakeY[i], snakeX[i], true); // Draw the snake's body
  }
  // Draw the food
  mx.setPoint(foodY, foodX, true); // Draw the food
}

// Function to place food in a random position
void placeFood() {
  foodX = random(0, matrixWidth);
  foodY = random(0, matrixHeight);
}

// Function to show Game Over message
void showGameOver() {
  delay(suspenseDelay);
  mx.clear();
  flashingAnimation2(); // Flashing animation during game over
}

// Function to reset the game
void resetGame() {
  snakeLength = 2;            // Reset length
  snakeX[0] = 3;              // First block
  snakeY[0] = 3;              // First block
  snakeX[1] = 2;              // Second block
  snakeY[1] = 3;              // Reset second block position
  direction = 0;              // Reset direction
  lastDirection = 0;          // Reset last direction
  gameOver = false;           // Reset game over flag
  placeFood();                // Place food
}
