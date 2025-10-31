/*
  Study Buddy - FINAL ESP32 PHYSICAL BOARD CODE
  Board: ESP32
  Display: 0.96" I2C OLED (SSD1306)
  Features: Personality, Custom Modes, Timer
  
  --- WOKWI COMPATIBILITY FIX ---
  - Removed all Buzzer code.
  - Using 'analogWrite()' for Wokwi compatibility.
    
  --- MODIFICATION ---
  - Changed main lamp to an RGB LED:
    - Breathes GREEN for Focus/Deep Focus.
    - Breathes BLUE for Break/Deep Break.
  - Removed the separate "End LED" (on GPIO 17) for simplicity.
*/

// --- Include Libraries ---
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_random.h" 

// --- Pin Definitions (ESP32 GPIO numbers) ---
#define GREEN_LED_PIN     19 // GPIO19 - Breathing lamp (Green)
#define BLUE_LED_PIN      18 // GPIO18 - Breathing lamp (Blue)
#define RED_LED_PIN       5  // GPIO5  - Breathing lamp (Red)
#define LAMP_BUTTON_PIN   15 // GPIO15
#define MODE_BUTTON_PIN   2  // GPIO2
// #define END_LED_PIN       17 // --- REMOVED ---
// Note: Default I2C pins for ESP32 are SDA=21, SCL=22

// --- LEDC (PWM) Configuration ---
// const int ledFreq = 5000; // Not needed for analogWrite
const int ledResolution = 8; // 8-bit (0-255), which analogWrite uses

// --- OLED Display Setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Global Variables ---
bool isLampOn = false;
int lampBrightness = 0;
int fadeAmount = 5;
// bool isSilent = false; // Removed

// --- Personality Phrases ---
String focusPhrases[] = {
  "Let's crush this!",
  "lets do it ",
  "You got this mohan :)",
  "Time to shine!"
};
int focusPhraseCount = 4;
String breakPhrases[] = {
  "Recharge mode!",
  "Take 5...",
  "Zzz... (resting)",
  "Great work!"
};
int breakPhraseCount = 4;

// --- Timer & Mode Variables ---
String modeNames[] = {"Focus", "Break", "Deep"};
unsigned long modeDurations[] = { 12000, 5000, 15000, 7000 };
int modeCount = 3;
int currentModeIndex = 0;
unsigned long timerStartTime = 0;
bool isTimerRunning = false;

// --- Button Debounce Variables ---
int lampButtonState, lastLampButtonState = HIGH;
int modeButtonState, lastModeButtonState = HIGH;
// int silenceButtonState, lastSilenceButtonState = HIGH; // Removed
unsigned long lastLampDebounce = 0;
unsigned long lastModeDebounce = 0;
// unsigned long lastSilenceDebounce = 0; // Removed
unsigned long debounceDelay = 50;

// --- Function Prototypes (Forward Declarations) ---
// void playTone(int frequency, int duration); // Removed
void checkLampButton();
void checkModeButton();
// void checkSilenceButton(); // Removed
void checkTimer();
void endSession();
void showModeScreen();
void updateDisplay(String line1, String line2 = ""); 

