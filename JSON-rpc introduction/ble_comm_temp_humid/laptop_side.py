import asyncio
import json
from bleak import BleakClient

# Define the BLE characteristic UUIDs for temperature and humidity
TEMPERATURE_UUID = "12345678-1234-1234-1234-1234567890cd"
HUMIDITY_UUID = "12345678-1234-1234-1234-1234567890de"

# JSON-RPC Method to send requests to the Arduino
async def send_json_rpc_request(address, method, params={}):
    async with BleakClient(address) as client:
        print(f"Connected: {client.is_connected}")

        # Prepare the JSON-RPC request
        request = {
            "jsonrpc": "2.0",
            "method": method,
            "params": params,
            "id": 1
        }

        # Send the request as a JSON string to the appropriate characteristic
        request_data = json.dumps(request).encode('utf-8')
        characteristic_uuid = TEMPERATURE_UUID if method == "getTemperature" else HUMIDITY_UUID
        
        await client.write_gatt_char(characteristic_uuid, request_data)

        # Read response data (if Arduino provides a response)
        response_data = await client.read_gatt_char(characteristic_uuid)
        
        response = json.loads(response_data.decode('utf-8'))

        print(f"Response: {response}")
        return response

# Read temperature and humidity from Arduino over BLE using JSON-RPC
async def read_sensor_data(address):
    # Request temperature data
    temperature_response = await send_json_rpc_request(address, "getTemperature")
    temperature = temperature_response.get('result', 'No data')

    # Request humidity data
    humidity_response = await send_json_rpc_request(address, "getHumidity")
    humidity = humidity_response.get('result', 'No data')

    print(f"Temperature: {temperature} °C")
    print(f"Humidity: {humidity} %")

# Replace with your Arduino's BLE address
arduino_address = "87E73EC6-C46E-FE0B-B68F-AC3EB0090440"  # Example address

# Run the script to read sensor data using asyncio.run()
if __name__ == "__main__":
    asyncio.run(read_sensor_data(arduino_address))
