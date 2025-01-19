// Simple communication with central device
// Receive info from central device and send it back as confirmation

#include <ArduinoBLE.h>

// BLE service and characteristic
BLEService customService("12345678-1234-5678-1234-56789abcdef0");
BLEStringCharacteristic customCharacteristic("abcdef01-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 32);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Set device name
  BLE.setDeviceName("Nano33BLE");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(customService);

  // Add characteristic to service
  customService.addCharacteristic(customCharacteristic);

  // Add service to BLE
  BLE.addService(customService);

  // Start advertising
  BLE.advertise();
  Serial.println("BLE Peripheral is advertising");
}

void loop() {
  // Listen for BLE central connections
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Stay connected
    while (central.connected()) {
      if (customCharacteristic.written()) {
        String receivedData = customCharacteristic.value();
        Serial.print("Received: ");
        Serial.println(receivedData);

        // Echo the data back
        customCharacteristic.writeValue("Echo: " + receivedData);
      }
    }

    Serial.println("Disconnected from central");
  }
}
