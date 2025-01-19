// Sending sensors readings from arduino to connected central device (e.g. laptop)

#include <Arduino_HTS221.h>
#include <ArduinoBLE.h> // Include BLE library

BLEService sensorService("180A"); // Create a custom BLE service
BLEFloatCharacteristic temperatureCharacteristic("2A6E", BLERead | BLENotify); // Temperature characteristic
BLEFloatCharacteristic humidityCharacteristic("2A6F", BLERead | BLENotify);    // Humidity characteristic

float old_temp = 0;
float old_hum = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  Serial.println("BLE initialized.");

  // Add characteristics to the service
  sensorService.addCharacteristic(temperatureCharacteristic);
  sensorService.addCharacteristic(humidityCharacteristic);

  // Add the service to the peripheral
  BLE.addService(sensorService);

  // Set initial values for the characteristics
  temperatureCharacteristic.writeValue(0.0);
  humidityCharacteristic.writeValue(0.0);

  // Set BLE device name and start advertising
  BLE.setDeviceName("Nano33BLE");
  BLE.setLocalName("Nano33BLE");
  BLE.advertise();

  Serial.println("BLE service is now advertising.");
}

void loop() {
  // Listen for BLE connections
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      float temperature = HTS.readTemperature();
      float humidity    = HTS.readHumidity();

      // Update values only if there's a significant change
      if (abs(old_temp - temperature) >= 0.5 || abs(old_hum - humidity) >= 1) {
        old_temp = temperature;
        old_hum = humidity;

        // Write updated sensor values to characteristics
        temperatureCharacteristic.writeValue(temperature);
        humidityCharacteristic.writeValue(humidity);

        // Print to Serial Monitor for debugging
        Serial.print("Updated Temperature = ");
        Serial.print(temperature);
        Serial.println(" °C");
        Serial.print("Updated Humidity    = ");
        Serial.print(humidity);
        Serial.println(" %");
      }

      delay(1000); // Wait before checking again
    }

    Serial.println("Central device disconnected.");
  }
}