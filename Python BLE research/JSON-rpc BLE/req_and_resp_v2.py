import asyncio
from bleak import BleakClient

# Replace with your BLE device's address and characteristic UUID
BLE_ADDRESS = "874D6A7E-7B30-DB59-48B2-094E4E1B72B5"
WRITE_CHARACTERISTIC_UUID = "00001101-0000-1000-8000-00805F9B34FB"
NOTIFY_CHARACTERISTIC_UUID = "00001101-0000-1000-8000-00805F9B68CF"

# JSON-RPC request
json_rpc_request = {
    "jsonrpc": "2.0",
    "method": "getStatus",
    "params": {},
    "id": 1
}

def notification_handler(sender, data):
    print("Received notification:", data.decode())

async def main():
    async with BleakClient(BLE_ADDRESS) as client:
        print("Connected:", client.is_connected)

        # Enable notifications
        await client.start_notify(NOTIFY_CHARACTERISTIC_UUID, notification_handler)

        # Send JSON-RPC request
        request_data = str(json_rpc_request).encode()
        await client.write_gatt_char(WRITE_CHARACTERISTIC_UUID, request_data)
        print("Sent JSON-RPC request.")

        # Wait to receive notifications
        await asyncio.sleep(5)  # Allow time to receive response

        # Stop notifications
        await client.stop_notify(NOTIFY_CHARACTERISTIC_UUID)

asyncio.run(main())