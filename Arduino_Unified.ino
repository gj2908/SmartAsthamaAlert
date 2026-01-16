/*
 * PROJECT: IoT Safety Monitor - Dual Test Mode
 * DEVICE: Arduino UNO R4 WiFi
 * 
 * TEST MODES:
 * 1. Serial Commands: 'f'=Fall, 'h'=High BPM, 'l'=Low SpO2, 'n'=Normal
 * 2. Web Dashboard: Toggle test mode and adjust sliders
 */

#include <WiFiS3.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

// ======================= CONFIGURATION =======================
const char* ssid     = "Dell";          
const char* password = "123456789gj";   
const char* host     = "sms2-ed4fb-default-rtdb.firebaseio.com";

// ======================= OBJECTS =======================
WiFiSSLClient client;
WiFiServer server(80);

const int chipSelect = 10;

// ======================= TIMERS =======================
unsigned long lastLiveUpdate = 0;
unsigned long lastHistoryLog = 0;
unsigned long lastTestModeCheck = 0;

const long liveInterval = 1000;    // Live updates every 1 second
const long historyInterval = 10000; // History every 10 seconds
const long testModeCheckInterval = 2000; // Check web test mode every 2 seconds

// ======================= SENSOR VARIABLES =======================
const int MPU_ADDR = 0x68;
int16_t acX, acY, acZ;
int simBPM = 72;
int simSpO2 = 98;
int totalSteps = 0;              // Pedometer: Total steps
float lastG = 1.0;               // Pedometer: Last G-force reading
unsigned long lastStepTime = 0;   // Pedometer: Debounce timer

// ======================= TEST MODE FLAGS =======================
bool webTestMode = false;      // Controlled from web dashboard
bool serialTestMode = false;   // Controlled from Serial Monitor
bool forceFall = false;
bool forceHighBPM = false;
bool forceLowSpO2 = false;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // 1. SD Card Init
  Serial.print("Initializing SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Failed!");
  } else {
    Serial.println("OK.");
  }

  // 2. WiFi Init
  connectToWiFi();
  server.begin();
  Serial.println("✓ HTTP Server started!");

  // Upload Device Info (IP)
  uploadDeviceInfo();
  
  // 3. Sensor Init
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);
  
  Serial.println("\n=== SYSTEM READY ===");
  Serial.println("Serial Commands: f=Fall, h=High BPM, l=Low SpO2, n=Normal");
  Serial.println("Web Test Mode: Use dashboard Test Mode page");
  Serial.print("Local IP: "); Serial.println(WiFi.localIP());
  Serial.println("==================\n");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- CHECK SERIAL COMMANDS ---
  checkSerialCommands();

  // --- CHECK WEB COMMANDS & TEST MODE ---
  if (currentMillis - lastTestModeCheck > testModeCheckInterval) {
    checkWebTestMode();
    checkWebCommands();  // Check for Restart/Recalibrate
    lastTestModeCheck = currentMillis;
  }

  // --- HANDLE SD CARD WEB SERVER ---
  handleSDServer();

  // --- GATHER SENSOR DATA ---
  bool anyTestMode = webTestMode || serialTestMode;
  if (!anyTestMode) {
    simulateVitals();
  }
  readMPU();
  int aqi = analogRead(A0);
  
  // Pedometer Logic
  detectSteps();

  // Fall Logic
  bool fall = detectFall(acX, acY, acZ);

  // --- APPLY TEST OVERRIDES ---
  int uploadBPM = simBPM;
  int uploadSpO2 = simSpO2;
  int uploadAQI = aqi;
  bool uploadFall = fall;

  if (serialTestMode) {
    if (forceHighBPM) uploadBPM = 145;
    if (forceLowSpO2) uploadSpO2 = 85;
    if (forceFall) uploadFall = true;
  }

  // --- LIVE DASHBOARD UPDATE (Every 1 second) ---
  if (currentMillis - lastLiveUpdate > liveInterval) {
    String json = "{";
    json += "\"AQI\":" + String(uploadAQI) + ",";
    json += "\"BPM\":" + String(uploadBPM) + ",";
    json += "\"SpO2\":" + String(uploadSpO2) + ",";
    json += "\"Steps\":" + String(totalSteps) + ",";
    json += "\"Fall_Status\":" + String(uploadFall ? "true" : "false");
    if (!webTestMode) {
      json += ",\"test_mode\":false";
    }
    json += "}";

    if (!webTestMode) {
      sendToFirebase("PATCH", "/sensor_data.json", json);
    }
    lastLiveUpdate = currentMillis;
  }

  // --- HISTORY & FALL LOGGING ---
  if (currentMillis - lastHistoryLog > historyInterval || uploadFall) {
    String json = "{";
    json += "\"timestamp\": {\".sv\": \"timestamp\"},";
    json += "\"AQI\":" + String(uploadAQI) + ",";
    json += "\"BPM\":" + String(uploadBPM) + ",";
    json += "\"SpO2\":" + String(uploadSpO2) + ",";
    json += "\"Steps\":" + String(totalSteps) + ",";
    json += "\"Fall_Status\":" + String(uploadFall ? "true" : "false");
    json += "}";

    sendToFirebase("POST", "/history.json", json);
    
    // Dedicated Fall Log
    if (uploadFall) {
       sendToFirebase("POST", "/fall_records.json", json);
       Serial.println("🚨 FALL RECORDED!");
       delay(2000); // Debounce fall log
    }
    
    lastHistoryLog = currentMillis;
  }
}

