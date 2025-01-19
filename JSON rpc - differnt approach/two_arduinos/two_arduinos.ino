// arduino code which enables communication of central device to few different arduinos 
// user could choose to which device to connect, based on arduinos ble names

#include <ArduinoBLE.h>
#include <ArduinoJson.h>

// **CHANGE THIS FOR EACH DEVICE**
const char* LOCAL_NAME = "Nano33BLE-JSON-RPC-1"; // Use "Nano33BLE-JSON-RPC-2" for the second device

// Simulated sensor readings for demo purposes
float readTemperature() { return 22.5; } // Replace with actual sensor read function
float readHumidity()    { return 60.2; } // Replace with actual sensor read function

BLEService jsonRpcService("91ED0001-0000-0000-0000-000000000000");
BLECharacteristic jsonRpcCharacteristic("91ED0002-0000-0000-0000-000000000000", BLERead | BLEWrite, "json-rpc");

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }
  
  BLE.setLocalName(LOCAL_NAME);
  BLE.setAdvertisedService(jsonRpcService);
  jsonRpcService.addCharacteristic(jsonRpcCharacteristic);
  BLE.addService(jsonRpcService);
  jsonRpcCharacteristic.writeValue("");
  BLE.advertise();
  Serial.println("BLE JSON-RPC Service Started. Waiting for connections...");
}

void loop() {
  BLE.poll();
  
  if (BLE.connected()) {
    if (jsonRpcCharacteristic.written()) {
      size_t jsonDataLength = jsonRpcCharacteristic.valueLength();
      const char* jsonData = (const char*)jsonRpcCharacteristic.value();
      String incomingJson = String(jsonData, jsonDataLength);
      
      Serial.println("Incoming JSON-RPC Request:");
      Serial.println(incomingJson);
      
      DynamicJsonDocument incomingDoc(2048);
      DeserializationError error = deserializeJson(incomingDoc, jsonData, jsonDataLength);
      
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        respondWithError("Parse error", 1); // Assuming id is always present for simplicity
      } else {
        if (incomingDoc["method"] == "getSensorReadings") {
          const char* targetDevice = incomingDoc["params"][0]["device"]; // Check target device
          
          if (strcmp(targetDevice, LOCAL_NAME) == 0) { // If request is for this device
            respondWithSensorReadings(incomingDoc["id"]);
          } else { // Ignore requests not meant for this device
            Serial.println("Ignoring request not meant for this device.");
          }
        } else {
          respondWithError("Method not found", incomingDoc["id"]);
        }
      }
    }
  }
}

void respondWithError(const char* errorMessage, int id) {
  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["error"]["code"] = -32600; // Method not found
  outgoingDoc["error"]["message"] = errorMessage;
  outgoingDoc["id"] = id;
  
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Error Response:");
  Serial.println(outgoingJson);
}

void respondWithSensorReadings(int id) {
  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["result"]["temperature"] = readTemperature();
  outgoingDoc["result"]["humidity"] = readHumidity();
  outgoingDoc["id"] = id;
  
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Sensor Readings Response:");
  Serial.println(outgoingJson);
}
