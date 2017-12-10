/*

  HCMS-2011/HDSP-2011/HCMS-2XXX Example with dynamic brightness
  Harrison Shutan - December 2017

  This example is an easy to use String display HCMS-2XXX library.
  The pulse timing is dynamically controlled depending on how many pixels are to be lit on each draw, creating an even overall brightness.

  Tested to work well with up to 8 cascaded displays (32 characters), potentially more.
  NOTE: To drive more than two or three displays, you will need a fast microcontroller (>16Mhz), or to use direct port control instead of digitalWrite()

  This code should work with the following displays:

  HDSP-201X/-231X/-235X

  HCMS-201X Series
  HCMS-231X
  HCMS-235X Series

  HCMS-2010 3.8 mm (0.15 inch) Standard Red
  HCMS-2011 3.8 mm (0.15 inch) Yellow
  HCMS-2012 3.8 mm (0.15 inch) High-Efficiency Red
  HCMS-2013 3.8 mm (0.15 inch) High-Performance Green
  HCMS-2310 5.0 mm (0.20 inch) Standard Red
  HCMS-2311 5.0 mm (0.20 inch) Yellow
  HCMS-2312 5.0 mm (0.20 inch) High-Efficiency Red
  HCMS-2313 5.0 mm (0.20 inch) High-Performance Green
  HCMS-2314 5.0 mm (0.20 inch) Orange
  HCMS-2351 5.0 mm (0.20 inch) Yellow
  HCMS-2352 5.0 mm (0.20 inch) High-Efficiency Red
  HCMS-2353 5.0 mm (0.20 inch) High-Performance Green
  HCMS-2354 5.0 mm (0.20 inch) Orange

*/

#include "font.h"

#define displayCharacterCount 4 // The number of characters. A single display has 4. Two displays have 8, etc.

float brightnessFactor = 5.0; // Test a wide range of values with your hardware configuration.

const byte vBlank = 4; // Pin connected to Vb pin on display(s)
const byte clockPin = 5; // Pin connected to the Clock pin on display(s)
const byte dataPin = 19; // Pin connected to the Data in pin on the display
const byte columnPin[] = { 21, 23, 25, 27, 33 }; // Pins for column PNP drivers, 1 through 5 in order

void setup() {
  // Initial setup for pins
  pinMode(vBlank, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(vBlank, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, LOW);

  // Turn off all of the PNP column drivers by setting them high
  for (int i = 0; i < 5; i++) {
    pinMode(columnPin[i], OUTPUT);
    digitalWrite(columnPin[i], HIGH);
  }

}


void loop() {
  // Show the same example text.
  showString("ABCDEFGH");
}


void showString(String inString) {
  // Default vert offset to 6 if not specified in function call
  showString(inString, 6);
}

void showString(String inString, int inVScroll) {
  // Ensure the input String is the exact number of characters in the display
  if (inString.length() < displayCharacterCount) {
    while (inString.length() < displayCharacterCount) {
      inString = inString + " ";
    }
  } else {
    inString.remove(displayCharacterCount);
  }

  // Convert input String to char array
  char charArray[displayCharacterCount + 1];
  inString.toCharArray(charArray, displayCharacterCount + 1); // +1 to leave room for the terminating char

  for (byte draws = 0; draws < 5; draws++) { // Number of times to draw the display with the same input
    for (byte column = 0; column < 5; column++) { // Loop through strobe columns (there are always only 5)
      int pixelCounter = 0;
      for (int matrix = displayCharacterCount; matrix > 0; --matrix) { // Loop through each of the char matrix
        for (byte row = 0; row < 7; row++) { // Loop through each of the rows (there are always only 7)
          digitalWrite(clockPin, HIGH); // clockPin HIGH
          if (bitRead(font[charArray[matrix - 1]][column], (inVScroll - row))) {
            digitalWrite(dataPin, HIGH); // dataPin HIGH
            pixelCounter++; // Count each on pixel
          } else {
            digitalWrite(dataPin, LOW); // dataPin LOW
          }
          delayMicroseconds(1); // This delay is important to prevent display corruption with fast signaling and 3.3v microcontrollers
          digitalWrite(clockPin, LOW); // clockPin LOW
        }
      }
      digitalWrite(vBlank, HIGH); // Enable the display
      digitalWrite(columnPin[column], LOW); // Enable the PNP column driver
      // The next delayMicroseconds is a dynamic method of determining how long to pulse a column on.
      // The more pixels that are on, the more time and current will be needed to light them.
      // This method creates a more even brightness compared to a static pulse delay, however it is less flexible in controlling overall brightness.
      delayMicroseconds(pixelCounter * brightnessFactor);
      digitalWrite(columnPin[column], HIGH); // Disable the column driver
      digitalWrite(vBlank, LOW); // Disable the display
    }
  }
}
