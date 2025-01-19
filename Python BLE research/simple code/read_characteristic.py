# Read the current state of a characteristic on a BLE device

import asyncio
from bleak import BleakClient

async def main():
    ble_address = "EE9A5EC7-6941-29CE-DAB9-35499F9C56A6"
    characteristic_uuid = "2A37"

    async with BleakClient(ble_address) as client:
        data = await client.read_gatt_char(characteristic_uuid)
        print(data)

asyncio.run(main())