#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "subsidary.h"

#include "time_stone.h"
#include "mind_stone.h"

// Declare OLED operations
#define SCREEN_WIDTH 128  // OLED Display Width
#define SCREEN_HEIGHT 64 // OLED Display Height

// Define pins
#define SET 25      // START/STOP Button
#define TIME 26     // Time Increment Button
#define MODE 27     // Choose Mode for Timer

// Declaration for a I2C SSD1306 Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initial Declaration
void clear();
void OLED(int state);
void chooseMode(int num);

int minutes(int counter);
int hours(int counter);

void setup() {
  pinMode(SET, INPUT_PULLDOWN);
  pinMode(TIME, INPUT_PULLDOWN);
  pinMode(MODE, INPUT_PULLDOWN);


  attachInterrupt(SET, &toggle_timer, RISING);
  attachInterrupt(TIME, &time_incr, RISING);
  attachInterrupt(MODE, &time_decr, RISING);
  

  init_timer();
  init_ble();

  Serial.begin(115200);
  Serial.println("BLE advertising...");

  // Set Initial Values
  resetStates();

  // Connect through I2C to the OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){ // Address for Screen Size
    Serial.println(F("SSD1306 allocation failed"));
    // Hang
    while(true);
  }
  
  // Clear previous entries
  delay(2000);
  clear();
}

void loop() {
  clear();    // Clear Display
  int oledState = 0;
  pressed[0] = digitalRead(SET);    // Read START/STOP Button State
  pressed[1] = digitalRead(TIME);  // User chooses TIME
  pressed[2] = digitalRead(MODE);  // User chooses MODE

  if (pressed[0]){    // START/STOP Button
    // Re-read from this pin to see if hit again.
    HEAD_PAT = true;
    // Log values
    MODE_SELECT = findMode(mosh-1); 
    mosh = 0;
    count = 0;
    int HOUR, MIN, SEC;

    display.setTextSize(3);
    display.setTextColor(WHITE);

    while (time_left >= 0){
      pressed[0] = digitalRead(SET);
      // Get logistics -- Variables to be editted later
      int16_t x1;
      int16_t y1;
      uint16_t width;
      uint16_t height;

      // Break-it-out
      if (pressed[0]){
        clear();
        display.getTextBounds(":(", 0, 0, &x1, &y1, &width, &height);
        display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
        display.print(":(");
        display.display();
        delay(5000);
        QUITTER = true;
        break;
      }
      // Clear Screen
      clear();

      // Split the time into identifiable factors
      int temp = time_left;
      HOUR = temp / 3600;
      temp = (temp % 3600);
      MIN = temp / 60;
      temp = (temp % 60);
      SEC = temp;

      // Make it a string
      String hourStr = String(HOUR);
      String minStr = String(MIN);
      String secStr = String (SEC);
      String time;
      time += hourStr;
      time += ":";
      time += minStr;
      time += ":";
      time += secStr;

      display.getTextBounds(time, 0, 0, &x1, &y1, &width, &height);
      display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
      display.print(time);
      display.display();
    }
    if (!QUITTER){
      OLED(1);
    }
    // Reset Main Button to Base
    HEAD_PAT = false;
  }
  if (pressed[1] && HEAD_PAT == false){    // TIME Selection
    MODE_SELECT = findMode(mosh-1);
    OLED(2);
    countUp();
  }
  if (pressed[2] && HEAD_PAT == false){    // MODE
    count = 0;
    OLED(3);
    upMode();
  }

  if (deviceConnected) {
    uint32_t value = time_left;
    time_elapsed_CHAR->setValue(value);
  }
}

/// Function Declarations for use interior to design are declared HERE.
// Clear Function
void clear(){
  display.clearDisplay();
}

// Operate OLED
void OLED(int state){
  clear();

  // Get logistics -- Variables to be editted later
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  if(state == 1){     // If the START/STOP Button is pressed
    // Condition the font
    display.setTextSize(3);
    display.setTextColor(WHITE);
    // Display a Happy Message when the timer is up
    display.getTextBounds("WooHoo!", 0, 0, &x1, &y1, &width, &height);
    display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
    display.print("WooHoo!");
    display.display();
    delay(2000);
  }
  else if(state == 2){    // If TIMER is pressed
    display.setTextSize(3);
    display.setTextColor(WHITE);
    // Interpret the count - Each integer represents 10 minutes.
    String HOURS = String(hours(count));
    String MINS = String(minutes(count));
    String time;
    time += HOURS;
    time += ":";
    time += MINS;
    time += ":00";
    display.getTextBounds(time, 0, 0, &x1, &y1, &width, &height);
    display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
    display.print(time);
    display.display();
  }
  else if(state == 3){    // IF MODE is pressed
    display.setTextSize(2);
    display.setTextColor(WHITE);
    // Grab data of MODE
    display.getTextBounds("MODE:", 0, 0, &x1, &y1, &width, &height);
    // Display in desired location
    display.setCursor((SCREEN_WIDTH - width)/2, 0);
    display.print("MODE:");
    display.setTextSize(1);
    display.setCursor(10, SCREEN_HEIGHT/2);
    display.print("STUDY");
    display.setCursor(10, SCREEN_HEIGHT-10);
    display.print("CHORES");
    display.getTextBounds("EXERCISE", 0, 0, &x1, &y1, &width, &height);
    display.setCursor(SCREEN_WIDTH - (SCREEN_WIDTH - width)/2 - 15, SCREEN_HEIGHT/2);
    display.print("EXERCISE");
    display.getTextBounds("SELF-CARE", 0, 0, &x1, &y1, &width, &height);
    display.setCursor(SCREEN_WIDTH - (SCREEN_WIDTH - width)/2 - 20, SCREEN_HEIGHT-10);
    display.print("SELF-CARE");
    chooseMode(mosh);
    display.display();
  }
  else{
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("No Button Pressed!");
    display.display();
  }
}

void chooseMode(int num){
  // Get logistics -- Variables to be editted later
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;
  display.setTextSize(1);

  switch(num){
    case 0:
    // Grab data of MODE
    display.setCursor(5, SCREEN_HEIGHT/2);
    display.print(">");
    break;
    case 1:
    display.setCursor(5, SCREEN_HEIGHT-10);
    display.print(">");
    break;
    case 2:
    display.getTextBounds("EXERCISE", 0, 0, &x1, &y1, &width, &height);
    display.setCursor(SCREEN_WIDTH - (SCREEN_WIDTH - width)/2 - 20, SCREEN_HEIGHT/2);
    display.print(">");
    break;
    case 3:
    display.getTextBounds("SELF-CARE", 0, 0, &x1, &y1, &width, &height);
    display.setCursor(SCREEN_WIDTH - (SCREEN_WIDTH - width)/2 - 25, SCREEN_HEIGHT-10);
    display.print(">");
    break;
    default:
    break;
  }
}
