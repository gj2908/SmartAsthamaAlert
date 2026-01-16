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
  
  // 3. Sensor Init
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);
  
  Serial.println("\n=== SYSTEM READY ===");
  Serial.println("Serial Commands: f=Fall, h=High BPM, l=Low SpO2, n=Normal");
  Serial.println("Web Test Mode: Use dashboard Test Mode page");
  Serial.println("==================\n");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- CHECK SERIAL COMMANDS ---
  checkSerialCommands();

  // --- CHECK WEB TEST MODE STATUS ---
  if (currentMillis - lastTestModeCheck > testModeCheckInterval) {
    checkWebTestMode();
    lastTestModeCheck = currentMillis;
  }

  // --- HANDLE SD CARD WEB SERVER ---
  handleSDServer();

  // --- DETERMINE IF ANY TEST MODE IS ACTIVE ---
  bool anyTestMode = webTestMode || serialTestMode;

  // --- GATHER SENSOR DATA ---
  if (!anyTestMode) {
    simulateVitals();
  }
  readMPU();
  int aqi = analogRead(A0);
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
    json += "\"Fall_Status\":" + String(uploadFall ? "true" : "false");
    if (!webTestMode) {
      json += ",\"test_mode\":false";  // Reset flag if not in web test mode
    }
    json += "}";

    if (!webTestMode) {  // Only update if web test mode is OFF
      sendToFirebase("PATCH", "/sensor_data.json", json);
      Serial.print(".");
    } else {
      Serial.print("W"); // W = Web test mode active
    }
    
    lastLiveUpdate = currentMillis;
  }

  // --- HISTORY LOGGING (Every 10 seconds) ---
  if (currentMillis - lastHistoryLog > historyInterval) {
    // Log to SD
    File f = SD.open("datalog.txt", FILE_WRITE);
    if (f) {
      f.print(millis()); f.print(",");
      f.print(uploadAQI); f.print(",");
      f.print(uploadBPM); f.print(",");
      f.print(uploadSpO2); f.print(",");
      f.println(uploadFall ? "1" : "0");
      f.close();
    }

    // Upload to Firebase History
    String json = "{";
    json += "\"timestamp\": {\".sv\": \"timestamp\"},";
    json += "\"AQI\":" + String(uploadAQI) + ",";
    json += "\"BPM\":" + String(uploadBPM) + ",";
    json += "\"SpO2\":" + String(uploadSpO2) + ",";
    json += "\"Fall_Status\":" + String(uploadFall ? "true" : "false");
    json += "}";

    sendToFirebase("POST", "/history.json", json);
    Serial.println(" [History Saved]");
    
    lastHistoryLog = currentMillis;
  }
}

// ======================= TEST MODE FUNCTIONS =======================

void checkSerialCommands() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'f') {
      serialTestMode = true;
      forceFall = true;
      forceHighBPM = false;
      forceLowSpO2 = false;
      Serial.println("\n🚨 SERIAL TEST: FALL DETECTED");
    } 
    else if (cmd == 'h') {
      serialTestMode = true;
      forceHighBPM = true;
      forceFall = false;
      forceLowSpO2 = false;
      Serial.println("\n❤️ SERIAL TEST: HIGH HEART RATE");
    }
    else if (cmd == 'l') {
      serialTestMode = true;
      forceLowSpO2 = true;
      forceFall = false;
      forceHighBPM = false;
      Serial.println("\n🫁 SERIAL TEST: LOW OXYGEN");
    }
    else if (cmd == 'n') {
      serialTestMode = false;
      forceFall = false;
      forceHighBPM = false;
      forceLowSpO2 = false;
      Serial.println("\n✓ SERIAL TEST: NORMAL MODE");
    }
  }
}

void checkWebTestMode() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(host, 443)) {
      client.println("GET /sensor_data/test_mode.json HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Connection: close");
      client.println();
      
      bool foundData = false;
      while(client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          foundData = true;
          break;
        }
      }
      
      if (foundData && client.available()) {
        String response = client.readStringUntil('\n');
        response.trim();
        
        bool previousMode = webTestMode;
        webTestMode = (response == "true");
        
        if (previousMode != webTestMode) {
          Serial.println();
          Serial.println(webTestMode ? "🌐 WEB TEST MODE: ON" : "✓ WEB TEST MODE: OFF");
        }
      }
      
      client.stop();
    }
  }
}

// ======================= SD CARD WEB SERVER =======================

void handleSDServer() {
  WiFiClient localClient = server.available();
  if (localClient) {
    Serial.println("\n[HTTP] Client connected");
    boolean currentLineIsBlank = true;
    
    while (localClient.connected()) {
      if (localClient.available()) {
        char c = localClient.read();
        if (c == '\n' && currentLineIsBlank) {
          // Send HTTP response
          localClient.println("HTTP/1.1 200 OK");
          localClient.println("Content-Type: text/html");
          localClient.println("Connection: close");
          localClient.println();
          
          // HTML page
          localClient.println("<!DOCTYPE html><html><head>");
          localClient.println("<title>Sensor Logs</title>");
          localClient.println("<style>body{font-family:monospace;background:#1a1a1a;color:#0f0;padding:20px;}");
          localClient.println("pre{background:#000;padding:15px;border:1px solid #0f0;}</style></head><body>");
          localClient.println("<h2>📊 SD Card Data Log</h2><pre>");
          
          File dataFile = SD.open("datalog.txt");
          if (dataFile) {
            while (dataFile.available()) {
              localClient.write(dataFile.read());
            }
            dataFile.close();
          } else {
            localClient.println("Error: datalog.txt not found");
          }
          
          localClient.println("</pre></body></html>");
          break;
        }
        if (c == '\n') currentLineIsBlank = true;
        else if (c != '\r') currentLineIsBlank = false;
      }
    }
    
    delay(1);
    localClient.stop();
    Serial.println("[HTTP] Client disconnected");
  }
}

// ======================= NETWORK HELPER =======================

void sendToFirebase(String method, String path, String data) {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(host, 443)) {
      client.println(method + " " + path + " HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(data.length());
      client.println("Connection: close");
      client.println();
      client.println(data);
      
      while(client.connected()) {
        String line = client.readStringUntil('\n');
        if(line == "\r") break;
      }
      client.stop();
    }
  } else {
    connectToWiFi();
  }
}

void connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✓ WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ======================= SENSOR HELPERS =======================

void simulateVitals() {
  if (random(0, 100) > 85) simBPM = constrain(simBPM + random(-2, 3), 60, 110);
  if (random(0, 100) > 90) simSpO2 = constrain(simSpO2 + random(-1, 2), 94, 99);
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
  return totalG > 2.5;
}
