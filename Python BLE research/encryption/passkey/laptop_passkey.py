# simple example of achieving secure ble communication with central device
# using simple password to establish communication - PASSKEY METHOD

import asyncio
from bleak import BleakClient
import struct

# BLE Device and Characteristic UUIDs
BLE_ADDRESS = "87E73EC6-C46E-FE0B-B68F-AC3EB0090440"
TEMPERATURE_CHARACTERISTIC_UUID = "2A6E"
HUMIDITY_CHARACTERISTIC_UUID = "2A6F"
PASSKEY_CHARACTERISTIC_UUID = "12345678-1234-5678-1234-56789abcdef0"

async def main():
    async with BleakClient(BLE_ADDRESS) as client:
        print("Connected:", client.is_connected)

        # Step 1: Read the passkey
        passkey = await client.read_gatt_char(PASSKEY_CHARACTERISTIC_UUID)
        print(f"Passkey received: {passkey.decode()}")

        # Step 2: Write the passkey back for verification
        await client.write_gatt_char(PASSKEY_CHARACTERISTIC_UUID, passkey)
        print("Passkey sent for verification.")

        # Step 3: If successful, read sensor data
        temperature_data = await client.read_gatt_char(TEMPERATURE_CHARACTERISTIC_UUID)
        temperature = struct.unpack('<f', temperature_data)[0]
        print(f"Temperature: {temperature:.2f} °C")

        humidity_data = await client.read_gatt_char(HUMIDITY_CHARACTERISTIC_UUID)
        humidity = struct.unpack('<f', humidity_data)[0]
        print(f"Humidity: {humidity:.2f} %")

asyncio.run(main())
