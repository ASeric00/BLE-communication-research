# Python script for connecting central device to arduino 
# so that it could retreive sensors readings 

import asyncio
from bleak import BleakClient
import struct

# Replace with your Arduino's BLE address
BLE_ADDRESS = "87E73EC6-C46E-FE0B-B68F-AC3EB0090440"

# Replace with the UUIDs used in your Arduino code
TEMPERATURE_CHARACTERISTIC_UUID = "2A6E"  # Temperature characteristic UUID
HUMIDITY_CHARACTERISTIC_UUID = "2A6F"    # Humidity characteristic UUID

async def main():
    async with BleakClient(BLE_ADDRESS) as client:
        print("Connected:", client.is_connected)

        # Read temperature data
        temperature_data = await client.read_gatt_char(TEMPERATURE_CHARACTERISTIC_UUID)
        temperature = struct.unpack('<f', temperature_data)[0]  # Decode as little-endian float
        print(f"Temperature: {temperature:.2f} °C")

        # Read humidity data
        humidity_data = await client.read_gatt_char(HUMIDITY_CHARACTERISTIC_UUID)
        humidity = struct.unpack('<f', humidity_data)[0]  # Decode as little-endian float
        print(f"Humidity: {humidity:.2f} %")

asyncio.run(main())
