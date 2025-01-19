// sending sensors readings over serial communication

#include <Arduino_HTS221.h> // Include the HTS221 library
#include <aJSON.h>
#include <JsonRPCServer.h>

// Extend TargetController to handle temperature and humidity
class TargetController: public JsonRPCServer {
public:
    TargetController(Stream* stream);

    DECLARE_JSON_PROC(TargetController, init, int);
    DECLARE_JSON_PROC(TargetController, toggleLED, String);
    DECLARE_JSON_PROC(TargetController, readTemperature, float); // New method
    DECLARE_JSON_PROC(TargetController, readHumidity, float);    // Optional method

    BEGIN_JSON_REGISTRATION
        REGISTER_JSON_PROC(init, JSON_RPC_RET_TYPE_NUMERIC);
        REGISTER_JSON_PROC(toggleLED, JSON_RPC_RET_TYPE_STRING);
        REGISTER_JSON_PROC(readTemperature, JSON_RPC_RET_TYPE_FLOAT); // Register temperature method
        REGISTER_JSON_PROC(readHumidity, JSON_RPC_RET_TYPE_FLOAT);    // Register humidity method
    END_JSON_REGISTRATION

private:
    int _led; // LED pin
};

// Constructor
TargetController::TargetController(Stream* stream): JsonRPCServer(stream), _led(13) {}

// Initialize LED pin
int TargetController::init(aJsonObject* params) {
    pinMode(_led, OUTPUT);
    // Initialize HTS221 sensor
    if (!HTS.begin()) {
        Serial.println("Failed to initialize HTS221 sensor!");
        return false;
    }
    return true;
}

// Toggle LED state
String TargetController::toggleLED(aJsonObject* params) {
    aJsonObject* statusParam = aJson.getObjectItem(params, "status");
    boolean requestedStatus = statusParam->valuebool;

    if (requestedStatus) {
        digitalWrite(_led, HIGH);
        return "led HIGH";
    } else {
        digitalWrite(_led, LOW);
        return "led LOW";
    }
}

// Read the ambient temperature from HTS221
float TargetController::readTemperature(aJsonObject* params) {
    return HTS.readTemperature();
}

// Read the ambient humidity from HTS221
float TargetController::readHumidity(aJsonObject* params) {
    return HTS.readHumidity();
}

// Create the JSON-RPC server instance
TargetController jsonController(&Serial);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing JSON RPC server");
    jsonController.begin(4); // Allow up to 4 concurrent requests
}

void loop() {
    jsonController.process();
}
