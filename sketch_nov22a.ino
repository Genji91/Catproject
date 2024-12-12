#include <PCM.h>                 // Include PCM library for audio playback
#include <Adafruit_NeoPixel.h>   // Include NeoPixel library for NeoPixel control
#include <PWMServo.h>            // Include Servo library for controlling servos
#include "pcmdata.h"             // Include the PCM data library to access the PCM data of the audio file being played

#define PIN 6                    // Define the NeoPixel pin
#define NUMPIXELS 1              // Number of NeoPixels in the strip

// Create Servo objects for controlling servos
PWMServo servo1; 
PWMServo servo2; 

// Servo position and direction variables
int servo1Pos = 0, servo2Pos = 180; // Independent positions for each servo
bool servo1Increasing = true, servo2Increasing = false; // Directions
unsigned long previousServo1Millis = 0, previousServo2Millis = 0;
const long servo1Interval = 10; // Interval for servo1 movement
const long servo2Interval = 20; // Interval for servo2 movement
// Servo control flag
bool isServo1Active = true;  // Flag to alternate between servos

// Audio and pause control variables
unsigned long previousPauseMillis = 0;
const long pauseInterval = 3000;  // Pause every 3 seconds
bool isPaused = false;
unsigned long pauseStartMillis = 0;
const long pauseDuration = 2000;  // Pause duration (2 seconds)

// NeoPixel object and variables
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
unsigned long previousPixelMillis = 0;
const long pixelInterval = 100;
int red = 0, green = 0, blue = 0;

void ColourSettings();
void neoPixel();
void updateServos();
void handlePauseAndAudio();

// Setup function
void setup() {
  pixels.begin();       // Initialize NeoPixel
  servo1.attach(9);     // Attach servo1 to pin 9
  servo2.attach(10);    // Attach servo2 to pin 10
}

// Main loop function
void loop() {
  neoPixel();              // Update NeoPixel colors
  handlePauseAndAudio();   // Manage servo pause and audio playback
  updateServos();          // Control servos
}


// Randomly set colors for NeoPixel
void ColourSettings() {
  red = random(0, 32);
  green = random(0, 32);
  blue = random(0,32);
}

// Non-blocking NeoPixel color updates
void neoPixel() {
  unsigned long currentMillis = millis();

  if (isPaused) {
    // Turn off the NeoPixel when the audio is playing
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));  // Set color to off (black)
    }
    pixels.show();  // Update NeoPixel colors
    return;  // Skip the rest of the function
  }

  if (currentMillis - previousPixelMillis >= pixelInterval) {
    previousPixelMillis = currentMillis;  // Reset the timer
    ColourSettings();  // Randomly set red, green, and blue

    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(red, green, blue));
    }
    pixels.show();  // Update NeoPixel colors
  }
}


// Control the servos independently
void updateServos() {
  unsigned long currentMillis = millis();

  if (isPaused) {
    // During pause, detach both servos to save power
    if (servo1.attached()) servo1.detach();
    if (servo2.attached()) servo2.detach();
    return;
  }

  // Attach the active servo, detach the inactive one
  if (isServo1Active) {
    if (!servo1.attached()) servo1.attach(9);
    if (servo2.attached()) servo2.detach();
  } else {
    if (!servo2.attached()) servo2.attach(10);
    if (servo1.attached()) servo1.detach();
  }

  // Servo 1 Movement
  if (isServo1Active && currentMillis - previousServo1Millis >= servo1Interval) {
    previousServo1Millis = currentMillis;
    if (servo1Increasing) {
      servo1Pos++;
      if (servo1Pos >= 180) servo1Increasing = false;
    } else {
      servo1Pos--;
      if (servo1Pos <= 0) servo1Increasing = true;
    }
    servo1.write(servo1Pos);
  }

  // Servo 2 Movement
  if (!isServo1Active && currentMillis - previousServo2Millis >= servo2Interval) {
    previousServo2Millis = currentMillis;
    if (servo2Increasing) {
      servo2Pos++;
      if (servo2Pos >= 180) servo2Increasing = false;
    } else {
      servo2Pos--;
      if (servo2Pos <= 0) servo2Increasing = true;
    }
    servo2.write(servo2Pos);
  }
}

// Handle audio playback and pausing servos
void handlePauseAndAudio() {
  unsigned long currentMillis = millis();

  if (!isPaused && (currentMillis - previousPauseMillis >= pauseInterval)) {
    // Start pause and play audio
    isPaused = true;
    pauseStartMillis = currentMillis;
    stopPlayback(); // Ensure previous audio playback is stopped
    startPlayback(sample, sizeof(sample)); // Start PCM audio playback
  }

  if (isPaused && (currentMillis - pauseStartMillis >= pauseDuration)) {
    // End pause, switch active servo, and resume servo movement
    isPaused = false;
    isServo1Active = !isServo1Active;  // Alternate the active servo
    previousPauseMillis = currentMillis;
  }
}