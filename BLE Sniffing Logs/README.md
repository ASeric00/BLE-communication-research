BLE LOGs:
- arduino_passkey - razmjena jednostavnog kljuca za uspustavu ble komunikacije
- arduino_temp_to_central - slanje info o temp i vlaznosti s arduina na laptop
- BLE_arduino - jednostavan program, salje se string Hello, Arduino koji arduino po primitku vraca laptopu kao potvrdu
- Iphone_sound_airtag_airpods - slanje zahtjeva za pustanjem zvuka na periferne zvucnike airpods pro slusalica i airtaga
- Iphone_sound_airtag - slanje zahtjeva za pronalazenje airtag-a, na paketu br 772 vidljivo slanje podataka u segmentima (zbog ogranicenja MTU-a airtaga)
- Iphone_airtag_watch - paket br 343 slanje BT imena mobitela, paket br 356 primanje imena sata; dosta btt i l2cap paketa (vjerojatno se pokusavalo vidjeti heart rate service, ali sve enkriptirano)
