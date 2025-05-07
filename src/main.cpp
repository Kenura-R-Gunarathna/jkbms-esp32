#include <Arduino.h>
#include <HardwareSerial.h>
#include <vector>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

using namespace std;

/* -------------------------------------------------
 * Constants
 * ------------------------------------------------- */

const int DEFAULT_RX = 16;                    // GPIO for receiving data from JKBMS
const int DEFAULT_TX = 17;                    // GPIO for transmitting data to JKBMS
const int DEFAULT_BAUD_RATE = 115200;         // Baud rate for serial communication
const int DEFAULT_RESPONSE_TIMEOUT = 100;     // Timeout for reading BMS response (ms)
const int DEFAULT_REQUEST_TIMEOUT = 5000;     // Interval between requests (ms)
const bool DEFAULT_DEBUG = true;              // Enable or disable debug mode

const char* DEFAULT_WIFI_SSID = "admin";              // Replace with your WiFi SSID
const char* DEFAULT_WIFI_PASSWORD = "pass";           // Replace with your WiFi password
const String DEFAULT_API_URL = "https://your-api-endpoint.com";  // Your API endpoint


/* -------------------------------------------------
 * Globals
 * ------------------------------------------------- */

int RX_PIN = DEFAULT_RX;
int TX_PIN = DEFAULT_TX;
int BAUD_RATE = DEFAULT_BAUD_RATE;
int RESPONSE_TIMEOUT = DEFAULT_RESPONSE_TIMEOUT;
int REQUEST_TIMEOUT = DEFAULT_REQUEST_TIMEOUT;
bool DEBUG = DEFAULT_DEBUG;

String WIFI_SSID = DEFAULT_WIFI_SSID;
String WIFI_PASSWORD = DEFAULT_WIFI_PASSWORD;
String API_URL = DEFAULT_API_URL;

bool frameReceived = false;       // Flag to indicate frame was received

// Request data to be sent to JKBMS
const vector<uint8_t> requestData = {
  0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00,
  0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x68, 0x00, 0x00, 0x01, 0x29
};

// Sample test data from JKBMS (for debugging purposes)
const vector<uint8_t> sampleResponseData = {
  0x4E, 0x57, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x79, 0x15, 0x01, 0x0E, 0xD2, 0x02, 0x0E, 
  0xD2, 0x03, 0x0E, 0xD2, 0x04, 0x0E, 0xD2, 0x05, 0x0E, 0xDB, 0x06, 0x0E, 0xDB, 0x07, 0x0E, 0xDB, 0x80, 0x00, 
  0x1C, 0x81, 0x00, 0x1C, 0x82, 0x00, 0x1C, 0x83, 0x0A, 0x62, 0x84, 0x00, 0x00, 0x85, 0x5A, 0x86, 0x02, 0x87, 
  0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x05, 0x8A, 0x00, 0x07, 0x8B, 0x00, 0x00, 0x8C, 0x00, 0x03, 0x8E, 0x0B, 
  0x7C, 0x8F, 0x07, 0xB6, 0x90, 0x10, 0x68, 0x91, 0x10, 0x54, 0x92, 0x00, 0x03, 0x93, 0x0B, 0x04, 0x94, 0x0B, 
  0x22, 0x95, 0x00, 0x03, 0x96, 0x01, 0x2C, 0x97, 0x00, 0xC8, 0x98, 0x01, 0x2C, 0x99, 0x00, 0x19, 0x9A, 0x00, 
  0x1E, 0x9B, 0x0B, 0xB8, 0x9C, 0x00, 0x0A, 0x9D, 0x01, 0x9E, 0x00, 0x64, 0x9F, 0x00, 0x50, 0xA0, 0x00, 0x46, 
  0xA1, 0x00, 0x3C, 0xA2, 0x00, 0x14, 0xA3, 0x00, 0x46, 0xA4, 0x00, 0x46, 0xA5, 0xFF, 0xEC, 0xA6, 0xFF, 0xF6, 
  0xA7, 0xFF, 0xEC, 0xA8, 0xFF, 0xF6, 0xA9, 0x07, 0xAA, 0x00, 0x00, 0x00, 0x75, 0xAB, 0x01, 0xAC, 0x01, 0xAD, 
  0x03, 0xCC, 0xAE, 0x01, 0xAF, 0x01, 0xB0, 0x00, 0x0A, 0xB1, 0x14, 0xB2, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 
  0x00, 0x00, 0x00, 0x00, 0xB3, 0x00, 0xB4, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x20, 0x55, 0x73, 0xB5, 0x32, 0x34, 
  0x30, 0x37, 0xB6, 0x00, 0x00, 0x08, 0x19, 0xB7, 0x31, 0x31, 0x2E, 0x58, 0x41, 0x5F, 0x53, 0x31, 0x31, 0x2E, 
  0x35, 0x31, 0x5F, 0x5F, 0x5F, 0xB8, 0x00, 0xB9, 0x00, 0x00, 0x00, 0x75, 0xBA, 0x49, 0x6E, 0x70, 0x75, 0x74, 
  0x20, 0x55, 0x73, 0x65, 0x72, 0x64, 0x61, 0x4A, 0x4B, 0x5F, 0x42, 0x31, 0x41, 0x38, 0x53, 0x32, 0x30, 0x50, 
  0x00, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x4D, 0x89
};


