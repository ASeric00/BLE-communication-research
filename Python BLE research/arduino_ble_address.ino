// arduino code which was used to get ble address of arduino nano
#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Get the local BLE address
  String bleAddress = BLE.address();
  Serial.print("BLE Address: ");
  Serial.println(bleAddress);

  // You can stop BLE if not needed after getting the address
  BLE.end();
}

void loop() {
  // Nothing needed here
}

// e8:8f:5d:88:27:e3
// _ws.col.def_dst == "e8:8f:5d:88:27:e3"
