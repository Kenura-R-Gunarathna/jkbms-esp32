#include <Arduino.h>
#include <HardwareSerial.h>
#include <vector>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>
#include "mbedtls/md.h"
#include "mbedtls/aes.h"

using namespace std;

/* -------------------------------------------------
 * Constants
 * ------------------------------------------------- */

// Defaults (compile-time)
const int    DEFAULT_RX               = 16;
const int    DEFAULT_TX               = 17;
const int    DEFAULT_BAUD_RATE        = 115200;
const int    DEFAULT_RESPONSE_TIMEOUT = 100;
const int    DEFAULT_WIFI_TIMEOUT     = 500;
const int    DEFAULT_REQUEST_TIMEOUT  = 5000;
const bool   DEFAULT_DEBUG            = true;

const char* DEFAULT_DEVICE_ID     = "AA:BB:CC:DD:EE:FF";
const char* DEFAULT_DEVICE_NAME   = "MyJKBMS";
const char* DEFAULT_DEVICE_TYPE   = "ESP32-JKBMS";
const char* DEFAULT_VERSION       = "1.0.0";
const char* DEFAULT_WIFI_SSID     = "admin";
const char* DEFAULT_WIFI_PASSWORD = "pass";
const char* DEFAULT_API_URL       = "https://your-api-endpoint.com";

// AES key & IV (16 bytes each) — REPLACE with your own secure values!
static const unsigned char aes_key[16] = {
  0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B, 0x0C,0x0D,0x0E,0x0F
};
static const unsigned char aes_iv[16] = {
  0xA0,0xA1,0xA2,0xA3, 0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB, 0xAC,0xAD,0xAE,0xAF
};

/* -------------------------------------------------
 * Globals (runtime-configurable)
 * ------------------------------------------------- */

int    RX_PIN           = DEFAULT_RX;
int    TX_PIN           = DEFAULT_TX;
int    BAUD_RATE        = DEFAULT_BAUD_RATE;
int    RESPONSE_TIMEOUT = DEFAULT_RESPONSE_TIMEOUT;
int    WIFI_TIMEOUT     = DEFAULT_WIFI_TIMEOUT;
int    REQUEST_TIMEOUT  = DEFAULT_REQUEST_TIMEOUT;
bool   DEBUG            = DEFAULT_DEBUG;

String DEVICE_ID     = DEFAULT_DEVICE_ID;
String WIFI_SSID     = DEFAULT_WIFI_SSID;
String WIFI_PASSWORD = DEFAULT_WIFI_PASSWORD;
String API_URL       = DEFAULT_API_URL;

String expectedHash;               // SHA-256 hash of admin password
bool   frameReceived = false;      // flag for BMS frame
bool   authenticated = false;      // flag for serial login

Preferences prefs;
HardwareSerial BMS_SERIAL(2);      // UART2 for JKBMS

// JKBMS request/response buffers
const vector<uint8_t> requestData = {
  0x4E,0x57,0x00,0x13,0x00,0x00,0x00,0x00,
  0x06,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x68,0x00,0x00,0x01,0x29
};
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

// JKBMS header bytes
const uint8_t headerStartBytes[] = {0x4E, 0x57};

/* -------------------------------------------------
 * Prototypes
 * ------------------------------------------------- */
void    loadOrInitializePassword();
void    loadConfig();
void    handleSerialInput();
void    aes_encrypt(const unsigned char* in, size_t len, unsigned char* out, size_t& outLen);
void    aes_decrypt(const unsigned char* in, size_t len, unsigned char* out, size_t& outLen);
String  sha256(const String& in);
String  toHex(const unsigned char* buf, size_t len);
size_t  fromHex(const String& hex, unsigned char* buf, size_t maxLen);

void    sendBMSRequest();
vector<uint8_t> readBMSDataVector();
void    printHexArray(const vector<uint8_t>& data);
bool    validateHeader(const vector<uint8_t>& data);
bool    validateChecksum(const vector<uint8_t>& data);
void    sendPostRequest(const vector<uint8_t>& data);

