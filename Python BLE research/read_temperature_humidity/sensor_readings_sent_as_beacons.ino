// Sending temp and humidity info as BLE beacons (everyone could see them)

#include <Arduino_HTS221.h>
#include <ArduinoBLE.h> // Include BLE library

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

  // Set the device name and advertise with no data initially
  BLE.setDeviceName("Nano33BLE");
  BLE.setLocalName("Nano33BLE");
  BLE.advertise();
}

void loop() {
  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();

  // Check if the sensor values have significant changes
  if (abs(old_temp - temperature) >= 0.5 || abs(old_hum - humidity) >= 1) {
    old_temp = temperature;
    old_hum = humidity;

    // Print updated values to Serial Monitor
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity    = ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.println();

    // Update BLE advertisement with sensor data
    String dataPayload = "T:" + String(temperature, 1) + " H:" + String(humidity, 1);

    // Convert the String to a uint8_t array
    uint8_t dataBytes[dataPayload.length() + 1]; // +1 for null terminator
    dataPayload.getBytes(dataBytes, sizeof(dataBytes));

    // Stop advertising, update data, and restart advertising
    BLE.stopAdvertise();
    BLE.setManufacturerData(dataBytes, sizeof(dataBytes) - 1); // Exclude null terminator
    BLE.advertise();
  }

  delay(1000); // Wait 1 second before the next loop
}