// ===================================
// --- SETUP ---
// ===================================
void setup() {
  Serial.begin(115200);
  Serial.println("Study Buddy Booting Up...");

  pinMode(LAMP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  // pinMode(SILENCE_BTN_PIN, INPUT_PULLUP); // Removed

  // Setup the End LED (Removed)
  // pinMode(END_LED_PIN, OUTPUT);
  // digitalWrite(END_LED_PIN, LOW);

  // --- Configure RGB LED pins for analogWrite ---
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  Serial.println("RGB LED pins configured for analogWrite.");


  // --- Initialize OLED ---
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  Serial.println("OLED Initialized.");

  randomSeed(esp_random());
  Serial.println("Random Seed Initialized.");
  
  showModeScreen(); 
}

// ===================================
// --- MAIN LOOP ---
// ===================================
void loop() {
  checkLampButton();
  checkModeButton();
  // checkSilenceButton(); // Removed
  checkTimer();

  // Lamp breathing using analogWrite (Wokwi-compatible)
  if (isLampOn) {
    // --- NEW RGB LOGIC ---
    if (currentModeIndex == 0 || currentModeIndex == 2) {
      // Focus or Deep Focus: Breathe GREEN
      analogWrite(RED_LED_PIN, 0);
      analogWrite(GREEN_LED_PIN, lampBrightness);
      analogWrite(BLUE_LED_PIN, 0);
    } else {
      // Break or Deep Break: Breathe BLUE
      analogWrite(RED_LED_PIN, 0);
      analogWrite(GREEN_LED_PIN, 0);
      analogWrite(BLUE_LED_PIN, lampBrightness);
    }
    // --- END NEW RGB LOGIC ---

    lampBrightness = lampBrightness + fadeAmount;
    if (lampBrightness <= 0 || lampBrightness >= 255) { // 8-bit range is 0-255
      fadeAmount = -fadeAmount;
    }
  } else {
    // Lamp is off
    analogWrite(GREEN_LED_PIN, 0);
    analogWrite(RED_LED_PIN, 0);
    analogWrite(BLUE_LED_PIN, 0);
  }
  delay(30);
}

// ===================================
// --- FUNCTIONS ---
// ===================================

// --- playTone function removed ---

// --- Checks the LAMP (Start/Stop) button ---
void checkLampButton() {
  int reading = digitalRead(LAMP_BUTTON_PIN);
  if (reading != lastLampButtonState) lastLampDebounce = millis();

  if ((millis() - lastLampDebounce) > debounceDelay) {
    if (reading != lampButtonState) {
      lampButtonState = reading;
      if (lampButtonState == LOW) {
        isLampOn = !isLampOn;

        if (isLampOn) { // --- TURNING ON ---
          // digitalWrite(END_LED_PIN, LOW); // Removed
          if (currentModeIndex == 1) {
            int phrase = random(breakPhraseCount);
            updateDisplay(breakPhrases[phrase], "Resting...");
          } else {
            int phrase = random(focusPhraseCount);
            updateDisplay(focusPhrases[phrase], "...");
          }
          Serial.println("Timer Started!");
          isTimerRunning = true;
          timerStartTime = millis();

        } else { // --- TURNING OFF (Manually) ---
          // digitalWrite(END_LED_PIN, HIGH); // Removed
          showModeScreen();
          Serial.println("Timer Cancelled!");
          isTimerRunning = false;
        }
      }
    }
  }
  lastLampButtonState = reading;
}

// --- Checks the MODE button ---
void checkModeButton() {
  if (isTimerRunning) return;

  int reading = digitalRead(MODE_BUTTON_PIN);
  if (reading != lastModeButtonState) lastModeDebounce = millis();

  if ((millis() - lastModeDebounce) > debounceDelay) {
    if (reading != modeButtonState) {
      modeButtonState = reading;
      if (modeButtonState == LOW) {
        currentModeIndex++;
        if (currentModeIndex >= modeCount) {
          currentModeIndex = 0;
        }
        showModeScreen();
      }
    }
  }
  lastModeButtonState = reading;
}


// --- checkSilenceButton function removed ---

// --- Check Timer Function ---
void checkTimer() {
  if (isTimerRunning) {
    unsigned long duration = modeDurations[currentModeIndex];
    if (millis() - timerStartTime >= duration) {
      Serial.println("Timer Finished!");
      endSession();
    }
  }
}

// --- End Session Function ---
void endSession() {

  // --- SPECIAL: Deep Focus Combo ---
  if (currentModeIndex == 2) {
    currentModeIndex = 3; // Switch to Deep Break mode
    timerStartTime = millis();
    int phrase = random(breakPhraseCount);
    updateDisplay(breakPhrases[phrase], "Auto-Break...");
    Serial.println("Deep Focus ended, starting Deep Break.");
    return; // Keep lamp on, keep timer running
  }

  // --- End of regular Focus, Break, or Deep Break ---
  isTimerRunning = false;
  isLampOn = false;
  
  // digitalWrite(END_LED_PIN, HIGH); // Removed

  int nextModeToShow;
  if (currentModeIndex == 0) { // If Focus just ended
      nextModeToShow = 1; // Switch to Break
  } else { // If Break or Deep Break just ended
      nextModeToShow = 0; // Switch to Focus
      if (currentModeIndex == 3) { // If Deep Break just ended
          nextModeToShow = 2; // Switch back to Deep mode
      }
  }
   currentModeIndex = nextModeToShow;

  showModeScreen();
}


// --- Helper function to update the LCD with mode info ---
void showModeScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  String modeLine = "Mode: " + modeNames[currentModeIndex];
  String durationLine;
  unsigned long durationSec = modeDurations[currentModeIndex] / 1000;

  if (currentModeIndex == 0 || currentModeIndex == 1) {
     durationLine = String(durationSec) + "s Session";
  } else if (currentModeIndex == 2) {
     unsigned long breakSec = modeDurations[3] / 1000;
     durationLine = String(durationSec) + "s + " + String(breakSec) + "s";
  }

  display.println(modeLine);
  display.setCursor(0, 10);
  display.println(durationLine);
  display.setCursor(0, 20);
  display.println("(Press Start)");
  display.display();
}

// --- Helper function to update the LCD (generic) ---
void updateDisplay(String line1, String line2) { 
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  // Basic word wrap
  if (line1.length() > 10) { 
      int wrapPoint = line1.lastIndexOf(' ', 10);
      if (wrapPoint > 0) { // Found a space to wrap at
          display.println(line1.substring(0, wrapPoint));
          display.setCursor(0, 18);
          display.println(line1.substring(wrapPoint + 1));
      } else {
         display.println(line1.substring(0, 10)); // Just truncate if no space
      }
  } else {
    display.println(line1);
  }

  if (line2 != "") {
      display.setTextSize(1);
      display.setCursor(0, 40);
      display.println(line2);
  }
  display.display();
}