/* -------------------------------------------------
 * Setup
 * ------------------------------------------------- */
void setup() {
  Serial.begin(BAUD_RATE);
  loadOrInitializePassword();
  loadConfig();

  BMS_SERIAL.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("\nESP32 JKBMS Decoder Starting…");

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi…");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_TIMEOUT); Serial.print('.');
  }
  Serial.println(" connected.");
  
  DEVICE_ID = getMacAddress();
}

/* -------------------------------------------------
 * Main Loop
 * ------------------------------------------------- */
void loop() {
  handleSerialInput();

  if (DEBUG) {
    Serial.print("Sent request: "); printHexArray(requestData);
  }

  // BMS request/response
  sendBMSRequest();
  delay(RESPONSE_TIMEOUT);
  auto response = readBMSDataVector();

  if (response.empty()) {
    Serial.println("No response!");
    if (!DEBUG) return;
    response = sampleResponseData;
  } else {
    Serial.print("Received: "); printHexArray(response);
  }

  if (frameReceived) {
    Serial.println(validateHeader(response) ? "Header OK" : "Header FAIL");
    Serial.println(validateChecksum(response) ? "Checksum OK" : "Checksum FAIL");
    sendPostRequest(response);
    frameReceived = false;
  }

  delay(REQUEST_TIMEOUT);
}

/* -------------------------------------------------
 * Load / Initialize Admin Password
 * ------------------------------------------------- */
void loadOrInitializePassword() {
  prefs.begin("config", false);
  if (prefs.isKey("adminHash")) {
    expectedHash = prefs.getString("adminHash");
  } else {
    expectedHash = sha256("pass");
    prefs.putString("adminHash", expectedHash);
  }
  prefs.end();
}

/* -------------------------------------------------
 * Load Wi-Fi + Other Config from NVS
 * ------------------------------------------------- */
void loadConfig() {
  prefs.begin("config", true);
  RX_PIN           = prefs.getInt   ("rxPin",   DEFAULT_RX);
  TX_PIN           = prefs.getInt   ("txPin",   DEFAULT_TX);
  BAUD_RATE        = prefs.getInt   ("baud",    DEFAULT_BAUD_RATE);
  RESPONSE_TIMEOUT = prefs.getInt   ("rTO",     DEFAULT_RESPONSE_TIMEOUT);
  WIFI_TIMEOUT     = prefs.getInt   ("wTO",     DEFAULT_WIFI_TIMEOUT);
  REQUEST_TIMEOUT  = prefs.getInt   ("reqTO",   DEFAULT_REQUEST_TIMEOUT);
  DEBUG            = prefs.getBool  ("dbg",     DEFAULT_DEBUG);
  API_URL          = prefs.getString("api",     DEFAULT_API_URL);

  // Decrypt SSID & password if stored
  String hexSSID = prefs.getString("ssid", "");
  if (hexSSID.length()) {
    unsigned char buf[128]; size_t bl;
    size_t hlen = fromHex(hexSSID, buf, sizeof(buf));
    unsigned char out[128]; size_t olen;
    aes_decrypt(buf, hlen, out, olen);
    WIFI_SSID = String((char*)out);
  }

  String hexPASS = prefs.getString("pass", "");
  if (hexPASS.length()) {
    unsigned char buf[128]; size_t bl;
    size_t hlen = fromHex(hexPASS, buf, sizeof(buf));
    unsigned char out[128]; size_t olen;
    aes_decrypt(buf, hlen, out, olen);
    WIFI_PASSWORD = String((char*)out);
  }
  prefs.end();
}

/* -------------------------------------------------
 * Serial / CLI Handler
 * ------------------------------------------------- */
