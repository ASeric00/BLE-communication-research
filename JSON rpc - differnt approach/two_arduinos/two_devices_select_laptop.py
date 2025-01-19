# python script which enables communication of central device to few different arduinos 
# user could choose to which device to connect, based on arduinos ble names
# user could also choose which readings to retrieve (e.g. just temperature readings, both temperature and humidity readings, etc.)

import asyncio
from bleak import BleakClient, BleakScanner
import json

# Service and Characteristic UUIDs (Match those in the Arduino code)
JSON_RPC_SERVICE_UUID = "91ED0001-0000-0000-0000-000000000000"
JSON_RPC_CHARACTERISTIC_UUID = "91ED0002-0000-0000-0000-000000000000"

# Arduino Nano 33 BLE Device Names
TARGET_DEVICE_NAMES = ["Nano33BLE-JSON-RPC-1", "Nano33BLE-JSON-RPC-2"]

async def main():
    # Scan for devices
    print("Scanning for devices...")
    scanner = BleakScanner()
    await scanner.start()
    await asyncio.sleep(5)  # Scan for 5 seconds
    await scanner.stop()

    # Find target devices
    target_devices = {device.name: device for device in scanner.discovered_devices if device.name in TARGET_DEVICE_NAMES}
    
    if not target_devices:
        print("No target devices found.")
        return
    
    # Select target device
    print("Select target device:")
    for i, (name, device) in enumerate(target_devices.items()):
        print(f"{i+1}. {name} - {device.address}")
    
    choice = input("Enter the number of your choice: ")
    try:
        choice = int(choice)
        if choice < 1 or choice > len(target_devices):
            print("Invalid choice.")
            return
    except ValueError:
        print("Invalid input. Please enter a number.")
        return
    
    selected_name = list(target_devices.keys())[choice - 1]
    target_device = target_devices[selected_name]
    
    print(f"Connecting to {selected_name} - {target_device.address}...")
    
    async with BleakClient(target_device) as client:
        print(f"Connected to {selected_name}")
        
        # Discover the service and characteristic
        svc = await client.get_service(JSON_RPC_SERVICE_UUID)
        if not svc:
            print(f"Failed to find service {JSON_RPC_SERVICE_UUID}")
            return
        
        char = next((c for c in svc.characteristics if c.uuid == JSON_RPC_CHARACTERISTIC_UUID), None)
        if not char:
            print(f"Failed to find characteristic {JSON_RPC_CHARACTERISTIC_UUID} in service")
            return
        
        # Prepare and send JSON-RPC requests for different methods
        methods = ["getTemperature", "getHumidity", "getSensorReadings"]
        for method in methods:
            request_id = 1
            target_device_name = selected_name  # Use the selected device's name in the request
            json_rpc_request = {
                "jsonrpc": "2.0",
                "method": method,
                "params": [{"device": target_device_name}],  # Specify the target device
                "id": request_id
            }
            
            request_json = json.dumps(json_rpc_request)
            print(f"Sending JSON-RPC Request for {method}: {request_json}")
            
            await client.write_gatt_char(char, request_json.encode())
            
            # Wait for the response (Note: This example assumes the response comes immediately after the request.
            #  In a real-world scenario, consider implementing a more robust waiting mechanism.)
            await asyncio.sleep(1)  # Wait for 1 second
            
            # Read the response (Assuming it's available in the characteristic value)
            response = await client.read_gatt_char(char)
            response_json = response.decode()
            print(f"Received JSON-RPC Response for {method}: {response_json}")
            
            try:
                response_data = json.loads(response_json)
                if "result" in response_data:
                    if method == "getSensorReadings":
                        print(f"Temperature: {response_data['result']['temperature']}°C, Humidity: {response_data['result']['humidity']}%")
                    elif method == "getTemperature":
                        print(f"Temperature: {response_data['result']['temperature']}°C")
                    elif method == "getHumidity":
                        print(f"Humidity: {response_data['result']['humidity']}%")
                else:
                    print("No sensor readings in response.")
            except json.JSONDecodeError as e:
                print(f"Failed to parse JSON response: {e}")

asyncio.run(main())
