<h1>ESP32 VoiceBot</h1>

<p>A smart voice assistant using ESP32 that can <strong>record audio</strong>, <strong>transcribe it</strong>, <strong>get AI responses</strong>, and <strong>speak back</strong> in real-time!</p>

<h2>📦 What’s Included</h2>
<ul>
  <li><strong>ESP32_VoiceBot.ino</strong> – Complete Arduino code
    <ul>
      <li>Records 3 seconds of audio from I2S microphone</li>
      <li>Sends audio to <strong>Google Speech-to-Text API</strong></li>
      <li>Retrieves AI response from <strong>OpenAI</strong></li>
      <li>Converts AI response to speech via <strong>Google TTS</strong></li>
      <li>Plays audio through <strong>I2S amplifier</strong></li>
    </ul>
  </li>
  <li><strong>ESP32_WIRING_GUIDE.md</strong> – Detailed Setup Guide
    <ul>
      <li>Complete parts list</li>
      <li>Step-by-step wiring instructions</li>
      <li>Library installation guide</li>
      <li>Troubleshooting tips</li>
      <li>Power consumption info</li>
    </ul>
  </li>
  <li><strong>ESP32_WIRING_DIAGRAM.txt</strong> – Visual Wiring Diagram
    <ul>
      <li>ASCII art wiring diagram for easy reference</li>
      <li>Quick pin summary</li>
      <li>Color coding suggestions</li>
    </ul>
  </li>
</ul>

<h2>🔌 Quick Pin Reference</h2>

<h3>INMP441 Microphone</h3>
<table>
  <tr><th>Pin</th><th>Connects To</th></tr>
  <tr><td>VDD</td><td>3.3V</td></tr>
  <tr><td>GND</td><td>GND</td></tr>
  <tr><td>SD</td><td>GPIO 32</td></tr>
  <tr><td>WS</td><td>GPIO 25</td></tr>
  <tr><td>SCK</td><td>GPIO 33</td></tr>
  <tr><td>L/R</td><td>GND</td></tr>
</table>

<h3>MAX98357A Amplifier</h3>
<table>
  <tr><th>Pin</th><th>Connects To</th></tr>
  <tr><td>VIN</td><td>5V</td></tr>
  <tr><td>GND</td><td>GND</td></tr>
  <tr><td>DIN</td><td>GPIO 26</td></tr>
  <tr><td>BCLK</td><td>GPIO 27</td></tr>
  <tr><td>LRC</td><td>GPIO 14</td></tr>
  <tr><td>SD</td><td>3.3V (enable)</td></tr>
  <tr><td>GAIN</td><td>GND (9dB)</td></tr>
</table>

<h3>Other Components</h3>
<ul>
  <li>Button: GPIO 15 → Button → GND</li>
  <li>Speaker: Connect to AMP + and -</li>
</ul>

<h2>⚙️ Before Uploading</h2>
<ol>
  <li>Install required libraries in Arduino IDE</li>
  <li>Add your WiFi credentials</li>
  <li>Add your API keys (Google Cloud + OpenAI)</li>
  <li>Select ESP32 board and correct COM port</li>
  <li>Upload the code!</li>
</ol>
