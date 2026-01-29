# ESP32 AI Voice Bot - Hardware Setup Guide

## 📦 Required Components

1. **ESP32-WROOM-32** Development Board
2. **INMP441 I2S Digital Microphone** (or similar I2S MEMS mic)
3. **MAX98357A I2S Audio Amplifier** (or similar I2S DAC/Amp)
4. **Speaker** (4-8 Ohm, 3W recommended)
5. **Push Button** (momentary, normally open)
6. **10kΩ Resistor** (for button pull-up, optional if using internal pull-up)
7. **Jumper Wires**
8. **Breadboard** (optional, for prototyping)
9. **USB Cable** (for programming and power)
10. **5V Power Supply** (2A minimum for stable operation)

---

## 🔌 Complete Wiring Diagram

```
ESP32-WROOM-32 PIN CONNECTIONS
=====================================

I2S MICROPHONE (INMP441)
------------------------
INMP441 Pin  →  ESP32 Pin    →  Description
-------------------------------------------------
VDD          →  3.3V          →  Power supply
GND          →  GND           →  Ground
SD (DOUT)    →  GPIO 32       →  Serial Data (microphone audio out)
WS (LRCK)    →  GPIO 25       →  Word Select (left/right clock)
SCK (BCLK)   →  GPIO 33       →  Serial Clock (bit clock)
L/R          →  GND           →  Channel select (GND=Left, 3.3V=Right)


I2S AMPLIFIER (MAX98357A)
-------------------------
MAX98357A Pin →  ESP32 Pin   →  Description
-------------------------------------------------
VIN           →  5V (VIN)     →  Power supply (3.3-5V)
GND           →  GND          →  Ground
DIN           →  GPIO 26      →  Serial Data (audio in)
BCLK          →  GPIO 27      →  Bit Clock
LRC (LRCLK)   →  GPIO 14      →  Left/Right Clock (Word Select)
SD            →  3.3V         →  Shutdown (HIGH=on, LOW=off)
GAIN          →  GND          →  Gain setting (GND=9dB, Float=15dB, VDD=12dB)


SPEAKER CONNECTION
------------------
Speaker Wire  →  MAX98357A Terminal
-------------------------------------------------
Positive (+)  →  + Terminal on MAX98357A
Negative (-)  →  - Terminal on MAX98357A


PUSH BUTTON (Start Recording)
------------------------------
Button Pin    →  ESP32 Pin    →  Description
-------------------------------------------------
One side      →  GPIO 15      →  Button input
Other side    →  GND          →  Ground
(Internal pull-up resistor enabled in code)


POWER SUPPLY
------------
Power Source  →  ESP32 Pin    →  Notes
-------------------------------------------------
USB 5V        →  VIN or 5V    →  For programming & testing
External 5V   →  VIN          →  For standalone operation (2A recommended)
GND           →  GND          →  Common ground for all components
```

---

## 🎯 Pin Summary Table

| Component      | Component Pin | ESP32 GPIO | Function                |
|----------------|---------------|------------|-------------------------|
| **INMP441 Mic**|               |            |                         |
| Data Out       | SD            | GPIO 32    | I2S Data In             |
| Word Select    | WS            | GPIO 25    | I2S LRCLK (Mic)         |
| Clock          | SCK           | GPIO 33    | I2S BCLK (Mic)          |
| Power          | VDD           | 3.3V       | Power Supply            |
| Ground         | GND           | GND        | Ground                  |
| Channel        | L/R           | GND        | Left Channel            |
| **MAX98357A**  |               |            |                         |
| Data In        | DIN           | GPIO 26    | I2S Data Out            |
| Bit Clock      | BCLK          | GPIO 27    | I2S BCLK (Amp)          |
| LR Clock       | LRC           | GPIO 14    | I2S LRCLK (Amp)         |
| Power          | VIN           | 5V         | Power Supply            |
| Ground         | GND           | GND        | Ground                  |
| Shutdown       | SD            | 3.3V       | Enable Amplifier        |
| Gain           | GAIN          | GND        | 9dB Gain                |
| **Button**     |               |            |                         |
| Input          | -             | GPIO 15    | Recording Trigger       |
| Ground         | -             | GND        | Ground                  |

---

## 🔧 Step-by-Step Assembly

### 1. Prepare ESP32
- Connect ESP32 to breadboard or prepare for direct wiring
- Identify all GPIO pins clearly

### 2. Connect INMP441 Microphone
```
INMP441 → ESP32
VDD  →  3.3V
GND  →  GND
SD   →  GPIO 32
WS   →  GPIO 25
SCK  →  GPIO 33
L/R  →  GND
```

### 3. Connect MAX98357A Amplifier
```
MAX98357A → ESP32
VIN   →  5V
GND   →  GND
DIN   →  GPIO 26
BCLK  →  GPIO 27
LRC   →  GPIO 14
SD    →  3.3V
GAIN  →  GND (or leave floating for more volume)
```

### 4. Connect Speaker
```
Connect 4-8 Ohm speaker to MAX98357A:
Speaker + → MAX98357A +
Speaker - → MAX98357A -
```

### 5. Connect Push Button
```
Button → ESP32
Pin 1  →  GPIO 15
Pin 2  →  GND
```

---

## 📚 Required Arduino Libraries

Install these libraries via Arduino IDE Library Manager:

1. **WiFi** (Built-in with ESP32 board support)
2. **HTTPClient** (Built-in with ESP32 board support)
3. **ArduinoJson** by Benoit Blanchon (v6.x)
4. **ESP32-audioI2S** by Schreibfaul1
5. **base64** by Arturo Guadalupi