/**
 * @brief Handles commands received over the primary Serial interface.
 *
 * This function reads incoming serial data, parses commands, and acts upon them.
 * Some commands require prior authentication using the 'login' command.
 * Configuration changes are saved to NVS (Non-Volatile Storage) using Preferences.
 *
 * Available Commands:
 *
 * Unauthenticated:
 *   login <username> <password> - Authenticates the user. Default username is "admin".
 *                                 The initial default password is "pass", which is then
 *                                 hashed and stored. Subsequent logins use the hashed password.
 *                                 Example: login admin mysecretpassword
 *
 * Authenticated Commands:
 *   logout                        - De-authenticates the current user.
 *
 *   ssid=<your_wifi_ssid>         - Sets and saves the WiFi SSID. The SSID is encrypted before storing.
 *                                   Example: ssid=MyHomeNetwork
 *
 *   wifiPass=<your_wifi_password> - Sets and saves the WiFi password. The password is encrypted before storing.
 *                                   Example: wifiPass=mySecureWiFiPassword123
 *
 *   adminPass=<new_admin_pass>    - Changes the admin password for serial CLI login.
 *                                   The new password is an unhashed string, which will be
 *                                   hashed and stored.
 *                                   Example: adminPass=newAdminPassword456
 *
 *   rxPin=<pin_number>            - Sets and saves the RX pin for BMS_SERIAL (UART2).
 *                                   Note: A device reboot or manual re-initialization of
 *                                   BMS_SERIAL is required for this change to take effect.
 *                                   Example: rxPin=16
 *
 *   txPin=<pin_number>            - Sets and saves the TX pin for BMS_SERIAL (UART2).
 *                                   Note: A device reboot or manual re-initialization of
 *                                   BMS_SERIAL is required for this change to take effect.
 *                                   Example: txPin=17
 *
 *   baud=<baud_rate>              - Sets and saves the baud rate for both the primary Serial
 *                                   and BMS_SERIAL (UART2). The primary Serial is re-initialized
 *                                   immediately.
 *                                   Note: For BMS_SERIAL, a device reboot or manual
 *                                   re-initialization is required for the change to take full effect.
 *                                   Example: baud=115200
 *
 *   rTO=<timeout_ms>              - Sets and saves the BMS response timeout in milliseconds.
 *                                   This is the time to wait for a response from the BMS
 *                                   after sending a request.
 *                                   Example: rTO=100
 *
 *   wTO=<timeout_ms>              - Sets and saves the WiFi connection timeout in milliseconds.
 *                                   This is the duration for each attempt/delay when connecting to WiFi.
 *                                   Example: wTO=500
 *
 *   reqTO=<timeout_ms>            - Sets and saves the main loop request timeout in milliseconds.
 *                                   This is the delay at the end of the main loop before
 *                                   the next BMS request cycle.
 *                                   Example: reqTO=5000
 *
 *   dbg=<1|0|true|false>          - Sets and saves the debug mode. '1' or 'true' enables
 *                                   verbose logging. '0' or 'false' disables it.
 *                                   Example: dbg=1
 *                                   Example: dbg=false
 *
 *   api=<full_api_url>            - Sets and saves the API endpoint URL for POSTing BMS data.
 *                                   Example: api=https://your-api.com/data
 *
 * If an unrecognized command is entered while authenticated, "Unknown cmd" is printed.
 * If a command requiring authentication is entered without logging in, a login prompt is shown.
 */
