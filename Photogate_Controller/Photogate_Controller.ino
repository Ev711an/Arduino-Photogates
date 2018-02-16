/**
 * * * * * * * * * * * * * * * * * Version 0.1.0 * * * * * * * * * * * * * * * * *
 * 
 * This Program is written for the Adafruit Feather HUZZAH (including featherwing shield) 
 * inside the control housing for the Photo Gate system.
 *
 *
 * Single Gate Mode:
 * A single photogate is plugged into the port corresponding to the
 * pin number specified under the variable photoGate1 in the Photo Gate Pin Declarations.
 * The program will start a stopwatch when an object enters the photo gate, and stop
 * the stopwatch when it exits, then it will show the time on the display.
 *
 *
 * Double Gate Mode:
 * Two photogates are plugged into the two ports, which correspond to the pin
 * number specified under the variables photoGate1 and photoGate2 in the Photo Gate
 * Pin Declarations. The program will start a stopwatch when an object enters either
 * photogate, and stop the stopwatch when an object enters the second photogate. The
 * time will be shown on the display.
 *
 */

/**
 * * * * * * * * * * * * * * * * * Version 0.1.1 * * * * * * * * * * * * * * * * *
 * Changed Reset Button Pin to pin 2, for button c on the Wing. Fixed an error caused
 * by a while loop once the stopwatch ended, where after ended the time, the program
 * would crash and restart. Now an if statement checks for an ended state, then displays
 * a time if currently in that ended state.
 */

/**
 * These libraries are required to use the OLED display
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


/**
 * Object display declaration and definition
 */
Adafruit_SSD1306 display = Adafruit_SSD1306();

/**
 * Button Pin Declarations -
 * Change modeButton to change which pin is connnected to the mode button
 * Change resetButton to change which pin is connected to the reset button
 */
const int modeButton = 0;
const int resetButton = 2;

/**
 * Photo Gate Pin Declarations - 
 * Change these to change which pin is connected to each photogate.
 */
const int photoGate1 = 13;
const int photoGate2 = 12;

/**
 * Variables used to store Stop Watch Times, and whether or not it is running
 */
unsigned long stopWatchStart = 0;
unsigned long stopWatchEnd = 0;
unsigned long stopWatchTime = 0;
int stopWatchState = LOW;
int stopWatchEndState = LOW;

/**
 * These Variables ensure that the program only stays within its current running stop watch.
 */
int checkD1;
int checkD2;
int previousReadingS1 = HIGH;

/**
 * These are the function declarations for two functions,
 * displayTime displays the current time, and overwrites anything else on the screen
 * displayMenu displays text, and does NOT overwrite anything on the screen
 */
void displayTime(float textSize, // The Text Size
unsigned long displayNum); //Time to be displayed

void displayMenu(float textSize, // Text Size
int cursorX, //Placement of cursor, x axis
int cursorY, //Placement of cursor, y axis
char menuString[]); // String to be displayed

/**
 * TextSize Variable, change this to change the size of text in strings and numbers
 */
float TextSize = 1;

/**
 * Mode variable used for changing between singlegate(mode 1) and doublegate(mode 0) modes.
 */
int mode = 1;

/**
 * setup is called once at startup; used for any initialization code
 */
void setup() {
  /**
   * display.begin initializes the display,
   * then the screen is tested using the displayMenu function, this displays for .5 seconds
   */
  display.begin(SSD1306_SWITCHCAPVCC , 0x3C);
  display.clearDisplay(); 
  displayMenu(TextSize , 0 , 12 , "Initializing...");
   
   /*
    * Button Pin initialization
    */
   pinMode(modeButton, INPUT);
   pinMode(resetButton, INPUT);

   /*
    * Photogate Pin initialization
    */
   pinMode(photoGate1, INPUT);
   pinMode(photoGate2, INPUT);

   /**
    * Clears display
    */
   delay(500);
   display.clearDisplay();
   display.display();
}

/**
 * The loop function is called periodically after the setup function is complete; use for main code
 */
