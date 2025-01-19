#include <ArduinoBLE.h>
#include <HTS221.h>  // Include HTS221 library for temperature and humidity sensor
#include <aJSON.h>
#include <JsonRPCServer.h>

// BLE service and characteristic UUIDs
BLEService sensorService("12345678-1234-1234-1234-1234567890ab");  // Custom service UUID
BLECharacteristic temperatureChar("12345678-1234-1234-1234-1234567890cd", BLERead, 4);  // Temperature characteristic
BLECharacteristic humidityChar("12345678-1234-1234-1234-1234567890de", BLERead, 4);     // Humidity characteristic

class TargetController : public JsonRPCServer {
public:
    TargetController(Stream* stream);

    DECLARE_JSON_PROC(TargetController, init, int);
    DECLARE_JSON_PROC(TargetController, getTemperature, float);
    DECLARE_JSON_PROC(TargetController, getHumidity, float);

    BEGIN_JSON_REGISTRATION
        REGISTER_JSON_PROC(init, JSON_RPC_RET_TYPE_NUMERIC);
        REGISTER_JSON_PROC(getTemperature, JSON_RPC_RET_TYPE_FLOAT);
        REGISTER_JSON_PROC(getHumidity, JSON_RPC_RET_TYPE_FLOAT);
    END_JSON_REGISTRATION

private:
    // HTS221 sensor initialization
    void initSensor();
};

TargetController jsonController(&Serial);

TargetController::TargetController(Stream* stream) : JsonRPCServer(stream) {}

int TargetController::init(aJsonObject* params) {
    initSensor();
    return true;
}

void TargetController::initSensor() {
    // Initialize the HTS221 sensor
    if (!HTS.begin()) {
        Serial.println("Failed to initialize HTS221 sensor!");
        while (1);  // Stay here if sensor initialization fails
    }
}

float TargetController::getTemperature(aJsonObject* params) {
    return HTS.readTemperature();  // Return the temperature in °C
}

float TargetController::getHumidity(aJsonObject* params) {
    return HTS.readHumidity();  // Return the humidity as a percentage
}

void setup() {
    // Start up BLE
    if (!BLE.begin()) {
        Serial.println("Failed to initialize BLE!");
        while (1);  // Stay here if BLE fails
    }

    BLE.setDeviceName("ArduinoNano33BLE");
    BLE.addService(sensorService);
    BLE.advertise();
    Serial.println("BLE Advertising...");

    jsonController.begin(2);  // Allow up to 2 concurrent requests
}

void loop() {
    jsonController.process();  // Process incoming JSON-RPC requests
}
