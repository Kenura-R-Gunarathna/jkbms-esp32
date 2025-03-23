#include <Arduino.h>
#include <HardwareSerial.h>
#include <map>
#include <vector>
#include "BMSConfig.hpp"


#define RX 16 // GPIO pin used for receiving data from JKBMS (JKBMS.TX --> ESP32.GPIO:RX)
#define TX 17 // GPIO pin used for transmitting data to the JKBMS device (JKBMS.RX --> ESP32.GPIO:TX)
HardwareSerial BMS_SERIAL(2); // Use BMS_SERIAL for UART2

#define BMS_BAUD_RATE 115200 // Adjust to your BMS's baud rate
#define DEBUG true

#define MAX_DATA_LENGTH 200 // Increased for safety (adjust based on your frame size)

// Request data to be sent to the BMS
const byte requestData[] = {0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x01, 0x29};

byte data[MAX_DATA_LENGTH]; // Array to store received data
int dataIndex = 0;          // Index to track current position in the array
bool frameReceived = false; // Flag to indicate a complete frame has been received

// --- Global Configuration ---
BMSConfig config = initializeConfig(); // Initialize the config

// Function Prototypes
void sendBMSRequest();
void readBMSData();
void decodeBMSData(const byte* data);
void printHexArray(const uint8_t *data, size_t len);
void printDecodedData(const byte* data);

// Helper function to convert bytes to an integer number
template <typename T>
T bytesToNumber(const byte* data, int index, int length) {
    static_assert(std::is_integral<T>::value, "T must be an integer type");
    T result = 0;
    for (int i = 0; i < length; ++i) {
        result <<= 8;
        result |= data[index + i];
    }
    return result;
}

// Helper function to convert bytes to a floating-point number
float bytesToFloat(const byte* data, int index, int length, float scale) {
    unsigned long result = 0;
    for (int i = 0; i < length; ++i) {
        result <<= 8;
        result |= data[index + i];
    }
    return (float)result * scale;
}

void setup() {
  Serial.begin(115200); // Initialize Serial Monitor for debugging
  BMS_SERIAL.begin(BMS_BAUD_RATE, SERIAL_8N1, RX, TX); // Initialize Serial1 for BMS communication (RX pin, TX pin)

  Serial.println("ESP32 JKBMS Decoder Starting...");
}

void loop() {
  sendBMSRequest(); // Send request to BMS
  delay(100);       // Wait for a short period to receive the response
  readBMSData();    // Read data from the BMS

  if (frameReceived) {
    decodeBMSData(data);    // Decode the data
    frameReceived = false;  // Reset the flag
  }

  delay(5000); // Wait for a while before sending the next request
}

// Function to send request to the BMS
void sendBMSRequest() {
  BMS_SERIAL.write(requestData, sizeof(requestData));
  if (DEBUG) {
    Serial.print("Sent request: ");
    for (int i = 0; i < sizeof(requestData); i++) {
      Serial.print(requestData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

// Function to print a byte array in hexadecimal
void printHexArray(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    Serial.printf("%02X ", data[i]); // Print each byte in 2-digit hexadecimal
  }
  Serial.println(); // New line after printing all bytes
}

// Function to read data from the BMS serial port
void readBMSData() {
    dataIndex = 0;
    frameReceived = false;
    unsigned long startTime = millis(); // Start timeout counter
    std::vector<uint8_t> responseData;

    while (millis() - startTime < 1000) { // Wait for up to 1000ms (1 second)
        if (BMS_SERIAL.available() > 0) {
            byte inByte = BMS_SERIAL.read();
            responseData.push_back(inByte);

            // Detect start sequence (4E 57)
            if (dataIndex == 0 && inByte != 0x4E) {
                continue; // Ignore until we get the correct start byte
            }

            data[dataIndex++] = inByte;

            // Stop if we reach expected frame length (adjust MAX_DATA_LENGTH accordingly)
            if (dataIndex >= MAX_DATA_LENGTH) {
                frameReceived = true;
                break;
            }
        }
    }

    if (frameReceived) {
        Serial.println("Received response from JKBMS (Hex):");
        printHexArray(responseData.data(), responseData.size());
    } else {
        Serial.println("Error: No valid response received within timeout.");
    }
}

void decodeBMSData(const byte* data) {
  // Check header
  if (data[0] != strtol(config.header.start_bytes[0].c_str(), NULL, 16) || data[1] != strtol(config.header.start_bytes[1].c_str(), NULL, 16)) {
    Serial.println("Invalid Header!");
    return;
  }

  printDecodedData(data);
}

// Function to decode and print BMS data in a tabular format
void printDecodedData(const byte* data) {
  Serial.println("------------------------------------------------");
  Serial.println("JKBMS Data:");
  Serial.println("------------------------------------------------");

  for (const auto& [key, fieldConfig] : config.dataFields) {
      Serial.print("Processing: ");
      Serial.println(key);

    long code = strtol(fieldConfig.code.c_str(), NULL, 16);

    if (code == 0x79) { // Cell Voltages
      Serial.println("--- Cell Voltages ---");
      int cellcountByteIndex = fieldConfig.cellcount_byte_index;
      int cellCountMultiplier = fieldConfig.cell_count_multiplier;

      byte cellVoltageByteCount = data[cellcountByteIndex]; // Number of bytes of cell voltages
      int numCells = cellVoltageByteCount / cellCountMultiplier;
      int cellIndexBase = fieldConfig.cell.cell_index_base; // Where the cell info begins

      for (int j = 0; j < numCells; j++) {
        int cellNumber = data[cellIndexBase + j * cellCountMultiplier];
        int voltageIndex = cellIndexBase + j * cellCountMultiplier + 1;

        float scale = fieldConfig.cell.scale;
        int length = fieldConfig.cell.data_length;
        float voltage = bytesToFloat(data, voltageIndex, length, scale);

        Serial.print("Cell ");
        Serial.print(cellNumber);
        Serial.print(" Voltage: ");
        Serial.print(voltage ); // Convert mV to V
        Serial.println(" V");
      }
    } else { // all other data points
      // Generic data processing based on structure configuration
      int index = fieldConfig.index;
      int length = fieldConfig.data_length;
      const char* label = fieldConfig.label.c_str();
      const char* unit = fieldConfig.unit.c_str();
      const char* type = fieldConfig.type.c_str();
      float scale = fieldConfig.scale;

      Serial.print(label); // Use the label for printing
      Serial.print(": ");

      if (strcmp(type, "int") == 0) {
        int value = bytesToNumber<int>(data, index, length);
        Serial.print(value);
      } else if (strcmp(type, "float") == 0) {
           float value = bytesToFloat(data, index, length, scale);
           if (fieldConfig.signed_value) {
                if ((data[index] & 0x80) == 0x80) {
                    value = (bytesToFloat(data, index, length, scale) - 65536) * scale;
               }
            }
        Serial.print(value);
      } else if (strcmp(type, "long") == 0) {
        long value = bytesToNumber<long>(data, index, length);
        Serial.print(value);
      } else if (strcmp(type, "string") == 0) {
        char value[length + 1];
        for (int k = 0; k < length; k++) {
          value[k] = (char)data[index + k];
        }
        value[length] = '\0';
        Serial.print(value);
      }

      if (unit != NULL && strlen(unit) > 0) {
        Serial.print(" ");
        Serial.print(unit);
      }
      Serial.println();
    }
    Serial.println();
  }

  Serial.println("------------------------------------------------");
  Serial.println();
}