void loop() {

  /**
   * displays opening menu texts
   */
  if (stopWatchState == LOW && stopWatchEndState == LOW) {
  displayMenu(1 , 0 , 0 , "To select mode, press");
  displayMenu(1 , 0 , 9 , "A.");
  displayMenu(1 , 0 , 18 , "currently:");

  /**
   * Checks for mode button press, sets to mode 1 if the current mode is 0, and sets to mode 0 if the current mode is 1.
   */
  if (digitalRead(modeButton) == LOW && mode == 0) { //button press check
    while (digitalRead(modeButton) == LOW) { //debounce
    mode = 1;
    display.clearDisplay();
    }
  }

  if (digitalRead(modeButton) == LOW && mode == 1) { //button press check
    while (digitalRead(modeButton) == LOW) { //debounce
    mode = 0;
    display.clearDisplay();
    }
  }

  /**
   * displays the current mode after opening menu text.
   */
    switch (mode) {
      case 1 : displayMenu(1 , 60 , 18 , "SingleGate");
      break;
      case 0 : displayMenu(1 , 60 , 18 , "DoubleGate");
    }
  } 

  if (stopWatchEndState == HIGH) {
    displayTime(TextSize , stopWatchTime);
    displayMenu(1 , 0 , 9 , "Press C to");
    displayMenu(1 , 0 , 18 , "return to menu.");

    if (digitalRead(resetButton) == LOW) {
      stopWatchEndState = LOW;
      display.clearDisplay();
    }
  }
  
  if (mode == 1) { //mode check

    /**
     * Takes the int 'readingS1' from the first photogate, then checks to see if an object has past in front of it, and, if so, starts stopwatch.
     */
    int readingS1 = digitalRead(photoGate1);
    if (readingS1 == LOW && previousReadingS1 == HIGH && stopWatchEndState == LOW) { //photoGate and stopWatchState check
      stopWatchStart = millis();
      previousReadingS1 = LOW;
      stopWatchState = HIGH;
    }

   /**
    * If 'readingS1' is HIGH, and the stopwatch is running, stops the stopwatch and displays the time.
    */
    if (stopWatchState == HIGH && readingS1 == HIGH) {
      stopWatchEnd = millis();
      stopWatchTime = stopWatchEnd - stopWatchStart;
      
      previousReadingS1 = readingS1;
      stopWatchState = LOW;
      stopWatchEndState = HIGH;
     // while (digitalRead(resetButton) == HIGH) { //will stay in while until reset button is pressed
        
      //}
      display.clearDisplay();
    }
    
  }
  
  if (mode == 0) { //mode check

    /**
     * Takes the readings from the photogates.
     */
    int readingD1 = digitalRead(photoGate1);
    int readingD2 = digitalRead(photoGate2);

    /**
     * starts stopwatch if the stopwatch is not already running, and photogate1 is triggered.
     */
    if ((readingD1 == LOW) && stopWatchState == LOW && checkD2 == LOW && stopWatchEndState == LOW) {
      stopWatchStart = millis();
      stopWatchState = HIGH;
      checkD1 = HIGH;
    }
    
   /**
    * stops the stopwatch and displays time if the stopwatch isn't running, photogate2 is triggered, and photogate2 was not the photogate that started the stopwatch.
    */
    if ((readingD2 == LOW) && stopWatchState == HIGH && checkD1 == HIGH) {
      stopWatchEnd = millis();
      stopWatchTime = stopWatchEnd - stopWatchStart;
      displayTime(1 , stopWatchTime);
      stopWatchState = LOW;
      stopWatchEndState = HIGH;
      checkD1 = LOW;
      display.clearDisplay();
    }

    /**
     * Takes readings from the photogates.
     */
    readingD1 = digitalRead(photoGate1);
    readingD2 = digitalRead(photoGate2);

    /**
     * starts stopwatch if the stopwatch is not already running, and photogate2 is triggered.
     */
    if ((readingD2 == LOW) && stopWatchState == LOW && checkD1 == LOW && stopWatchEndState == LOW) {
      stopWatchStart = millis();
      stopWatchState = HIGH;
      checkD2 = HIGH;
    }
    
   /**
    * stops the stopwatch and displays time if the stopwatch isn't running, photogate1 is triggered, and photogate1 was not the photogate that started the stopwatch.
    */
    if ((readingD1 == LOW) && stopWatchState == HIGH && checkD2 == HIGH) {
      stopWatchEnd = millis();
      stopWatchTime = stopWatchEnd - stopWatchStart;
      displayTime(1 , stopWatchTime);
      stopWatchState = LOW;
      stopWatchEndState = HIGH;
      checkD2 = LOW;
      display.clearDisplay();
      
    }
    
  } 
  
}


/**
 * Function definition of displayTime
 */
void displayTime(float textSize, // IN // determines text size
unsigned long displayNum) // IN // number to be displayed
{
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(displayNum * .001);
  display.display();
}



/**
 * Function definition of displayMenu
 */
void displayMenu(float textSize, // IN // determines text sixe
int cursorX, // IN // cursor X coordinate
int cursorY, // IN // cursor Y coordinate
char menuString[]) // IN // string to be displayed
{
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(cursorX, cursorY);
  display.print(menuString);
  display.display();
}