// ======================= SYSTEM COMMANDS =======================

void uploadDeviceInfo() {
  String ip = WiFi.localIP().toString();
  sendToFirebase("PUT", "/sensor_data/device_ip.json", "\"" + ip + "\"");
}

void checkWebCommands() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(host, 443)) {
      client.println("GET /sensor_data/commands.json HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Connection: close");
      client.println();
      
      bool foundData = false;
      while(client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") { foundData = true; break; }
      }
      
      if (foundData && client.available()) {
        String cmd = client.readStringUntil('\n');
        cmd.replace("\"", ""); // Remove quotes
        cmd.trim();
        
        if (cmd == "restart") {
          Serial.println("🔄 REMOTE RESTART REQUESTED...");
          sendToFirebase("DELETE", "/sensor_data/commands.json", ""); // Clear command
          delay(1000);
          NVIC_SystemReset(); // Reboot Arduino R4
        } 
        else if (cmd == "recalibrate") {
          Serial.println("⚖️ RECALIBRATING SENSORS...");
          Wire.beginTransmission(MPU_ADDR);
          Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);
          sendToFirebase("DELETE", "/sensor_data/commands.json", ""); // Clear command
        }
      }
      client.stop();
    }
  }
}

// ======================= SENSOR HELPERS =======================

void detectSteps() {
  float currentG = sqrt(pow(acX / 16384.0, 2) + pow(acY / 16384.0, 2) + pow(acZ / 16384.0, 2));
  float delta = abs(currentG - lastG);
  
  // Step detection: Simple peak threshold
  if (delta > 0.3 && (millis() - lastStepTime > 300)) {
    totalSteps++;
    lastStepTime = millis();
    Serial.print("Step: "); Serial.println(totalSteps);
  }
  lastG = currentG;
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  acX = Wire.read() << 8 | Wire.read();
  acY = Wire.read() << 8 | Wire.read();
  acZ = Wire.read() << 8 | Wire.read();
}

bool detectFall(int16_t x, int16_t y, int16_t z) {
  float totalG = sqrt(pow(x / 16384.0, 2) + pow(y / 16384.0, 2) + pow(z / 16384.0, 2));
  return totalG > 2.8; // Slightly higher threshold for reliability
}
