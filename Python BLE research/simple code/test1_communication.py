import asyncio
from bleak import BleakClient

# Replace with your fitness band's MAC address or UUID (you can find this in the nRF Connect app)
BLE_ADDRESS = "B7F8D345-006C-1C28-E46D-650A0DB0B6EF"  # MAC address of your fitness band
HEART_RATE_SERVICE_UUID = "0000cc00-0000-1000-8000-00805f9b34fb"  # Heart Rate Service UUID
HEART_RATE_MEASUREMENT_UUID = "0000cc03-0000-1000-8000-00805f9b34f"  # Heart Rate Measurement Characteristic UUID

async def main():
    try:
        # Connect to the fitness band (acting as peripheral)
        async with BleakClient(BLE_ADDRESS) as client:
            print(f"Connected: {client.is_connected}")

            # Get services to confirm that the Heart Rate service is available
            services = await client.get_services()
            if services is None:
                print("No services found.")
                return

            print("Services discovered:")
            for service in services:
                print(f"Service: {service.uuid}")
                for char in service.characteristics:
                    print(f"  Characteristic: {char.uuid}, properties: {char.properties}")

            # Look for the heart rate measurement characteristic
            for service in services:
                if service.uuid == HEART_RATE_SERVICE_UUID:
                    print("Heart Rate Service found.")
                    for char in service.characteristics:
                        if char.uuid == HEART_RATE_MEASUREMENT_UUID:
                            heart_rate_data = await client.read_gatt_char(HEART_RATE_MEASUREMENT_UUID)
                            print(f"Heart Rate Data: {heart_rate_data}")
                            return
            print("Heart Rate Measurement characteristic not found!")
    except Exception as e:
        print(f"Error: {e}")

asyncio.run(main())
