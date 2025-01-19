# python script for retrieving sensors data from arduino (using json rpc)
# added debugging, error handling

import asyncio
from bleak import BleakClient, BleakScanner
import json

# Service and Characteristic UUIDs (Match those in the Arduino code)
JSON_RPC_SERVICE_UUID = "12345678-1234-1234-1234-1234567890ab"
JSON_RPC_CHARACTERISTIC_UUID = "87654321-4321-4321-4321-abcdefabcdef"

# Arduino Nano 33 BLE Device Name
TARGET_DEVICE_NAME = "Arduino"

async def main():
    # Scan for devices
    print("Scanning for devices...")
    scanner = BleakScanner()
    await scanner.start()
    await asyncio.sleep(5)  # Scan for 5 seconds
    await scanner.stop()
    target_device = next((d for d in scanner.discovered_devices if d.name == TARGET_DEVICE_NAME), None)
    
    if not target_device:
        print(f"Failed to find {TARGET_DEVICE_NAME} in nearby devices.")
        return
    
    print(f"Found {TARGET_DEVICE_NAME} - {target_device.address}")
    
    # Connect to the device
    async with BleakClient(target_device) as client:
        print(f"Connected to {TARGET_DEVICE_NAME}")
        
        # Discover all services
        services = client.services

        # Find the service by its UUID
        json_rpc_service_uuid = "12345678-1234-1234-1234-1234567890ab"
        json_rpc_service = next(
            (service for service in services if service.uuid == json_rpc_service_uuid), None
        )

        if not json_rpc_service:
            print(f"Failed to find service {json_rpc_service_uuid}")
            return

        # Now, find the characteristic within this service
        json_rpc_characteristic_uuid = "87654321-4321-4321-4321-abcdefabcdef"
        characteristics = json_rpc_service.characteristics
        json_rpc_characteristic = next(
            (char for char in characteristics if char.uuid == json_rpc_characteristic_uuid), None
        )

        if not json_rpc_characteristic:
            print(f"Failed to find characteristic {json_rpc_characteristic_uuid} in service")
            return
        
        # Prepare and send the JSON-RPC request
        request_id = 1
        json_rpc_request = {
            "jsonrpc": "2.0",
            "method": "getSensorReadings",
            "params": [],
            "id": request_id
        }
        
        request_json = json.dumps(json_rpc_request)
        print(f"Sending JSON-RPC Request: {request_json}")
        await client.write_gatt_char(json_rpc_characteristic, request_json.encode())
                
        # Wait for the response with a short delay (adjust as needed)
        await asyncio.sleep(0.5)  # 0.5-second delay

        # Read the response (Assuming it's available in the characteristic value)
        response = await client.read_gatt_char(json_rpc_characteristic)
        response_json = response.decode()
        print(f"Received JSON-RPC Response: {response_json}")

        try:
            response_data = json.loads(response_json)
            if "result" in response_data:
                print(f"Temperature: {response_data['result']['temperature']}°C, Humidity: {response_data['result']['humidity']}%")
            else:
                print("No sensor readings in response.")
        except json.JSONDecodeError as e:
            print(f"Failed to parse JSON response: {e}")

asyncio.run(main())
