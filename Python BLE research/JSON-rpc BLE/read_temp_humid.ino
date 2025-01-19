#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>
#include <ArduinoJson.h>  // Include ArduinoJson for handling JSON

// Define BLE service and characteristic
BLEService jsonRpcService("12345678-1234-1234-1234-1234567890ab");
BLECharacteristic jsonRpcCharacteristic(
    "87654321-4321-4321-4321-abcdefabcdef", 
    BLERead | BLEWrite, 
    256  // Specify maximum size of characteristic value
);

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }

  if (!HTS.begin()) {
    Serial.println("Failed to initialize sensor!");
    while (1);
  }

  // Add characteristic to service
  jsonRpcService.addCharacteristic(jsonRpcCharacteristic);

  // Add service and start advertising
  BLE.addService(jsonRpcService);
  BLE.advertise();

  Serial.println("BLE JSON-RPC service is running...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (jsonRpcCharacteristic.valueUpdated()) {
        String jsonRequest = String((char*)jsonRpcCharacteristic.value());
        Serial.println("Received JSON request: ");
        Serial.println(jsonRequest);

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, jsonRequest);
        if (error) {
          Serial.print("JSON parse error: ");
          Serial.println(error.c_str());
          continue;
        }

        if (doc["method"] == "getStatus") {
          float temperature = HTS.readTemperature();
          float humidity = HTS.readHumidity();

          StaticJsonDocument<256> response;
          response["jsonrpc"] = "2.0";
          response["id"] = doc["id"];
          response["result"]["temperature"] = temperature;
          response["result"]["humidity"] = humidity;

          String jsonResponse;
          serializeJson(response, jsonResponse);
          jsonRpcCharacteristic.writeValue((uint8_t*)jsonResponse.c_str(), jsonResponse.length());
          Serial.println("Response sent: ");
          Serial.println(jsonResponse);
        }
      }
      delay(100);  // Allow time for processing
    }
    Serial.println("Disconnected from central");
  }
}