# simple program to learn how json rpc is sent over ble
# writing json rpc to write characteristic on iphone
# we used app LigthBlue to emulate virtual peripheral device

import asyncio
from bleak import BleakClient

# Replace with your phone's BLE address and characteristic UUID
BLE_ADDRESS = "EE9A5EC7-6941-29CE-DAB9-35499F9C56A6" #address of iphone
WRITE_CHARACTERISTIC_UUID = "00001101-0000-1000-8000-00805F9B34FB"

# JSON-RPC example
json_rpc_request = {
    "jsonrpc": "2.0",
    "method": "getStatus",
    "params": {},
    "id": 1
}

async def main():
    async with BleakClient(BLE_ADDRESS) as client:
        print("Connected:", client.is_connected)

        # Send JSON-RPC request
        request_data = str(json_rpc_request).encode()
        await client.write_gatt_char(WRITE_CHARACTERISTIC_UUID, request_data)
        print("Sent JSON-RPC request.")

asyncio.run(main())
