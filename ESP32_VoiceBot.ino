/*
 * ESP32 AI Voice Bot
 * 
 * Hardware Requirements:
 * - ESP32-WROOM-32 Development Board
 * - I2S Digital Microphone (INMP441 or similar)
 * - I2S Audio Amplifier (MAX98357A or similar)
 * - Speaker (4-8 Ohm, 3W)
 * 
 * GPIO Pin Connections:
 * 
 * I2S MICROPHONE (INMP441):
 * --------------------------------
 * MIC VDD    -> 3.3V
 * MIC GND    -> GND
 * MIC SD     -> GPIO 32 (I2S Data In)
 * MIC WS     -> GPIO 25 (I2S Word Select / LRCLK)
 * MIC SCK    -> GPIO 33 (I2S Bit Clock / BCLK)
 * MIC L/R    -> GND (for left channel)
 * 
 * I2S AMPLIFIER (MAX98357A):
 * --------------------------------
 * AMP VIN    -> 5V
 * AMP GND    -> GND
 * AMP DIN    -> GPIO 26 (I2S Data Out)
 * AMP BCLK   -> GPIO 27 (I2S Bit Clock)
 * AMP LRC    -> GPIO 14 (I2S Word Select)
 * AMP SD     -> 3.3V (shutdown control, HIGH = enabled)
 * AMP GAIN   -> GND (9dB gain, or leave floating for 15dB)
 * 
 * SPEAKER:
 * --------------------------------
 * Connect speaker to AMP + and - terminals
 * 
 * Libraries Required:
 * - WiFi (built-in)
 * - HTTPClient (built-in)
 * - ArduinoJson (install via Library Manager)
 * - ESP32-audioI2S (install via Library Manager)
 * - base64 (install via Library Manager)
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>
#include <base64.h>
#include "Audio.h"

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// API Keys
const char* googleApiKey = "YOUR_GOOGLE_CLOUD_API_KEY";
const char* openaiApiKey = "YOUR_OPENAI_API_KEY";

// I2S Microphone Pins (Input)
#define I2S_MIC_SERIAL_DATA    32
#define I2S_MIC_WORD_SELECT    25
#define I2S_MIC_SERIAL_CLOCK   33

// I2S Amplifier Pins (Output)
#define I2S_AMP_SERIAL_DATA    26
#define I2S_AMP_BIT_CLOCK      27
#define I2S_AMP_WORD_SELECT    14

// Button Pin (to start recording)
#define BUTTON_PIN 15

// Recording settings
#define SAMPLE_RATE 16000
#define RECORD_TIME 6  // seconds if not works reduce to 3 sec
#define SAMPLE_BUFFER_SIZE 512

// Audio object for playback
Audio audio;

// Global variables
bool isRecording = false;
uint8_t* audioBuffer;
int audioBufferSize = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("ESP32 AI Voice Bot Starting...");
  Serial.println("=================================\n");

  // Initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize I2S for microphone (input)
  setupI2SMicrophone();
  
  // Initialize Audio library for playback
  audio.setPinout(I2S_AMP_BIT_CLOCK, I2S_AMP_WORD_SELECT, I2S_AMP_SERIAL_DATA);
  audio.setVolume(21); // 0-21, adjust as needed
  
  // Allocate buffer for recording
  audioBufferSize = SAMPLE_RATE * RECORD_TIME * 2; // 16-bit samples
  audioBuffer = (uint8_t*)malloc(audioBufferSize);
  
  Serial.println("✓ Setup complete!");
  Serial.println("Press button on GPIO 15 to start recording...\n");
}

void loop() {
  audio.loop(); // Keep audio library running
  
  // Check if button is pressed
  if (digitalRead(BUTTON_PIN) == LOW && !isRecording) {
    delay(50); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      handleVoiceCommand();
      delay(500); // Prevent multiple triggers
    }
  }
  
  delay(10);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ WiFi Connection Failed!");
    Serial.println("Please check credentials and restart.");
  }
}

void setupI2SMicrophone() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = SAMPLE_BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_WORD_SELECT,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
  
  Serial.println("✓ I2S Microphone initialized");
}

void handleVoiceCommand() {
  Serial.println("\n========================================");
  Serial.println("🎤 Starting Voice Command...");
  Serial.println("========================================\n");
  
  // Step 1: Record audio
  Serial.println("Step 1/4: Recording audio (3 seconds)...");
  recordAudio();
  
  // Step 2: Speech to Text
  Serial.println("\nStep 2/4: Converting speech to text...");
  String transcript = speechToText();
  
  if (transcript.length() == 0) {
    Serial.println("✗ No speech detected!");
    return;
  }
  
  Serial.print("✓ Transcript: ");
  Serial.println(transcript);
  
  // Step 3: Get AI Response
  Serial.println("\nStep 3/4: Getting AI response...");
  String response = getOpenAIResponse(transcript);
  
  if (response.length() == 0) {
    Serial.println("✗ Failed to get AI response!");
    return;
  }
  
  Serial.print("✓ AI Response: ");
  Serial.println(response);
  
  // Step 4: Text to Speech
  Serial.println("\nStep 4/4: Converting to speech and playing...");
  textToSpeech(response);
  
  Serial.println("\n✓ Voice command completed!\n");
}

void recordAudio() {
  isRecording = true;
  memset(audioBuffer, 0, audioBufferSize);
  
  size_t bytesRead = 0;
  size_t totalBytesRead = 0;
  int16_t* buffer = (int16_t*)audioBuffer;
  
  unsigned long startTime = millis();
  
  while ((millis() - startTime) < (RECORD_TIME * 1000)) {
    size_t bytes;
    i2s_read(I2S_NUM_0, buffer + (totalBytesRead / 2), SAMPLE_BUFFER_SIZE, &bytes, portMAX_DELAY);
    totalBytesRead += bytes;
    
    // Show progress
    int progress = ((millis() - startTime) * 100) / (RECORD_TIME * 1000);
    Serial.printf("\rRecording: %d%%", progress);
  }
  
  Serial.println("\r✓ Recording complete!");
  audioBufferSize = totalBytesRead;
  isRecording = false;
}

String speechToText() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ WiFi not connected!");
    return "";
  }

  HTTPClient http;
  String url = "https://speech.googleapis.com/v1/speech:recognize?key=" + String(googleApiKey);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  // Convert audio to base64
  String audioBase64 = base64::encode(audioBuffer, audioBufferSize);
  
  // Create JSON request
  DynamicJsonDocument doc(audioBase64.length() + 1024);
  JsonObject config = doc.createNestedObject("config");
  config["encoding"] = "LINEAR16";
  config["sampleRateHertz"] = SAMPLE_RATE;
  config["languageCode"] = "en-US";
  config["enableAutomaticPunctuation"] = true;
  
  JsonObject audioObj = doc.createNestedObject("audio");
  audioObj["content"] = audioBase64;
  
  String requestBody;
  serializeJson(doc, requestBody);
  
  Serial.println("Sending to Google Speech API...");
  int httpResponseCode = http.POST(requestBody);
  
  String transcript = "";
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    
    DynamicJsonDocument responseDoc(4096);
    deserializeJson(responseDoc, response);
    
    if (responseDoc.containsKey("results")) {
      transcript = responseDoc["results"][0]["alternatives"][0]["transcript"].as<String>();
    }
  } else {
    Serial.printf("✗ HTTP Error: %d\n", httpResponseCode);
    Serial.println(http.getString());
  }
  
  http.end();
  return transcript;
}

String getOpenAIResponse(String text) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ WiFi not connected!");
    return "";
  }

  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(openaiApiKey));
  
  DynamicJsonDocument doc(2048);
  doc["model"] = "gpt-3.5-turbo";
  doc["max_tokens"] = 100;
  doc["temperature"] = 0.7;
  
  JsonArray messages = doc.createNestedArray("messages");
  
  JsonObject systemMsg = messages.createNestedObject();
  systemMsg["role"] = "system";
  systemMsg["content"] = "You are a friendly AI assistant in a robot body. Keep responses brief and conversational, under 50 words.";
  
  JsonObject userMsg = messages.createNestedObject();
  userMsg["role"] = "user";
  userMsg["content"] = text;
  
  String requestBody;
  serializeJson(doc, requestBody);
  
  Serial.println("Sending to OpenAI...");
  int httpResponseCode = http.POST(requestBody);
  
  String aiResponse = "";
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    
    DynamicJsonDocument responseDoc(4096);
    deserializeJson(responseDoc, response);
    
    aiResponse = responseDoc["choices"][0]["message"]["content"].as<String>();
  } else {
    Serial.printf("✗ HTTP Error: %d\n", httpResponseCode);
    Serial.println(http.getString());
  }
  
  http.end();
  return aiResponse;
}

void textToSpeech(String text) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ WiFi not connected!");
    return;
  }

  HTTPClient http;
  String url = "https://texttospeech.googleapis.com/v1/text:synthesize?key=" + String(googleApiKey);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(1024);
  JsonObject input = doc.createNestedObject("input");
  input["text"] = text;
  
  JsonObject voice = doc.createNestedObject("voice");
  voice["languageCode"] = "en-US";
  voice["name"] = "en-US-Neural2-J";
  voice["ssmlGender"] = "NEUTRAL";
  
  JsonObject audioConfig = doc.createNestedObject("audioConfig");
  audioConfig["audioEncoding"] = "MP3";
  audioConfig["pitch"] = 0;
  audioConfig["speakingRate"] = 1.0;
  
  String requestBody;
  serializeJson(doc, requestBody);
  
  Serial.println("Sending to Google TTS...");
  int httpResponseCode = http.POST(requestBody);
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    
    DynamicJsonDocument responseDoc(response.length() + 1024);
    deserializeJson(responseDoc, response);
    
    String audioContent = responseDoc["audioContent"].as<String>();
    
    // Decode base64 audio
    int audioLength = base64::decodeLength(audioContent.c_str());
    uint8_t* audioData = (uint8_t*)malloc(audioLength);
    base64::decode(audioContent.c_str(), audioData);
    
    Serial.println("✓ Playing audio...");
    
    // Save to SPIFFS and play (ESP32-audioI2S library handles MP3 decoding)
    // For simplicity, we'll use connecttohost method with a temporary server
    // In production, save to SPIFFS or stream directly
    
    // Alternative: Use audio.connecttospeech() for direct TTS
    audio.connecttospeech(text.c_str(), "en");
    
    while(audio.isRunning()) {
      audio.loop();
      delay(1);
    }
    
    free(audioData);
  } else {
    Serial.printf("✗ HTTP Error: %d\n", httpResponseCode);
    Serial.println(http.getString());
  }
  
  http.end();
}
