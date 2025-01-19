// arduino code which enables communication of central device to few different arduinos 
// user could choose to which device to connect, based on arduinos ble names
// user could also choose which readings to retrieve (e.g. just temperature readings, both temperature and humidity readings, etc.)

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

      if (!error) {
        const char* method = incomingDoc["method"];

        // Check if targetDevice exists in params
        const char* targetDevice = NULL;
        if (incomingDoc["params"].is<JsonArray>() && incomingDoc["params"].size() > 0) {
          JsonVariant params = incomingDoc["params"][0];
          if (params.is<JsonObject>() && params.as<JsonObject>().containsKey("device")) {
            targetDevice = params["device"];
          }
        }

        // Check if request is for this device
        if (targetDevice != NULL && strcmp(targetDevice, LOCAL_NAME) != 0) { // If request is not for this device
          Serial.println("Ignoring request not meant for this device.");
          return; // Ignore requests not meant for this device
        }

        if (strcmp(method, "getTemperature") == 0) {
          respondWithTemperature(incomingDoc["id"]);
        } else if (strcmp(method, "getHumidity") == 0) {
          respondWithHumidity(incomingDoc["id"]);
        } else if (strcmp(method, "getSensorReadings") == 0) {
          respondWithSensorReadings(incomingDoc["id"]);
        } else {
          respondWithMethodError("Unknown method", method, incomingDoc["id"]);
        }
      } else {
        // Handle deserialization error
        respondWithParseError("JSON deserialization failed", error.c_str(), incomingDoc["id"]);
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      }  
    }
  }
}

void respondWithMethodError(const char* errorMessage, const char* methodName, int id) {
  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["error"]["code"] = -32600; // Method not found
  outgoingDoc["error"]["message"] = errorMessage;
  outgoingDoc["error"]["data"]["method"] = methodName;
  outgoingDoc["id"] = id;
  
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Error Response (Unknown Method):");
  Serial.println(outgoingJson);
}


void respondWithParseError(const char* errorMessage, const char* errorDetails, int id) {
  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["error"]["code"] = -32700; // Parse error
  outgoingDoc["error"]["message"] = errorMessage;
  outgoingDoc["error"]["data"]["details"] = errorDetails;
  outgoingDoc["id"] = id;
  
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Error Response (Deserialization):");
  Serial.println(outgoingJson);
}

// Example response functions
void respondWithTemperature(int id) {
  if (id == 0) {
    Serial.println("Warning: ID is null or empty.");
    return;
  }
  
  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["result"]["temperature"] = readTemperature();
  outgoingDoc["id"] = id;
  
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Temperature Readings Response to Client:");
  Serial.println(outgoingJson);
}


void respondWithHumidity(int id) {
  if (id == 0) {
    Serial.println("Warning: ID is null or empty.");
    return;
  }

  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["result"]["humidity"] = readHumidity();
  outgoingDoc["id"] = id;
  // Send response
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Humidity Readings Response to Client:");
  Serial.println(outgoingJson);
}

void respondWithSensorReadings(int id) {
  if (id == 0) {
    Serial.println("Warning: ID is null or empty.");
    return;
  }

  DynamicJsonDocument outgoingDoc(2048);
  outgoingDoc["jsonrpc"] = "2.0";
  outgoingDoc["result"]["temperature"] = readTemperature();
  outgoingDoc["result"]["humidity"] = readHumidity();
  outgoingDoc["id"] = id;
  
  String outgoingJson;
  serializeJson(outgoingDoc, outgoingJson);
  jsonRpcCharacteristic.writeValue((const uint8_t*)outgoingJson.c_str(), outgoingJson.length());
  Serial.println("Sent Sensor Readings Response to Client:");
  Serial.println(outgoingJson);
}