void handleSerialInput() {
  if (!Serial.available()) return;
  String s = Serial.readStringUntil('\n'); s.trim();

  prefs.begin("config", false);
  // not logged in? require login
  if (!authenticated) {
    if (s.startsWith("login ")) {
      int sp = s.indexOf(' ', 6);
      if (sp>6) {
        String u = s.substring(6,sp), p = s.substring(sp+1);
        if (u=="admin" && sha256(p)==expectedHash) {
          authenticated = true; Serial.println("Login OK");
        } else Serial.println("Bad creds");
      } else Serial.println("Usage: login <user> <pass>");
    } else {
      Serial.println("Please login first: login admin <pass>");
    }
    prefs.end();
    return;
  }

  // AUTHENTICATED COMMANDS:
  if (s.startsWith("ssid=")) {
    String v = s.substring(5);
    // encrypt & store
    unsigned char buf[128]={0}, out[128]={0}; size_t olen;
    size_t len=v.length();
    size_t pad = ((len+15)/16)*16;
    memcpy(buf, v.c_str(), len);
    aes_encrypt(buf,pad,out,olen);
    prefs.putString("ssid", toHex(out,olen));
    Serial.println("SSID saved.");
  }
  else if (s.startsWith("wifiPass=")) {
    String v = s.substring(9);
    unsigned char buf[128]={0}, out[128]={0}; size_t olen;
    size_t len=v.length();
    size_t pad = ((len+15)/16)*16;
    memcpy(buf, v.c_str(), len);
    aes_encrypt(buf,pad,out,olen);
    prefs.putString("pass", toHex(out,olen));
    Serial.println("WiFi pass saved.");
  }
  else if (s.startsWith("adminPass=")) {
    String np = s.substring(10);
    expectedHash = sha256(np);
    prefs.putString("adminHash", expectedHash);
    Serial.println("Admin pass updated.");
  }
  else if (s=="logout") {
    authenticated = false; Serial.println("Logged out");
  }
  // Config commands
  else if (s.startsWith("rxPin=")) {
    int v = s.substring(strlen("rxPin=")).toInt();
    prefs.putInt("rxPin", v);
    RX_PIN = v;
    Serial.printf("RX_PIN set to %d\n", v);
  }
  else if (s.startsWith("txPin=")) {
    int v = s.substring(strlen("txPin=")).toInt();
    prefs.putInt("txPin", v);
    TX_PIN = v;
    Serial.printf("TX_PIN set to %d\n", v);
  }
  else if (s.startsWith("baud=")) {
    int v = s.substring(strlen("baud=")).toInt();
    prefs.putInt("baud", v);
    BAUD_RATE = v;
    Serial.printf("BAUD_RATE set to %d\n", v);
    // re-init Serial if you like:
    Serial.begin(BAUD_RATE);
  }
  else if (s.startsWith("rTO=")) {
    int v = s.substring(strlen("rTO=")).toInt();
    prefs.putInt("rTO", v);
    RESPONSE_TIMEOUT = v;
    Serial.printf("RESPONSE_TIMEOUT set to %d ms\n", v);
  }
  else if (s.startsWith("wTO=")) {
    int v = s.substring(strlen("wTO=")).toInt();
    prefs.putInt("wTO", v);
    WIFI_TIMEOUT = v;
    Serial.printf("WIFI_TIMEOUT set to %d ms\n", v);
  }
  else if (s.startsWith("reqTO=")) {
    int v = s.substring(strlen("reqTO=")).toInt();
    prefs.putInt("reqTO", v);
    REQUEST_TIMEOUT = v;
    Serial.printf("REQUEST_TIMEOUT set to %d ms\n", v);
  }
  else if (s.startsWith("dbg=")) {
    bool v = (s.substring(strlen("dbg=")) == "1" || s.substring(strlen("dbg=")) == "true");
    prefs.putBool("dbg", v);
    DEBUG = v;
    Serial.printf("DEBUG set to %s\n", DEBUG ? "true" : "false");
  }
  else if (s.startsWith("api=")) {
    String v = s.substring(strlen("api="));
    prefs.putString("api", v);
    API_URL = v;
    Serial.printf("API_URL set to %s\n", v.c_str());
  }
  else Serial.println("Unknown cmd");

  prefs.end();
}

/* -------------------------------------------------
 * AES-CBC Encrypt / Decrypt + Hex Encoding
 * ------------------------------------------------- */
void aes_encrypt(const unsigned char* in, size_t len, unsigned char* out, size_t& outLen) {
  mbedtls_aes_context aes; unsigned char iv[16];
  memcpy(iv,aes_iv,16);
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, aes_key, 128);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, len, iv, in, out);
  mbedtls_aes_free(&aes);
  outLen = len;
}

