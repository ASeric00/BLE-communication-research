# Sending short string to arduino and waiting to retrieve same info from it

import asyncio
from bleak import BleakClient

BLE_ADDRESS = "87E73EC6-C46E-FE0B-B68F-AC3EB0090440"  # Replace with your Arduino's MAC address
SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
CHARACTERISTIC_UUID = "abcdef01-1234-5678-1234-56789abcdef0"

async def main():
    async with BleakClient(BLE_ADDRESS) as client:
        print("Connected:", client.is_connected)

        # Send data to the peripheral
        data = "Hello, Arduino!"
        await client.write_gatt_char(CHARACTERISTIC_UUID, data.encode())
        print(f"Sent: {data}")

        # Read response from the peripheral
        response = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f"Received: {response.decode()}")

asyncio.run(main())