### Installation Steps:
1. Open Arduino IDE
2. Go to: Sketch → Include Library → Manage Libraries
3. Search for each library and click "Install"

### ESP32 Board Support:
1. File → Preferences
2. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Tools → Board → Board Manager
4. Search "ESP32" and install "ESP32 by Espressif Systems"

---

## ⚙️ Configuration

### 1. Update WiFi Credentials
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 2. Add API Keys
```cpp
const char* googleApiKey = "YOUR_GOOGLE_CLOUD_API_KEY";
const char* openaiApiKey = "YOUR_OPENAI_API_KEY";
```

### 3. Upload Code
1. Connect ESP32 via USB
2. Select: Tools → Board → ESP32 Dev Module
3. Select correct COM Port
4. Click Upload

---

## 🎛️ GPIO Pin Notes

### Why These Specific Pins?

**Microphone (Input I2S):**
- GPIO 32, 25, 33 are input-capable and support I2S peripheral
- These pins don't interfere with boot or flash operations

**Amplifier (Output I2S):**
- GPIO 26, 27, 14 are output-capable and support I2S peripheral
- Safe to use, don't interfere with system functions

**Button:**
- GPIO 15 has internal pull-up, safe for input

### Pins to AVOID:
- ❌ GPIO 0: Boot mode selection
- ❌ GPIO 2: Boot mode, internal LED
- ❌ GPIO 5: Boot strapping
- ❌ GPIO 12: Boot voltage selection
- ❌ GPIO 6-11: Connected to flash memory
- ❌ GPIO 34-39: Input only (can't use for output)

---

## 🔊 Audio Quality Tips

### For Better Microphone Quality:
1. Keep wires short (< 15cm)
2. Use twisted pair or shielded cable for I2S signals
3. Keep microphone away from ESP32 and power supply
4. Add 0.1µF capacitor near INMP441 VDD pin

### For Better Speaker Quality:
1. Use high-quality speaker (full-range preferred)
2. Adjust GAIN pin on MAX98357A:
   - GND = 9dB (quieter, less distortion)
   - Float = 15dB (louder, may distort)
   - VDD = 12dB (medium)
3. Adjust volume in code: `audio.setVolume(0-21)`
4. Use 2A power supply for clean audio

### Power Supply Tips:
1. Use separate 5V supply (not USB) for final project
2. Add 100µF capacitor near ESP32 VIN
3. Add 100µF capacitor near MAX98357A VIN
4. Ensure good ground connection for all components

---

## 🧪 Testing Procedure

### 1. Test WiFi Connection
- Upload code and open Serial Monitor (115200 baud)
- Should see "WiFi Connected!" message

### 2. Test Microphone
- Press button on GPIO 15
- Should see "Recording..." progress
- Check for "Recording complete!" message

### 3. Test Speech Recognition
- After recording, should see transcript in Serial Monitor

### 4. Test Speaker
- Should hear audio playback from speaker
- If no sound, check MAX98357A SD pin (should be HIGH)

---

## 🐛 Troubleshooting

### No WiFi Connection
- ✓ Check SSID and password
- ✓ Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- ✓ Move closer to router

### No Microphone Input
- ✓ Check all I2S connections
- ✓ Verify 3.3V power to INMP441
- ✓ Check L/R pin connected to GND
- ✓ Try swapping WS and SCK if no audio

### No Speaker Output
- ✓ Check MAX98357A SD pin is connected to 3.3V
- ✓ Verify 5V power to MAX98357A
- ✓ Check speaker connections (+ and -)
- ✓ Test with different speaker
- ✓ Increase volume in code

### API Errors
- ✓ Verify API keys are correct
- ✓ Check Google Cloud APIs are enabled
- ✓ Ensure OpenAI account has credits
- ✓ Check internet connection

### Poor Audio Quality
- ✓ Shorten I2S wires
- ✓ Add capacitors for power filtering
- ✓ Use better power supply (2A minimum)
- ✓ Keep microphone away from ESP32

---

## 💡 Usage Instructions

1. **Power on** the ESP32
2. Wait for **"Ready!"** message in Serial Monitor
3. **Press button** on GPIO 15 to start recording
4. **Speak clearly** for 3 seconds
5. Wait for **processing** (LED activity on ESP32)
6. **Listen** to the AI response from speaker!

---

## 🎨 Optional Enhancements

### Add Status LEDs:
- Recording LED → GPIO 2 (built-in LED)
- WiFi Status LED → GPIO 4
- Speaking LED → GPIO 16

### Add Power Switch:
- Between power supply and VIN
- Allows easy on/off without unplugging

### 3D Printed Enclosure:
- Design enclosure with:
  - Microphone opening (covered with acoustic foam)
  - Speaker grille
  - Button access
  - Ventilation holes

---

## 📊 Power Consumption

| Component | Idle | Active | Peak |
|-----------|------|--------|------|
| ESP32 | 80mA | 160mA | 240mA |
| INMP441 | 1mA | 1.5mA | 2mA |
| MAX98357A | 10mA | 500mA | 1.4A |
| **Total** | ~100mA | ~660mA | ~1.6A |

**Recommended Power Supply:** 5V, 2A minimum

---

## 🔐 Security Notes

- API keys are stored in plain text in code
- For production, use secure storage (SPIFFS encrypted)
- Consider using environment variables
- Don't share code with API keys included!

---

## 📞 Support Resources

- ESP32 Documentation: https://docs.espressif.com/
- INMP441 Datasheet: https://invensense.tdk.com/
- MAX98357A Datasheet: https://www.analog.com/
- Arduino Forum: https://forum.arduino.cc/

---

**Ready to build! 🚀**
