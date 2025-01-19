// different approach to send sensors reading to connected cetral device
// temperature and humidity data are hard-coded (for now)

#include <ArduinoBLE.h>
#include <ArduinoJson.h>

// Simulated sensor readings for demo purposes
float readTemperature() { return 22.5; } // Replace with actual sensor read function
float readHumidity()    { return 60.2; } // Replace with actual sensor read function

BLEService jsonRpcService("12345678-1234-1234-1234-1234567890ab"); // Custom UUID for JSON-RPC service
BLECharacteristic jsonRpcCharacteristic("87654321-4321-4321-4321-abcdefabcdef", BLERead | BLEWrite, "json-rpc");

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Set advertised local name and service
  BLE.setLocalName("Arduino");
  BLE.setAdvertisedServiceUuid(jsonRpcService.uuid()); 
  BLE.advertise();

  // Add the characteristic to the service
  jsonRpcService.addCharacteristic(jsonRpcCharacteristic);

  // Add service to BLE
  BLE.addService(jsonRpcService);

  // Initialize characteristic with empty value
  jsonRpcCharacteristic.writeValue("");

  Serial.println("BLE JSON-RPC Service Started. Waiting for connections...");
}

void loop() {
  BLE.poll();

  // Check if a device is connected
  if (BLE.connected()) {
    // Check if the characteristic's value has been written to (i.e., incoming JSON-RPC request)
if (jsonRpcCharacteristic.written()) {
      size_t jsonDataLength = jsonRpcCharacteristic.valueLength();
      const char* jsonData = (const char*)jsonRpcCharacteristic.value(); // Cast to const char*
      String incomingJson = String(jsonData, jsonDataLength); // Safe conversion to String
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
          respondWithSensorReadings(incomingDoc["id"]);
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
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length()); // Convert String to const char* and specify length
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
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length()); // Convert String to const char* and specify length
  Serial.println("Sent Sensor Readings Response:");
  Serial.println(outgoingJson);
}
