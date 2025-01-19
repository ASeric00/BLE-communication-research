# script to send a request for sensors readings from arduino
# and to read a response (json rpc)

import asyncio
from bleak import BleakClient
import json

BLE_ADDRESS = "87E73EC6-C46E-FE0B-B68F-AC3EB0090440" #ble address of 2nd arduino nano
# BLE_ADDRESS = "874D6A7E-7B30-DB59-48B2-094E4E1B72B5" #ble address of 1st arduino nano
JSON_RPC_CHARACTERISTIC_UUID = "87654321-4321-4321-4321-abcdefabcdef"

json_rpc_request = {
    "jsonrpc": "2.0",
    "id": 1,
    "method": "getStatus",
    "params": {},
}

async def main():
    try:
        async with BleakClient(BLE_ADDRESS) as client:
            if client.is_connected:
                print("Connected to BLE device.")
                
                # Send JSON-RPC request
                request_data = json.dumps(json_rpc_request).encode()
                await client.write_gatt_char(JSON_RPC_CHARACTERISTIC_UUID, request_data)
                print("Sent JSON-RPC request.")
                
                # Receive response
                response_data = await client.read_gatt_char(JSON_RPC_CHARACTERISTIC_UUID)
                response = json.loads(response_data.decode())

                # Check for 'result' field in response
                if "result" in response:
                    print(f"Temperature: {response["result"]["temperature"]} °C")
                    print(f"Humidity: {response["result"]["humidity"]} %")
                else:
                    print("Unexpected response format:", response)

            else:
                print("Failed to connect to BLE device.")
    except Exception as e:
        print("An error occurred:", e)

asyncio.run(main())
