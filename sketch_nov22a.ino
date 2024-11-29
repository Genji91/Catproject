#include <PWMServo.h>            // Include PWMServo for controlling servos
#include <PCM.h>                  // Include PCM library for audio playback
#include <Adafruit_NeoPixel.h>    // Include NeoPixel library for NeoPixel control

#define PIN 6                     // Define the NeoPixel pin
#define NUMPIXELS 1               // Number of NeoPixels in the strip

// Create PWMServo objects for controlling servos
PWMServo servo1; 
PWMServo servo2; 

// Servo position and direction variables
int servoPos = 0;
bool increasing = true;
unsigned long previousServoMillis = 0;
const long servoInterval = 15;  // Time between servo position updates (15 ms)

// Create NeoPixel object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Colour delay for NeoPixel
int colourDelay = 100;
int red = 0;
int green = 0;
int blue = 0;

// PCM data for the speaker (cat sound)
const unsigned char sample[] PROGMEM = {
  126, 125, 127, 129, 131, 131, 129, 128, 126, 125
  // Add the rest of the PCM data here
};

// Setup function
void setup() {
  startPlayback(sample, sizeof(sample));  // Begin audio playback (PCM)
  
  pixels.begin();  // Initialize NeoPixel library
  servo1.attach(9);  // Attach servo1 to pin 9 (PWM pin)
  servo2.attach(5);  // Attach servo2 to pin 5 (PWM pin)
}

// Function to randomly set colors for NeoPixel
void ColourSettings() {
  red = random(0, 255);
  green = random(0, 255);
  blue = random(0, 255);
}

// Function to update NeoPixel color
void neoPixel() {
  ColourSettings();  // Randomly set red, green, and blue

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
    pixels.show();  // Update NeoPixel color
    delay(colourDelay);  // Delay for next color update
  }
}

// Function to control the servo motors
void servoMotors() {
  unsigned long currentMillis = millis();

  // Handle servo movement
  if (currentMillis - previousServoMillis >= servoInterval) {
    previousServoMillis = currentMillis;  // Reset the timer
    if (increasing) {
      servoPos++;  // Increase servo position
      if (servoPos >= 180) {
        increasing = false;  // Reverse direction at 180 degrees
      }
    } else {
      servoPos--;  // Decrease servo position
      if (servoPos <= 0) {
        increasing = true;  // Reverse direction at 0 degrees
      }
    }
  }

  // Move servos
  servo1.write(servoPos);
  servo2.write(servoPos);
}

// Main loop function
void loop() {
  neoPixel();  // Update NeoPixel colors
  servoMotors();  // Update servo positions
}
