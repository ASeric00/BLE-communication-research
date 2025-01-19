import asyncio
from bleak import BleakClient

BLE_ADDRESS = "EE9A5EC7-6941-29CE-DAB9-35499F9C56A6"  # Replace with your peripheral's MAC address
SERVICE_UUID = "00001101-0000-1000-8000-00805F9B34FB"
CHARACTERISTIC_UUID = "2A37"  # Example UUID

async def main():
    async with BleakClient(BLE_ADDRESS) as client:
        print("Connected:", client.is_connected)
        data = b"Hello, Peripheral!"
        await client.write_gatt_char(CHARACTERISTIC_UUID, data)
        print("Data sent!")
        response = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print("Received:", response)

asyncio.run(main())