Study Buddy - ESP32 Smart Pomodoro Timer

This is the complete simulation for "Study Buddy," a smart desk companion designed to help with focus and study habits, built for the ESP32.

This project was fully designed and prototyped in the Wokwi online simulator.

 Live Simulation

You can run the complete, working simulation right in your browser by visiting this link:

What is Study Buddy?

Study Buddy is your personal on-desk productivity partner!
Built on the ESP32 (simulated using Wokwi
), he’s got a personality, mood-based lighting, and smart study modes — all packed into one lil' device that feels alive.

This project was developed as part of my exploration into IoT + embedded systems, focusing on creativity, usability, and fun tech vibes.

 Features

 OLED Display — Shows messages, modes, and personality states
 Multiple Modes — Study, Focus, and Chill modes with different lighting behaviors
 RGB LED — Smooth animations and ambient mood lighting
 Personality — Friendly messages, sass included 😌
 Wokwi Compatible — 100% virtual simulation, no physical board required!

 Simulation

This project was built and tested using Wokwi, a virtual electronics simulator.
No ESP32 was harmed in the making of this project. 😆

 Try it here: https://wokwi.com/projects/446194638499401729



 Technologies Used

ESP32 microcontroller

C++ / Arduino IDE

SSD1306 OLED Display

Wokwi Simulator

 Behind the Scenes

Study Buddy wasn’t just about code — it was about building something that reflects the student grind.
Late-night debugging, design tweaking, and making sure my lil’ LED friend actually breathes like a sentient lamp 

 Future Plans

 Add real-time data (timer tracking, focus analytics)
 Build a physical version
 Maybe give him a voice…? 





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

Project Files

sketch.ino: The main C++ source code for the ESP32. It contains all the logic for the timers, buttons, screen, and LED.

diagram.json: The Wokwi diagram file. It defines all the virtual components (ESP32, OLED, buttons, LEDs) and their wiring.

libraries.txt: A list of the required libraries (Adafruit GFX Library, Adafruit SSD1306) needed for this project to compile.
