# First example of writing to a characteristic

import asyncio
from bleak import BleakClient

async def main():
    ble_address = "3E3697D1-8609-C631-133A-60E9D8D4C232"
    characteristic_uuid = "8667556C-9A37-4C91-84ED-54EE27D90049"

    async with BleakClient(ble_address) as client:
        data = await client.read_gatt_char(characteristic_uuid)
        data[1] = 1
        await client.write_gatt_char(characteristic_uuid, data)

asyncio.run(main())