# Writing json rpc to write characteristic on iphone 
# and reading from read characteistic from iphone
# We used app LigthBlue to emulate virtual peripheral device

import asyncio
from bleak import BleakClient

# Replace with your phone's BLE address and characteristic UUID
# BLE_ADDRESS = "EE9A5EC7-6941-29CE-DAB9-35499F9C56A6" #ble address of iphone
# BLE_ADDRESS = "87E73EC6-C46E-FE0B-B68F-AC3EB0090440" #ble address of 1st arduino nano
BLE_ADDRESS = "874D6A7E-7B30-DB59-48B2-094E4E1B72B5" #ble address of 2nd arduino nano
WRITE_CHARACTERISTIC_UUID = "00001101-0000-1000-8000-00805F9B34FB"
READ_CHARACTERISTIC_UUID = "00001101-0000-1000-8000-00805F9B68CF"

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

        # Wait for the response
        response_data = await client.read_gatt_char(READ_CHARACTERISTIC_UUID)
        response = response_data.decode()
        print("Received JSON-RPC response:", response)

asyncio.run(main())
