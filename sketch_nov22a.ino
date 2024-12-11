#include <PCM.h>                 // Include PCM library for audio playback
#include <Adafruit_NeoPixel.h>   // Include NeoPixel library for NeoPixel control
#include <PWMServo.h>            // Include Servo library for controlling servos

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

// PCM audio sample
const unsigned char sample[] PROGMEM = {
  126, 125, 127, 129, 131, 131, 129, 128, 126, 125, 125, 126, 127, 128, 128, 128, 127, 126, 126, 126, 126, 127, 127, 128, 128, 127, 127, 126, 126, 127, 127, 127, 127, 127, 127, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 127, 127, 127, 127, 126, 126, 126, 126, 126, 126, 126, 127, 127, 127, 127, 126, 127, 126, 126, 126, 126, 125, 125, 125, 126, 126, 126, 127, 128, 128, 128, 128, 128, 128, 128, 128, 127, 126, 125, 123, 122, 122, 122, 123, 125, 127, 128, 129, 132, 133, 133, 133, 132, 131, 129, 126, 123, 121, 118, 119, 121, 123, 124, 125, 127, 130, 133, 134, 133, 133, 133, 132, 131, 131, 130, 130, 129, 127, 124, 120, 119, 120, 120, 120, 121, 123, 127, 132, 135, 136, 136, 135, 134, 133, 131, 129, 128, 124, 120, 117, 117, 119, 121, 121, 122, 123, 128, 132, 134, 135, 135, 134, 134, 133, 132, 131, 129, 127, 124, 119, 116, 117, 118, 118, 119, 119, 122, 127, 131, 135, 136, 135, 134, 134, 133, 131, 130, 128, 126, 122, 118, 115, 116, 118, 118, 118, 118, 121, 127, 131, 134, 135, 135, 135, 135, 134, 132, 130, 129, 127, 123, 117, 113, 115, 118, 118, 119, 118, 120, 125, 131, 136, 137, 138, 138, 137, 135, 133, 131, 129, 127, 123, 118, 113, 114, 119, 121, 121, 119, 122, 128, 133, 136, 138, 137, 137, 136, 135, 133, 130, 129, 128, 125, 120, 115, 116, 119, 121, 120, 118, 118, 124, 133, 138, 141, 140, 139, 138, 138, 135, 132, 130, 129, 127, 123, 118, 115, 119, 121, 120, 119, 117, 122, 130, 135, 140, 140, 139, 139, 138, 135, 133, 131, 129, 127, 124, 119, 116, 120, 123, 121, 120, 118, 121, 127, 134, 138, 140, 140, 139, 138, 137, 135, 132, 132, 129, 123, 117, 113, 117, 120, 121, 120, 118, 121, 127, 135, 139, 140, 140, 139, 138, 137, 135, 132, 130, 128, 125, 119, 113, 117, 120, 121, 120, 115, 117, 124, 131, 137, 139, 140, 140, 139, 137, 136, 133, 130, 128, 124, 118, 113, 116, 120, 121, 120, 116, 117, 123, 131, 135, 138, 139, 138, 139, 138, 137, 133, 131, 129, 125, 121, 114, 115, 119, 119, 119, 115, 116, 122, 129, 135, 138, 139, 139, 140, 139, 138, 135, 132, 130, 126, 121, 113, 114, 120, 122, 121, 116, 117, 123, 132, 138, 140, 140, 139, 138, 137, 136, 135, 133, 130, 125, 120, 111, 115, 121, 124, 123, 118, 119, 123, 130, 136, 138, 139, 139, 139, 137, 137, 136, 134, 131, 126, 119, 112, 117, 122, 123, 121, 118, 121, 125, 133, 137, 139, 140, 138, 139, 138, 137, 135, 133, 130, 124, 115, 111, 119, 122, 124, 121, 119, 124, 128, 135, 138, 138, 138, 138, 139, 139, 137, 134, 133, 128, 121, 113, 114, 120, 123, 125, 120, 120, 123, 130, 138, 141, 140, 137, 136, 136, 136, 136, 133, 130, 125, 116, 109, 113, 118, 121, 121, 119, 123, 127, 133, 137, 139, 137, 137, 137, 137, 136, 134, 133, 128, 122, 110, 110, 116, 120, 122, 119, 120, 125, 131, 137, 139, 139, 137, 137, 135, 134, 131, 132, 132, 126, 116, 110, 117, 117, 123, 126, 119, 123, 134, 143, 138, 133, 135, 139, 134, 133, 137, 134, 131, 131, 118, 107, 119, 121, 119, 128, 124, 126, 138, 135, 138, 143, 131, 135, 134, 127, 137, 133, 132, 133, 123, 118, 114, 119, 125, 123, 124, 126, 133, 137, 140, 139, 140, 134, 133, 133, 132, 133, 131, 130, 120, 113, 115, 122, 126, 129, 127, 127, 132, 138, 139, 139, 138, 133, 132, 131, 131, 133, 133, 132, 125, 115, 114, 121, 124, 126, 125, 127, 133, 137, 140, 139, 137, 135, 132, 131, 134, 135, 133, 132, 123, 114, 113, 122, 128, 129, 130, 130, 133, 136, 141, 138, 138, 135, 132, 132, 132, 134, 132, 130, 119, 114, 115, 122, 126, 128, 127, 129, 135, 138, 138, 135, 135, 133, 132, 132, 134, 134, 132, 125, 116, 112, 116, 124, 127, 127, 126, 130, 134, 138, 136, 134, 134, 131, 131, 130, 132, 131, 130, 121, 111, 111, 119, 127, 129, 127, 126, 129, 134, 138, 137, 133, 130, 128, 128, 132, 136, 134, 126, 115, 110, 115, 125, 130, 131, 128, 128, 133, 137, 138, 135, 134, 131, 130, 131, 134, 134, 131, 120, 111, 113, 121, 131, 132, 131, 127, 130, 137, 140, 138, 135, 132, 132, 132, 135, 136, 132, 124, 112, 111, 118, 132, 135, 131, 125, 129, 133, 139, 139, 136, 132, 131, 132, 134, 133, 131, 124, 111, 111, 119, 131, 135, 134, 126, 125, 128, 137, 139, 138, 135, 132, 130, 132, 133, 130, 122, 109, 110, 120, 134, 137, 134, 125, 123, 126, 132, 136, 139, 135, 133, 130, 129, 128, 126, 118, 108, 113, 122, 135, 138, 134, 125, 122, 123, 129, 134, 135, 134, 133, 130, 130, 128, 124, 111, 104, 114, 129, 142, 143, 134, 124, 121, 125, 129, 133, 136, 135, 132, 131, 130, 124, 116, 105, 112, 125, 137, 144, 139, 129, 125, 125, 125, 130, 134, 137, 134, 133, 132, 129, 121, 106, 101, 115, 132, 144, 140, 131, 123, 122, 124, 130, 135, 138, 136, 131, 132, 127, 123, 110, 100, 109, 125, 140, 144, 136, 129, 123, 121, 126, 131, 136, 140, 136, 134, 126, 118, 107, 99, 113, 131, 143, 143, 134, 125, 124, 120, 123, 130, 135, 138, 135, 132, 126, 118, 100, 97, 118, 137, 146, 140, 130, 124, 124, 122, 127, 129, 134, 138, 139, 132, 121, 106, 91, 108, 130, 145, 146, 137, 125, 123, 123, 126, 130, 134, 138, 136, 134, 126, 116, 95, 101, 123, 146, 151, 140, 130, 127, 128, 126, 129, 129, 133, 133, 141, 133, 123, 96, 93, 117, 144, 156, 146, 133, 126, 128, 123, 131, 130, 134, 133, 139, 130, 121, 97, 93, 115, 142, 153, 144, 129, 126, 131, 131, 133, 133, 132, 126, 136, 131, 122, 93, 96, 119, 140, 147, 142, 130, 128, 130, 132, 130, 133, 134, 133, 133, 128, 114, 88, 105, 130, 148, 142, 132, 126, 126, 128, 134, 134, 133, 132, 135, 130, 121, 94, 93, 123, 144, 152, 133, 122, 122, 130, 133, 135, 133, 133, 130, 129, 123, 106, 90, 112, 139, 152, 138, 121, 122, 128, 133, 134, 135, 133, 133, 124, 128, 113, 91, 109, 141, 153, 143, 122, 119, 126, 133, 135, 136, 135, 134, 125, 121, 109, 90, 111, 141, 160, 148, 123, 112, 126, 132, 134, 141, 135, 135, 122, 120, 103, 88, 119, 149, 162, 140, 120, 114, 125, 139, 137, 141, 137, 136, 121, 116, 91, 88, 125, 153, 165, 135, 117, 111, 130, 135, 139, 139, 139, 135, 120, 113, 76, 103, 136, 159, 153, 131, 115, 116, 134, 133, 141, 132, 143, 125, 122, 97, 76, 120, 145, 162, 139, 128, 113, 125, 132, 137, 142, 138, 140, 121, 115, 70, 98, 139, 162, 151, 125, 118, 116, 137, 130, 143, 137, 147, 122, 124, 87, 80, 133, 157, 162, 125, 122, 111, 143, 128, 142, 135, 142, 126, 122, 101, 72, 134, 148, 169, 126, 127, 109, 136, 134, 143, 144, 139, 134, 113, 112, 65, 134, 143, 171, 129, 132, 116, 128, 140, 134, 148, 137, 135, 105, 115, 63, 143, 143, 163, 124, 129, 124, 134, 141, 127, 146, 142, 136, 103, 112, 54, 146, 147, 169, 117, 128, 122, 138, 136, 131, 145, 142, 138, 105, 106, 49, 157, 146, 165, 106, 134, 121, 148, 133, 137, 131, 145, 131, 110, 97, 61, 165, 147, 165, 99, 135, 115, 158, 131, 146, 130, 153, 116, 115, 67, 93, 177, 153, 146, 99, 136, 121, 163, 126, 149, 134, 155, 104, 116, 52, 134, 170, 161, 125, 104, 127, 135, 160, 133, 149, 140, 140, 109, 93, 58, 175, 164, 157, 103, 120, 117, 152, 149, 143, 145, 147, 116, 113, 52, 119, 191, 156, 135, 98, 131, 113, 162, 129, 158, 132, 154, 111, 114, 44, 142, 182, 156, 126, 104, 129, 123, 164, 128, 153, 140, 127, 118, 44, 114, 198, 154, 131, 103, 134, 114, 150, 137, 145, 154, 135, 119, 85, 48, 172, 174, 144, 124, 114, 126, 114, 162, 126, 166, 135, 126, 107, 41, 134, 196, 139, 130, 108, 140, 108, 143, 139, 148, 161, 111, 130, 39, 109, 195, 152, 129, 114, 133, 108, 128, 147, 138, 174, 117, 133, 52, 69, 201, 162, 138, 115, 134, 118, 113, 144, 134, 172, 123, 124, 79, 46, 198, 169, 137, 120, 133, 125, 102, 145, 131, 172, 135, 119, 96, 28, 172, 185, 138, 127, 130, 133, 91, 145, 133, 168, 143, 113, 115, 20, 152, 199, 133, 125, 129, 142, 91, 140, 132, 159, 154, 109, 121, 24, 120, 217, 133, 132, 118, 152, 92, 131, 139, 148, 171, 92, 142, 8, 121, 212, 138, 127, 122, 159, 89, 129, 136, 151, 164, 101, 137, 37, 75, 221, 143, 130, 122, 152, 101, 109, 154, 134, 185, 89, 142, 39, 59, 224, 146, 130, 116, 162, 98, 105, 148, 137, 179, 103, 133, 68, 25, 213, 160, 124, 121, 153, 118, 82, 164, 125, 188, 109, 123, 83, 9, 209, 171, 123, 120, 148, 132, 72, 162, 120, 190, 115, 116, 97, 0, 215, 166, 122, 117, 158, 134, 68, 161, 120, 180, 128, 108, 115, 0, 190, 186, 111, 130, 143, 153, 58, 160, 128, 175, 141, 96, 131, 0, 192, 185, 115, 123, 144, 156, 60, 152, 129, 167, 150, 94, 139, 0, 157, 208, 105, 133, 132, 169, 60, 147, 141, 151, 168, 82, 151, 0, 139, 222, 103, 135, 131, 175, 64, 135, 146, 146, 181, 78, 157, 0, 115, 235, 96, 142, 124, 181, 64, 126, 155, 131, 188, 78, 164, 0, 99, 248, 94, 137, 125, 178, 71, 117, 158, 129, 194, 83, 157, 6, 74, 255, 88, 147, 121, 181, 73, 107, 170, 116, 202, 77, 162, 0, 95, 248, 83, 147, 126, 175, 72, 110, 161, 121, 200, 76, 156, 0, 110, 240, 75, 149, 128, 175, 65, 117, 151, 129, 189, 85, 151, 0, 111, 242, 69, 156, 127, 172, 64, 125, 149, 131, 186, 80, 137, 0, 205, 173, 79, 160, 137, 163, 51, 167, 104, 171, 147, 111, 90, 2, 253, 107, 111, 155, 150, 134, 63, 179, 83, 205, 115, 140, 28, 54, 254, 49, 158, 142, 168, 84, 114, 160, 88, 217, 95, 142, 0, 170, 210, 31, 192, 134, 173, 47, 175, 109, 141, 181, 104, 96, 0, 255, 69, 92, 188, 153, 125, 60, 205, 67, 202, 123, 136, 2, 115, 255, 0, 184, 156, 177, 43, 159, 140, 103, 198, 109, 91, 0, 254, 84, 50, 198, 164, 120, 59, 207, 69, 185, 141, 131, 0, 151, 255, 0, 165, 170, 174, 40, 166, 131, 111, 179, 135, 41, 26, 254, 45, 81, 175, 190, 88, 92, 189, 80, 171, 142, 118, 0, 255, 176, 12, 163, 180, 148, 49, 194, 93, 152, 138, 151, 0, 189, 248, 4, 150, 151, 192, 36, 181, 120, 123, 144, 162, 15, 60, 255, 15, 115, 141, 191, 68, 125, 170, 105, 154, 162, 66, 30, 249, 61, 83, 148, 149, 107, 86, 171, 109, 140, 141, 116, 0, 255, 136, 39, 160, 168, 140, 92, 177, 112, 144, 100, 138, 0, 255, 198, 18, 169, 156, 151, 73, 172, 107, 147, 106, 138, 0, 246, 220, 3, 163, 135, 149, 72, 171, 107, 153, 117, 149, 0, 245, 231, 12, 158, 152, 141, 90, 170, 106, 152, 105, 129, 0, 255, 204, 56, 141, 167, 127, 108, 157, 104, 156, 115, 83, 2, 254, 138, 87, 130, 171, 105, 141, 150, 123, 147, 129, 29, 56, 249, 71, 128, 136, 161, 69, 173, 139, 126, 133, 146, 1, 181, 230, 49, 164, 152, 137, 68, 213, 106, 126, 155, 71, 5, 249, 137, 71, 169, 177, 86, 111, 184, 119, 123, 153, 5, 129, 255, 52, 138, 157, 175, 39, 171, 156, 103, 172, 48, 7, 255, 104, 83, 163, 165, 113, 65, 199, 115, 124, 140, 8, 183, 249, 14, 173, 141, 169, 74, 141, 155, 96, 177, 0, 74, 254, 59, 110, 157, 164, 123, 100, 164, 86, 192, 11, 24, 250, 141, 52, 177, 160, 114, 119, 160, 73, 175, 76, 13, 255, 177, 74, 145, 171, 100, 137, 148, 93, 133, 125, 12, 228, 187, 73, 153, 163, 110, 113, 172, 91, 102, 156, 18, 190, 228, 38, 181, 151, 124, 104, 163, 123, 67, 163, 23, 168, 245, 23, 184, 150, 138, 99, 152, 142, 36, 204, 0, 181, 231, 42, 175, 132, 151, 106, 168, 117, 54, 181, 3, 209, 210, 53, 183, 119, 163, 94, 186, 115, 69, 152, 0, 255, 168, 63, 183, 134, 139, 90, 214, 84, 94, 106, 0, 255, 112, 80, 207, 133, 115, 109, 209, 57, 134, 30, 27, 255, 59, 107, 182, 146, 106, 128, 199, 54, 131, 0, 110, 252, 23, 155, 158, 161, 73, 156, 184, 28, 139, 5, 252, 224, 26, 191, 125, 168, 77, 176, 135, 62, 91, 0, 255, 101, 80, 199, 120, 152, 85, 218, 50, 117, 5, 58, 255, 3, 168, 148, 145, 120, 124, 194, 33, 127, 0, 255, 241, 16, 206, 113, 164, 92, 194, 86, 92, 30, 38, 253, 92, 121, 156, 149, 109, 157, 158, 53, 114, 2, 234, 223, 60, 183, 119, 157, 71, 249, 61, 91, 7, 72, 255, 69, 179, 113, 184, 56, 198, 153, 53, 73, 8, 253, 113, 132, 175, 123, 128, 88, 244, 67, 95, 0, 221, 242, 10, 247, 108, 162, 39, 239, 118, 62, 31, 19, 255, 39, 174, 148, 165, 85, 116, 236, 42, 45, 14, 255, 105, 109, 200, 103, 154, 71, 235, 56, 74, 3, 255, 198, 52, 233, 99, 175, 61, 210, 97, 68, 1, 186, 252, 29, 221, 131, 134, 106, 176, 122, 77, 5, 104, 240, 61, 192, 141, 142, 90, 176, 137, 74, 0, 74, 243, 32, 199, 143, 146, 94, 170, 142, 64, 37, 29, 243, 55, 169, 183, 130, 93, 152, 169, 51, 45, 23, 229, 87, 148, 180, 137, 125, 125, 182, 50, 50, 12, 245, 90, 141, 180, 134, 125, 127, 196, 42, 70, 0, 255, 104, 125, 183, 132, 137, 106, 218, 27, 74, 0, 255, 121, 109, 214, 109, 155, 95, 200, 46, 51, 17, 242, 105, 113, 223, 85, 151, 112, 215, 42, 57, 0, 255, 135, 72, 232, 106, 139, 110, 238, 30, 37, 24, 241, 122, 98, 196, 115, 149, 104, 232, 37, 46, 15, 255, 103, 106, 211, 102, 140, 119, 228, 33, 41, 0, 255, 91, 102, 232, 87, 123, 132, 248, 15, 52, 0, 255, 109, 95, 205, 114, 132, 114, 255, 17, 37, 0, 255, 88, 109, 203, 124, 122, 110, 252, 17, 44, 0, 255, 90, 118, 193, 114, 136, 91, 255, 26, 29, 0, 255, 74, 123, 207, 96, 134, 110, 255, 11, 23, 4, 253, 56, 126, 202, 98, 144, 107, 253, 8, 39, 0, 239, 79, 120, 202, 97, 145, 95, 255, 22, 18, 4, 250, 69, 121, 220, 79, 141, 110, 249, 19, 22, 0, 255, 58, 107, 223, 104, 121, 96, 255, 8, 26, 0, 255, 40, 124, 222, 80, 158, 82, 255, 30, 33, 0, 255, 80, 81, 229, 86, 162, 80, 255, 32, 25, 0, 255, 103, 62, 249, 90, 156, 69, 255, 28, 37, 0, 255, 130, 42, 254, 72, 184, 62, 255, 19, 49, 0, 255, 152, 19, 255, 58, 184, 60, 255, 42, 43, 3, 253, 216, 0, 255, 71, 196, 53, 251, 68, 40, 15, 246, 255, 0, 255, 77, 179, 65, 242, 68, 50, 0, 212, 255, 0, 255, 86, 176, 65, 228, 93, 48, 0, 185, 255, 0, 255, 111, 171, 80, 212, 108, 49, 0, 115, 255, 0, 239, 137, 167, 77, 196, 154, 19, 9, 65, 255, 0, 226, 159, 142, 112, 167, 165, 26, 37, 5, 255, 0, 185, 188, 125, 129, 132, 202, 19, 53, 0, 255, 13, 130, 222, 101, 156, 103, 216, 22, 77, 0, 255, 90, 71, 240, 89, 176, 79, 229, 23, 84, 0, 255, 149, 15, 255, 84, 178, 73, 227, 39, 81, 0, 255, 222, 0, 255, 79, 165, 64, 224, 65, 69, 0, 218, 255, 0, 254, 107, 165, 70, 208, 95, 65, 0, 146, 255, 0, 233, 141, 153, 90, 189, 121, 69, 0, 88, 255, 0, 213, 186, 126, 112, 163, 143, 74, 0, 13, 255, 6, 148, 215, 118, 142, 109, 179, 59, 50, 0, 255, 70, 83, 239, 103, 179, 76, 191, 69, 86, 0, 255, 152, 18, 247, 77, 181, 85, 201, 80, 80, 0, 255, 242, 2, 244, 96, 176, 82, 184, 113, 70, 0, 139, 251, 1, 229, 146, 151, 100, 131, 162, 65, 0, 26, 253, 8, 165, 211, 108, 156, 82, 188, 77, 44, 0, 255, 69, 80, 251, 79, 195, 43, 204, 78, 76, 0, 255, 183, 6, 252, 74, 211, 28, 205, 108, 86, 0, 209, 255, 0, 251, 119, 188, 50, 159, 158, 75, 0, 81, 255, 0, 215, 195, 153, 107, 85, 220, 60, 0, 0, 255, 7, 100, 255, 101, 165, 48, 251, 54, 61, 0, 255, 154, 0, 255, 88, 199, 18, 243, 96, 74, 0, 229, 255, 0, 251, 117, 198, 33, 174, 164, 65, 0, 83, 255, 0, 203, 201, 154, 79, 103, 232, 51, 0, 0, 255, 33, 68, 255, 109, 125, 63, 255, 65, 42, 0, 255, 186, 0, 255, 104, 169, 44, 217, 121, 57, 7, 196, 255, 0, 249, 161, 145, 83, 148, 180, 64, 0, 36, 255, 5, 140, 255, 99, 121, 89, 229, 64, 11, 0, 255, 96, 2, 255, 82, 182, 45, 224, 116, 33, 0, 246, 255, 0, 253, 113, 173, 55, 180, 187, 41, 0, 117, 255, 0, 239, 210, 119, 110, 105, 228, 58, 0, 10, 247, 10, 92, 243, 84, 155, 55, 244, 89, 13, 0, 255, 172, 16, 232, 105, 171, 39, 208, 143, 38, 0, 222, 255, 0, 237, 177, 142, 86, 145, 200, 58, 0, 77, 252, 0, 161, 241, 101, 115, 82, 243, 65, 0, 17, 255, 46, 24, 242, 84, 124, 66, 247, 89, 21, 0, 255, 213, 27, 231, 125, 137, 69, 193, 149, 48, 0, 223, 248, 18, 230, 206, 114, 100, 130, 208, 77, 0, 92, 245, 17, 129, 255, 87, 118, 91, 230, 85, 0, 19, 255, 52, 0, 255, 85, 111, 80, 229, 111, 0, 0, 255, 202, 0, 255, 123, 98, 77, 199, 166, 0, 5, 255, 245, 0, 255, 207, 85, 97, 148, 209, 29, 0, 143, 252, 0, 182, 255, 54, 101, 127, 221, 58, 0, 19, 253, 0, 37, 255, 75, 105, 108, 224, 88, 0, 0, 255, 100, 0, 255, 107, 93, 108, 202, 122, 14, 0, 255, 228, 4, 253, 187, 63, 102, 181, 151, 30, 6, 250, 255, 4, 243, 255, 57, 107, 154, 178, 58, 0, 126, 248, 7, 172, 253, 52, 95, 149, 185, 79, 0, 34, 249, 0, 52, 246, 59, 65, 165, 182, 75, 0, 23, 245, 56, 19, 250, 92, 68, 151, 188, 93, 11, 11, 249, 168, 26, 251, 137, 55, 132, 187, 88, 43, 3, 239, 254, 19, 246, 202, 60, 111, 191, 100, 61, 0, 235, 255, 0, 255, 230, 49, 103, 200, 108, 69, 3, 168, 255, 0, 226, 241, 55, 85, 202, 138, 47, 10, 94, 255, 0, 152, 246, 29, 84, 205, 146, 35, 23, 19, 248, 0, 51, 255, 49, 73, 185, 166, 35, 15, 2, 243, 0, 0, 255, 65, 44, 192, 166, 36, 5, 4, 253, 90, 0, 255, 105, 29, 182, 180, 42, 17, 2, 254, 187, 0, 255, 153, 24, 153, 209, 49, 38, 0, 255, 255, 0, 255, 229, 10, 135, 229, 57, 46, 0, 249, 255, 0, 244, 255, 2, 111, 239, 81, 42, 0, 247, 255, 1, 239, 255, 13, 95, 233, 89, 58, 0, 166, 249, 0, 146, 255, 20, 59, 243, 107, 43, 0, 87, 248, 3, 47, 255, 49, 21, 245, 128, 47, 0, 27, 253, 3, 0, 255, 101, 0, 236, 155, 55, 0, 4, 247, 28, 0, 252, 134, 0, 232, 173, 35, 0, 9, 254, 117, 2, 253, 180, 0, 217, 195, 27, 0, 15, 255, 193, 2, 255, 209, 0, 195, 199, 33, 34, 8, 255, 240, 9, 253, 255, 4, 153, 237, 25, 38, 0, 255, 244, 8, 247, 255, 11, 107, 246, 45, 50, 0, 251, 255, 0, 220, 255, 7, 80, 247, 56, 53, 0, 219, 243, 6, 153, 250, 19, 50, 253, 63, 40, 12, 166, 242, 5, 99, 255, 44, 24, 255, 75, 31, 15, 107, 246, 0, 46, 255, 58, 17, 255, 79, 34, 5, 63, 243, 0, 2, 255, 106, 10, 255, 120, 23, 8, 32, 239, 7, 2, 248, 163, 13, 245, 156, 9, 2, 14, 246, 59, 0, 248, 191, 2, 222, 163, 32, 5, 0, 249, 140, 0, 255, 228, 3, 203, 183, 19, 20, 0, 252, 211, 0, 249, 255, 4, 163, 221, 10, 24, 2, 248, 253, 0, 247, 255, 8, 135, 237, 8, 26, 0, 245, 255, 2, 255, 250, 7, 123, 245, 12, 35, 0, 243, 251, 5, 217, 254, 10, 107, 247, 9, 29, 4, 242, 244, 7, 172, 255, 24, 81, 251, 23, 38, 6, 237, 253, 7, 135, 255, 43, 50, 248, 34, 30, 8, 237, 255, 9, 104, 252, 69, 25, 255, 49, 22, 5, 205, 250, 11, 70, 251, 101, 9, 255, 68, 20, 16, 156, 244, 8, 18, 246, 139, 8, 255, 102, 1, 22, 124, 246, 5, 1, 242, 186, 7, 230, 137, 8, 19, 80, 253, 29, 4, 252, 218, 5, 216, 160, 1, 6, 40, 255, 95, 0, 254, 248, 4, 177, 185, 17, 0, 7, 255, 149, 0, 251, 252, 0, 133, 209, 24, 6, 6, 255, 204, 0, 254, 250, 3, 95, 224, 34, 6, 11, 255, 252, 0, 220, 244, 22, 76, 233, 44, 5, 5, 255, 250, 0, 167, 243, 54, 37, 238, 71, 16, 2, 255, 250, 0, 106, 248, 102, 9, 239, 95, 12, 11, 241, 253, 0, 41, 251, 149, 13, 221, 122, 28, 7, 180, 254, 0, 0, 255, 204, 8, 188, 154, 35, 7, 112, 255, 19, 0, 255, 242, 13, 145, 181, 50, 1, 42, 255, 100, 0, 255, 245, 11, 95, 208, 72, 0, 12, 255, 179, 0, 255, 242, 20, 57, 218, 89, 2, 3, 255, 250, 2, 196, 248, 52, 20, 222, 116, 0, 5, 255, 251, 0, 140, 255, 103, 0, 216, 138, 0, 3, 255, 245, 0, 86, 254, 158, 0, 194, 159, 18, 7, 239, 252, 0, 25, 254, 200, 0, 187, 165, 17, 10, 223, 251, 0, 10, 249, 232, 2, 159, 169, 51, 7, 157, 251, 8, 5, 247, 255, 2, 129, 183, 64, 3, 115, 255, 33, 7, 246, 255, 3, 109, 190, 74, 1, 82, 255, 70, 10, 248, 246, 13, 94, 184, 82, 3, 29, 252, 141, 8, 236, 253, 17, 66, 200, 87, 2, 17, 250, 166, 5, 213, 252, 45, 52, 194, 99, 6, 3, 245, 195, 7, 197, 245, 59, 37, 202, 98, 0, 10, 248, 219, 10, 165, 235, 92, 27, 185, 99, 7, 6, 253, 236, 9, 152, 247, 92, 22, 188, 95, 4, 7, 255, 244, 6, 138, 250, 108, 24, 180, 105, 9, 0, 255, 252, 0, 134, 255, 105, 12, 180, 107, 5, 0, 255, 252, 0, 126, 254, 122, 10, 175, 103, 6, 2, 255, 251, 0, 132, 254, 121, 10, 177, 104, 6, 1, 252, 251, 2, 128, 255, 118, 9, 174, 104, 5, 13, 255, 237, 0, 133, 255, 111, 13, 175, 99, 4, 37, 255, 212, 0, 150, 255, 100, 20, 169, 96, 8, 45, 254, 204, 0, 147, 255, 100, 21, 166, 99, 2, 65, 255, 182, 0, 167, 255, 94, 25, 159, 101, 0, 85, 255, 158, 0, 182, 255, 79, 28, 160, 106, 0, 99, 255, 143, 4, 184, 255, 77, 31, 156, 102, 0, 120, 255, 115, 1, 206, 255, 66, 39, 163, 97, 0, 135, 255, 106, 1, 205, 255, 73, 31, 151, 106, 0, 136, 255, 97, 2, 221, 255, 60, 39, 160, 95, 0, 161, 255, 80, 0, 224, 255, 67, 26, 144, 114, 0, 156, 255, 81, 0, 235, 254, 58, 41, 139, 97, 0, 179, 251, 70, 1, 236, 255, 59, 34, 137, 102, 4, 185, 250, 65, 0, 245, 255, 57, 38, 137, 85, 7, 209, 247, 50, 0, 247, 255, 55, 34, 136, 81, 10, 218, 247, 37, 5, 252, 253, 42, 40, 152, 68, 4, 232, 249, 22, 3, 255, 255, 38, 27, 156, 75, 0, 236, 252, 18, 0, 255, 252, 39, 40, 158, 55, 0, 255, 255, 5, 0, 255, 251, 38, 23, 163, 78, 0, 240, 255, 18, 0, 255, 249, 35, 20, 166, 69, 0, 250, 255, 15, 0, 255, 250, 28, 13, 169, 68, 0, 254, 255, 6, 0, 255, 250, 27, 13, 164, 63, 3, 252, 253, 7, 3, 252, 252, 28, 5, 162, 65, 4, 255, 250, 5, 4, 255, 251, 22, 10, 162, 58, 0, 255, 246, 2, 7, 254, 253, 21, 9, 163, 59, 0, 255, 245, 4, 9, 251, 251, 26, 5, 157, 67, 0, 255, 243, 3, 13, 252, 247, 23, 7, 155, 68, 0, 255, 252, 4, 11, 246, 250, 25, 8, 144, 62, 19, 254, 255, 5, 14, 243, 255, 25, 5, 141, 61, 39, 250, 255, 8, 16, 248, 255, 15, 4, 137, 56, 59, 254, 238, 5, 31, 247, 255, 12, 6, 132, 53, 75, 255, 225, 3, 42, 250, 255, 17, 3, 120, 59, 91, 254, 208, 5, 52, 246, 255, 24, 4, 119, 70, 88, 255, 217, 0, 60, 253, 251, 17, 10, 110, 64, 113, 255, 192, 0, 78, 252, 246, 22, 6, 99, 71, 145, 249, 160, 4, 101, 252, 233, 10, 6, 119, 72, 135, 251, 159, 10, 111, 248, 215, 12, 17, 103, 72, 161, 255, 131, 4, 148, 248, 189, 20, 21, 90, 82, 192, 255, 97, 4, 173, 251, 173, 7, 24, 94, 98, 199, 244, 87, 8, 192, 242, 157, 9, 35, 83, 98, 225, 239, 67, 12, 216, 249, 142, 8, 34, 73, 124, 248, 211, 34, 24, 247, 246, 114, 3, 51, 88, 125, 244, 200, 27, 53, 248, 244, 110, 2, 41, 74, 154, 255, 168, 0, 67, 255, 243, 80, 2, 54, 82, 166, 255, 152, 0, 97, 255, 240, 67, 3, 39, 79, 207, 254, 124, 0, 108, 255, 238, 38, 0, 55, 96, 228, 255, 91, 0, 142, 255, 212, 28, 0, 51, 103, 238, 254, 73, 0, 168, 254, 195, 33, 0, 43, 124, 255, 233, 37, 0, 201, 255, 169, 2, 0, 62, 147, 255, 208, 25, 14, 229, 253, 140, 0, 13, 67, 160, 255, 189, 14, 36, 247, 250, 123, 2, 14, 63, 185, 255, 162, 5, 54, 253, 251, 97, 0, 28, 84, 198, 253, 146, 3, 80, 255, 253, 82, 0, 23, 90, 229, 254, 115, 0, 102, 255, 251, 60, 0, 27, 105, 249, 251, 84, 0, 138, 255, 232, 37, 2, 35, 129, 255, 233, 59, 0, 164, 255, 217, 16, 0, 44, 154, 252, 214, 37, 6, 189, 253, 199, 1, 3, 51, 165, 252, 202, 16, 9, 213, 255, 177, 0, 4, 56, 186, 251, 187, 2, 9, 234, 255, 160, 0, 0, 77, 215, 245, 154, 2, 40, 255, 248, 136, 0, 3, 87, 220, 252, 135, 0, 63, 255, 247, 121, 6, 6, 87, 233, 253, 116, 0, 67, 255, 248, 102, 5, 4, 106, 247, 250, 97, 0, 90, 255, 248, 86, 4, 9, 116, 250, 248, 82, 1, 102, 255, 250, 69, 5, 10, 130, 249, 242, 65, 3, 123, 255, 253, 53, 0, 11, 150, 247, 231, 54, 6, 134, 255, 255, 38, 1, 8, 168, 249, 223, 35, 7, 158, 253, 255, 21, 2, 14, 181, 253, 213, 13, 3, 182, 255, 250, 2, 9, 20, 196, 252, 203, 5, 0, 198, 255, 238, 0, 12, 26, 201, 251, 197, 4, 0, 208, 253, 229, 3, 10, 29, 213, 247, 187, 5, 2, 217, 247, 225, 5, 6, 37, 221, 245, 186, 4, 1, 225, 250, 218, 0, 8, 43, 238, 244, 173, 4, 2, 232, 253, 212, 0, 3, 46, 253, 243, 159, 0, 10, 245, 251, 202, 1, 6, 46, 251, 248, 155, 0, 7, 253, 255, 197, 0, 2, 45, 252, 252, 157, 0, 0, 247, 255, 208, 2, 0, 31, 254, 255, 163, 0, 0, 239, 255, 219, 5, 0, 13, 255, 255, 174, 0, 0, 229, 255, 227, 9, 0, 0, 255, 255, 191, 0, 0, 212, 255, 237, 10, 0, 0, 255, 255, 204, 0, 3, 194, 255, 247, 13, 0, 0, 244, 255, 222, 0, 2, 176, 251, 253, 32, 4, 0, 210, 255, 245, 0, 0, 155, 252, 255, 56, 5, 0, 178, 254, 255, 25, 5, 132, 255, 249, 89, 2, 3, 134, 255, 251, 62, 9, 112, 250, 242, 126, 1, 4, 82, 255, 252, 112, 14, 85, 208, 233, 160, 23, 0, 34, 251, 251, 161, 35, 64, 174, 224, 182, 61, 0, 3, 207, 250, 205, 60, 57, 138, 199, 194, 98, 0, 0, 159, 252, 236, 96, 53, 112, 173, 202, 136, 5, 0, 110, 253, 251, 134, 63, 88, 142, 194, 168, 38, 0, 65, 245, 254, 168, 76, 69, 125, 185, 190, 68, 0, 25, 214, 253, 205, 94, 54, 106, 172, 203, 101, 4, 1, 174, 249, 240, 118, 46, 89, 159, 211, 128, 4, 2, 131, 248, 255, 143, 51, 74, 145, 209, 156, 20, 2, 85, 250, 255, 170, 60, 64, 132, 202, 179, 47, 1, 42, 234, 254, 201, 75, 54, 116, 194, 192, 82, 0, 11, 188, 255, 232, 100, 51, 96, 180, 201, 113, 2, 2, 143, 255, 249, 134, 56, 83, 162, 200, 143, 26, 0, 94, 246, 254, 170, 70, 71, 141, 194, 161, 63, 0, 49, 206, 255, 204, 89, 70, 118, 177, 171, 93, 9, 18, 149, 255, 234, 127, 73, 97, 156, 168, 119, 39, 8, 96, 224, 249, 168, 83, 85, 137, 166, 134, 71, 16, 50, 171, 245, 211, 117, 83, 109, 152, 143, 103, 36, 26, 115, 220, 239, 160, 96, 90, 132, 145, 129, 71, 24, 66, 177, 240, 204, 131, 89, 103, 137, 136, 110, 46, 38, 122, 220, 236, 168, 100, 91, 129, 140, 136, 75, 35, 71, 177, 244, 211, 129, 82, 107, 140, 152, 110, 53, 35, 119, 222, 245, 173, 95, 83, 123, 155, 143, 87, 27, 65, 173, 249, 216, 133, 76, 102, 147, 158, 119, 47, 32, 113, 221, 244, 175, 92, 79, 126, 160, 150, 85, 26, 55, 163, 246, 220, 134, 73, 100, 146, 162, 121, 52, 24, 98, 213, 244, 183, 93, 78, 125, 165, 147, 90, 26, 45, 154, 240, 227, 137, 78, 97, 153, 163, 127, 54, 28, 86, 198, 241, 191, 108, 78, 125, 159, 152, 91, 44, 41, 139, 221, 224, 158, 91, 100, 142, 160, 123, 75, 32, 77, 171, 232, 208, 129, 93, 114, 152, 147, 109, 54, 41, 109, 200, 233, 180, 116, 94, 125, 150, 135, 90, 45, 60, 141, 219, 218, 159, 103, 103, 136, 146, 120, 75, 44, 86, 172, 224, 203, 137, 93, 113, 144, 140, 112, 59, 52, 111, 193, 225, 187, 115, 92, 123, 146, 138, 93, 54, 63, 136, 211, 223, 162, 98, 96, 134, 151, 126, 81, 46, 81, 166, 225, 207, 137, 88, 107, 144, 146, 117, 65, 52, 107, 192, 225, 184, 115, 90, 121, 146, 146, 100, 56, 63, 136, 209, 217, 161, 100, 96, 131, 155, 133, 87, 50, 83, 160, 219, 204, 142, 91, 106, 144, 152, 121, 71, 55, 103, 187, 217, 189, 115, 95, 120, 153, 147, 104, 64, 58, 130, 198, 221, 160, 107, 95, 132, 154, 135, 95, 52, 75, 146, 212, 204, 147, 95, 104, 140, 149, 124, 75, 52, 93, 168, 212, 190, 130, 92, 112, 141, 144, 110, 63, 59, 113, 188, 210, 173, 109, 92, 123, 146, 135, 92, 56, 68, 135, 196, 205, 153, 101, 97, 129, 143, 125, 83, 55, 84, 153, 207, 192, 136, 94, 111, 137, 142, 114, 71, 56, 100, 175, 206, 180, 119, 98, 117, 142, 138, 104, 65, 63, 122, 183, 205, 166, 115, 103, 125, 145, 132, 93, 65, 82, 139, 190, 195, 153, 112, 110, 133, 143, 122, 89, 69, 93, 150, 194, 189, 143, 108, 114, 137, 140, 120, 84, 72, 100, 158, 195, 182, 138, 107, 116, 137, 140, 115, 85, 72, 109, 165, 194, 176, 131, 103, 118, 137, 137, 111, 79, 73, 116, 172, 193, 168, 119, 105, 122, 143, 135, 101, 72, 80, 126, 179, 189, 155, 112, 107, 129, 143, 127, 91, 71, 84, 138, 180, 182, 143, 112, 114, 134, 138, 116, 89, 70, 95, 143, 182, 175, 139, 117, 118, 129, 132, 114, 84, 77, 101, 152, 185, 171, 138, 114, 120, 133, 132, 111, 82, 78, 108, 161, 180, 169, 133, 113, 123, 135, 135, 106, 80, 79, 122, 166, 182, 161, 130, 120, 128, 139, 129, 102, 81, 86, 126, 169, 177, 160, 130, 121, 131, 137, 125, 102, 83, 93, 132, 168, 178, 159, 131, 122, 131, 136, 127, 99, 85, 95, 133, 170, 178, 157, 129, 120, 129, 138, 125, 103, 86, 95, 135, 171, 177, 158, 128, 123, 131, 134, 123, 101, 86, 98, 138, 169, 175, 152, 128, 124, 131, 134, 120, 98, 86, 102, 142, 169, 170, 146, 126, 129, 136, 134, 115, 95, 85, 105, 143, 168, 165, 142, 127, 131, 138, 133, 117, 93, 86, 104, 142, 168, 166, 146, 128, 130, 134, 133, 116, 95, 86, 101, 137, 166, 170, 149, 129, 124, 133, 135, 120, 96, 87, 98, 134, 165, 169, 152, 128, 126, 133, 136, 118, 97, 86, 98, 135, 165, 171, 151, 126, 125, 136, 136, 121, 95, 83, 96, 135, 167, 171, 151, 123, 122, 134, 137, 123, 96, 84, 93, 131, 165, 171, 151, 122, 121, 131, 140, 123, 99, 84, 90, 129, 163, 170, 150, 125, 120, 131, 136, 127, 102, 87, 90, 122, 159, 168, 153, 127, 123, 129, 136, 129, 107, 90, 91, 115, 151, 166, 157, 134, 123, 130, 134, 131, 111, 93, 88, 108, 145, 166, 164, 141, 126, 127, 132, 133, 120, 99, 90, 102, 137, 163, 164, 148, 128, 125, 129, 133, 125, 107, 93, 97, 126, 155, 167, 154, 132, 123, 127, 132, 131, 115, 98, 93, 111, 147, 167, 160, 138, 122, 124, 130, 133, 123, 103, 91, 102, 136, 162, 164, 145, 124, 120, 128, 134, 129, 110, 94, 94, 121, 154, 167, 154, 128, 117, 124, 132, 135, 120, 100, 90, 105, 141, 166, 161, 137, 118, 120, 130, 135, 128, 105, 90, 94, 125, 158, 165, 150, 125, 115, 125, 133, 133, 118, 98, 91, 106, 140, 161, 159, 137, 118, 118, 128, 134, 130, 109, 93, 96, 119, 151, 161, 149, 127, 116, 123, 133, 135, 122, 103, 96, 103, 131, 156, 156, 141, 120, 117, 127, 132, 131, 119, 103, 98, 108, 137, 157, 153, 136, 118, 117, 126, 133, 131, 116, 103, 101, 115, 143, 158, 151, 130, 116, 119, 129, 136, 131, 117, 105, 101, 118, 147, 159, 149, 126, 114, 120, 133, 138, 132, 116, 103, 102, 124, 152, 158, 144, 122, 115, 125, 136, 138, 130, 112, 102, 108, 129, 150, 152, 139, 121, 116, 127, 138, 137, 127, 113, 107, 109, 127, 147, 149, 138, 122, 120, 127, 135, 135, 127, 114, 108, 111, 126, 144, 147, 140, 125, 121, 127, 133, 136, 131, 118, 110, 109, 124, 144, 150, 141, 127, 120, 125, 135, 139, 133, 117, 108, 110, 123, 141, 149, 142, 128, 121, 125, 134, 137, 132, 121, 110, 110, 120, 138, 147, 143, 130, 121, 123, 131, 136, 131, 122, 112, 110, 116, 134, 147, 143, 132, 122, 121, 127, 132, 134, 127, 112, 108, 115, 133, 146, 144, 134, 121, 118, 127, 135, 135, 128, 116, 112, 116, 127, 141, 142, 133, 123, 119, 125, 133, 135, 129, 117, 112, 115, 124, 136, 138, 134, 127, 122, 127, 134, 134, 130, 121, 116, 118, 120, 129, 137, 135, 131, 124, 125, 131, 131, 130, 125, 119, 117, 117, 124, 134, 137, 134, 128, 126, 128, 130, 130, 129, 121, 117, 116, 120, 130, 136, 136, 130, 126, 127, 129, 132, 131, 122, 118, 116, 118, 127, 134, 137, 131, 125, 126, 131, 132, 131, 125, 120, 118, 117, 123, 133, 135, 133, 131, 128, 128, 128, 129, 126, 120, 116, 114, 119, 130, 135, 135, 130, 126, 127, 129, 130, 129, 124, 119, 116, 118, 124, 131, 136, 135, 128, 124, 126, 129, 130, 123, 117, 114, 116, 124, 135, 138, 134, 129, 126, 130, 130, 130, 126, 121, 119, 119, 124, 131, 137, 136, 129, 125, 125, 127, 129, 125, 120, 118, 119, 122, 130, 137, 136, 131, 127, 127, 128, 129, 126, 123, 122, 121, 122, 130, 137, 138, 132, 126, 125, 126, 129, 128, 126, 122, 121, 124, 127, 135, 137, 133, 128, 125, 127, 130, 130, 125, 124, 123, 125, 128, 131, 133, 132, 129, 126, 127, 127, 129, 126, 124, 124, 124, 123, 125, 131, 133, 131, 127, 125, 125, 127, 127, 125, 122, 121, 121, 122, 127, 130, 130, 128, 125, 124, 126, 128, 125, 123, 121, 120, 120, 122, 127, 129, 128, 127, 125, 124, 126, 126, 125, 123, 121, 119, 121, 124, 128, 130, 129, 125, 124, 126, 129, 129, 125, 121, 118, 120, 124, 128, 130, 130, 127, 127, 127, 130, 132, 128, 125, 121, 121, 125, 127, 129, 130, 128, 128, 129, 130, 132, 131, 127, 125, 123, 124, 127, 130, 129, 128, 129, 129, 130, 131, 133, 130, 127, 125, 124, 123, 126, 128, 128, 129, 128, 129, 129, 130, 131, 129, 126, 122, 125, 126, 127, 128, 127, 128, 126, 128, 130, 130, 129, 128, 125, 124, 125, 126, 127, 126, 126, 126, 127, 128, 129, 128, 127, 127, 126, 125, 125, 126, 126, 126, 126, 127, 127, 127, 127, 126, 128, 127, 126, 126, 124, 125, 125, 126, 126, 126, 126, 126, 127, 128, 128, 126, 127, 125, 125, 126, 127, 127, 126, 125, 125, 126, 128, 128, 128, 126, 125, 126, 127, 127, 127, 127, 126, 126, 127, 127, 128, 128, 127, 127, 127, 127, 127, 128, 126, 127, 127, 127, 128, 127, 128, 128, 128, 128, 128, 127, 127, 128, 127, 127, 127, 127, 128, 128, 129, 129, 128, 128, 127, 128, 128, 128, 128, 128, 127, 128, 129, 129, 128, 128, 128, 128, 128, 128, 128, 127, 127, 127, 128, 128, 127, 128, 127, 127, 128, 128, 128, 127, 127, 127, 128, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 127, 126, 127, 127, 127, 127, 126, 127, 127, 128, 127, 127, 127, 126, 127, 126, 127, 126, 126, 127, 127, 127, 128, 127, 127, 127, 127, 128, 127, 126, 126, 126, 127, 127, 127, 127, 127, 127, 128, 128, 127, 127, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 128, 127, 126, 127, 126, 127, 127, 127, 128, 126, 127, 126, 127, 128, 127, 127, 126, 126, 127, 127, 127, 127, 126, 126, 126, 127, 127, 127, 126, 126, 127, 127, 127, 127, 126, 126, 126, 127, 127, 127, 127, 126, 126, 127, 128, 127, 127, 127, 126, 127, 127, 127, 127, 126, 126, 127, 127, 128, 127, 127, 127, 127, 127, 128, 128, 127, 126, 127, 127, 128, 128, 127, 127, 127, 127, 128, 128, 128, 127, 127, 127, 127, 128, 127, 127, 127, 127, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 127, 127, 127, 128, 128, 128, 128, 128, 128, 128, 128, 128, 127, 127, 127, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 127, 127, 127, 127, 128, 127, 127, 127, 128, 128, 128, 128, 127, 127, 128, 128, 128, 127, 127, 127, 127, 128, 128, 128, 128, 127, 128, 128, 128, 127, 127, 127, 127, 127, 127, 128, 127, 127, 127, 128, 128, 128, 128, 127, 127, 127,
};

// Servo control flag
bool isServo1Active = true;  // Flag to alternate between servos

// Setup function
void setup() {
  pixels.begin();       // Initialize NeoPixel
  servo1.attach(9);     // Attach servo1 to pin 9
  servo2.attach(10);    // Attach servo2 to pin 10
}

// Randomly set colors for NeoPixel
void ColourSettings() {
  //red = random(0, 255);
  //green = random(0, 255);
  blue = 128;
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

// Main loop function
void loop() {
  neoPixel();              // Update NeoPixel colors
  handlePauseAndAudio();   // Manage servo pause and audio playback
  updateServos();          // Control servos
}
