📦 What You Got:
1️⃣ ESP32_VoiceBot.ino - Complete Arduino Code

Records 3 seconds of audio from I2S microphone
Sends to Google Speech-to-Text API
Gets AI response from OpenAI
Converts response to speech via Google TTS
Plays audio through I2S amplifier

2️⃣ ESP32_WIRING_GUIDE.md - Detailed Setup Guide

Complete parts list
Step-by-step wiring instructions
Library installation guide
Troubleshooting tips
Power consumption info

3️⃣ ESP32_WIRING_DIAGRAM.txt - Visual Wiring Diagram

ASCII art diagram for easy reference
Quick pin summary
Color coding suggestions

🔌 Quick Pin Reference:
INMP441 Microphone:

VDD → 3.3V
GND → GND
SD → GPIO 32
WS → GPIO 25
SCK → GPIO 33
L/R → GND

MAX98357A Amplifier:

VIN → 5V
GND → GND
DIN → GPIO 26
BCLK → GPIO 27
LRC → GPIO 14
SD → 3.3V (enable)
GAIN → GND (9dB)

Button: GPIO 15 → Button → GND
Speaker: Connect to AMP + and -
⚙️ Before Uploading:

Install required libraries in Arduino IDE
Add your WiFi credentials
Add your API keys (Google Cloud + OpenAI)
Select ESP32 board and correct COM port
Upload!