const uint8_t headerStartBytes[] = {0x4E, 0x57};  // JKBMS header

HardwareSerial BMS_SERIAL(2);  // UART2 for BMS

Preferences prefs;  // Preferences for storing configuration


/* -------------------------------------------------
 * Function Prototypes
 * ------------------------------------------------- */

void sendBMSRequest();
vector<uint8_t> readBMSDataVector();
void printHexArray(const vector<uint8_t>& data);
bool validateHeader(const vector<uint8_t>& data);
bool validateChecksum(const vector<uint8_t>& data);


/* -------------------------------------------------
 * Setup
 * ------------------------------------------------- */

void setup() {
  loadConfig();

  Serial.begin(BAUD_RATE);
  BMS_SERIAL.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("ESP32 JKBMS Decoder Starting...");
  
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected.");

}


/* -------------------------------------------------
 * Main Loop
 * ------------------------------------------------- */

void loop() {
  vector<uint8_t> responseData;

  if (DEBUG) {
    Serial.print("Sent request: ");
    printHexArray(requestData);
  }

  sendBMSRequest();                    // Send command
  delay(RESPONSE_TIMEOUT);        // Allow response time
  responseData = readBMSDataVector(); // Capture response

  if (responseData.empty()) {
    Serial.println("Error: No response from BMS! Check connection or power.");

    if (!DEBUG) return;

    // For debug mode: simulate dummy response
    Serial.println("Using dummy test response:");
    responseData = sampleResponseData;

  } else {
    Serial.println("Received response from JKBMS (Hex):");
    printHexArray(responseData);
  }

  if (frameReceived) {
    if (validateHeader(responseData)) {
      Serial.println("Validation: Header OK");
    } else {
      Serial.println("Validation: Header Failed");
    }

    if (validateChecksum(responseData)) {
      Serial.println("Checksum: OK");
    } else {
      Serial.println("Checksum: Failed");
    }

    // Send POST request with the response data
    sendPostRequest(responseData);

    frameReceived = false;  // Reset flag
  }

  delay(REQUEST_TIMEOUT);  // Wait before next request
}


/* -------------------------------------------------
 * Configuration Loading
 * ------------------------------------------------- */
void loadConfig() {
  prefs.begin("bmsConfig", true);
  RX_PIN = prefs.getInt("rxPin", DEFAULT_RX);
  TX_PIN = prefs.getInt("txPin", DEFAULT_TX);
  BAUD_RATE = prefs.getInt("baud", DEFAULT_BAUD_RATE);
  RESPONSE_TIMEOUT = prefs.getInt("resTimeout", DEFAULT_RESPONSE_TIMEOUT);
  REQUEST_TIMEOUT = prefs.getInt("reqTimeout", DEFAULT_REQUEST_TIMEOUT);
  DEBUG = prefs.getBool("debug", DEFAULT_DEBUG);
  WIFI_SSID = prefs.getString("ssid", DEFAULT_WIFI_SSID);
  WIFI_PASSWORD = prefs.getString("pass", DEFAULT_WIFI_PASSWORD);
  API_URL = prefs.getString("api", DEFAULT_API_URL);
  prefs.end();
}


/* -------------------------------------------------
 * Communication Functions
 * ------------------------------------------------- */

void sendBMSRequest() {
  BMS_SERIAL.write(requestData.data(), requestData.size());
}

vector<uint8_t> readBMSDataVector() {
  vector<uint8_t> responseData;
  unsigned long startTime = millis();

  while (millis() - startTime < RESPONSE_TIMEOUT) {
    while (BMS_SERIAL.available()) {
      responseData.push_back(BMS_SERIAL.read());
    }
  }

  if (responseData.size() >= 4) {
    frameReceived = true;
  }

  return responseData;
}


/* -------------------------------------------------
 * POST Request Function
 * ------------------------------------------------- */

void sendPostRequest(const vector<uint8_t>& data) {
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("WiFi not connected, cannot send POST");
    return;
  }

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");

  // Convert binary data to hex string or base64 if needed
  String hexString;
  for (uint8_t byte : data) {
    if (byte < 0x10) hexString += "0";
    hexString += String(byte, HEX);
  }

  StaticJsonDocument<512> doc;
  doc["bms_data"] = hexString;
  String payload;
  serializeJson(doc, payload);

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    Serial.printf("HTTP POST Success: %d\n", httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.printf("HTTP POST Failed: %d\n", httpResponseCode);
  }

  http.end();
}


/* -------------------------------------------------
 * Validation & Utility
 * ------------------------------------------------- */

void printHexArray(const vector<uint8_t>& data) {
  for (size_t i = 0; i < data.size(); i++) {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println();
}

bool validateHeader(const vector<uint8_t>& data) {
  return data[0] == headerStartBytes[0] && data[1] == headerStartBytes[1];
}

bool validateChecksum(const vector<uint8_t>& data) {
  if (data.size() < 4) return false;

  uint32_t checksum = 0;
  for (size_t i = 0; i < data.size() - 4; ++i) {
    checksum += data[i];
  }

  size_t csStart = data.size() - 4;
  uint32_t receivedChecksum =
    ((uint32_t)data[csStart] << 24) |
    ((uint32_t)data[csStart + 1] << 16) |
    ((uint32_t)data[csStart + 2] << 8) |
    (uint32_t)data[csStart + 3];

  return checksum == receivedChecksum;
}
