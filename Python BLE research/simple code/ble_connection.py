import asyncio
from bleak import BleakClient

async def main():
    ble_address = "F16048BD-AD62-EA4D-D309-4A6A6C05E52E"

    async with BleakClient(ble_address) as client:
        # we’ll do the read/write operations here
        print("Connected to BLE device")
        print(client.is_connected)        

asyncio.run(main())