void aes_decrypt(const unsigned char* in, size_t len, unsigned char* out, size_t& outLen) {
  mbedtls_aes_context aes; unsigned char iv[16];
  memcpy(iv,aes_iv,16);
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, aes_key, 128);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, len, iv, in, out);
  mbedtls_aes_free(&aes);
  outLen = len;
}

String toHex(const unsigned char* buf, size_t len) {
  String h;
  for (size_t i=0;i<len;i++) {
    char b[3]; sprintf(b,"%02X", buf[i]);
    h += b;
  }
  return h;
}

size_t fromHex(const String& hex, unsigned char* buf, size_t maxLen) {
  size_t out=0;
  for (size_t i=0;i<hex.length() && out<maxLen;i+=2) {
    buf[out++] = strtoul(hex.substring(i, i+2).c_str(), nullptr, 16);
  }
  return out;
}

/* -------------------------------------------------
 * SHA-256 Hash
 * ------------------------------------------------- */
String sha256(const String& in) {
  byte h[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx,(const unsigned char*)in.c_str(),in.length());
  mbedtls_md_finish(&ctx,h);
  mbedtls_md_free(&ctx);

  char str[65];
  for(int i=0;i<32;i++) sprintf(str+i*2,"%02x",h[i]);
  return String(str);
}

/* -------------------------------------------------
 * JKBMS Serial & HTTP
 * ------------------------------------------------- */
void sendBMSRequest() {
  BMS_SERIAL.write(requestData.data(), requestData.size());
}

vector<uint8_t> readBMSDataVector() {
  vector<uint8_t> r; unsigned long t=millis();
  while (millis()-t < RESPONSE_TIMEOUT) {
    while (BMS_SERIAL.available()) {
      r.push_back(BMS_SERIAL.read());
    }
  }
  if (r.size()>=4) frameReceived=true;
  return r;
}

void printHexArray(const vector<uint8_t>& d) {
  for(auto b:d) Serial.printf("%02X ",b);
  Serial.println();
}

bool validateHeader(const vector<uint8_t>& d) {
  return d.size()>1 && d[0]==headerStartBytes[0] && d[1]==headerStartBytes[1];
}

bool validateChecksum(const vector<uint8_t>& d) {
  if (d.size()<4) return false;
  uint32_t s=0;
  for(size_t i=0;i<d.size()-4;i++) s+=d[i];
  uint32_t r =
    ((uint32_t)d[d.size()-4]<<24)|
    ((uint32_t)d[d.size()-3]<<16)|
    ((uint32_t)d[d.size()-2]<<8) |
    (uint32_t)d[d.size()-1];
  return s==r;
}

void sendPostRequest(const vector<uint8_t>& data) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ERROR] WiFi not connected");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();  // Use only if you don't have a certificate. Replace with `client.setCACert(...)` for real security.

  HTTPClient https;
  if (https.begin(client, API_URL)) {
    https.addHeader("Content-Type", "application/json");

    // Convert data vector to hex string
    String hexData = "[";
    for (size_t i = 0; i < data.size(); ++i) {
      hexData += "0x" + String(data[i], HEX);
      if (i < data.size() - 1) hexData += ",";
    }
    hexData += "]";

    // Build JSON
    DynamicJsonDocument doc(2048);
    doc["deviceId"] = DEVICE_ID;
    doc["version"] = DEFAULT_VERSION;
    doc["isDebug"] = DEBUG ? "true" : "false";
    doc["timestampMs"] = millis();
    doc["hexData"] = hexData;
    
    String requestBody;
    serializeJson(doc, requestBody);

    // Send request
    int httpCode = https.POST(requestBody);

    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      String payload = https.getString();
      Serial.println("[HTTP] Response: " + payload);
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.println("[ERROR] Unable to connect to API URL");
  }
}

// Function to get MAC address as a String
String getMacAddress() {
  uint8_t mac[6];
  esp_efuse_mac_get_default(mac);

  char macStr[18] = {0}; // AA:BB:CC:DD:EE:FF + null terminator
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}