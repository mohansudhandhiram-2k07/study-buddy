Study Buddy - ESP32 Smart Pomodoro Timer

This is the complete simulation for "Study Buddy," a smart desk companion designed to help with focus and study habits, built for the ESP32.

This project was fully designed and prototyped in the Wokwi online simulator.

 Live Simulation

You can run the complete, working simulation right in your browser by visiting this link:

https://wokwi.com/projects/446194638499401729


 Features
 ( used for test but the real timer follows the pomodoro )

Custom Timer Modes: Cycle between "Focus" (12s), "Break" (5s), and "Deep Focus" (15s + 7s combo) using the Mode Button.

Start/Stop Control: A dedicated button to start, stop, or manually cancel any timer session.

RGB Status Lamp: A "breathing" RGB LED provides visual feedback:

Green: "Focus" or "Deep Focus" mode is active.

Blue: "Break" or "Deep Break" mode is active.

Off: The timer is idle.

Personality Engine: The OLED screen displays random motivational phrases during focus sessions and relaxing phrases during breaks.

OLED Status Display: Clearly shows the current mode and timer duration when idle, and personality phrases when active.

Automatic Flow: The "Deep Focus" mode automatically transitions from the study session to the break session.

📁 Project Files

sketch.ino: The main C++ source code for the ESP32. It contains all the logic for the timers, buttons, screen, and LED.

diagram.json: The Wokwi diagram file. It defines all the virtual components (ESP32, OLED, buttons, LEDs) and their wiring.

libraries.txt: A list of the required libraries (Adafruit GFX Library, Adafruit SSD1306) needed for this project to compile.
