# ZMK-Config fuer BLEAdapter

Dieses Verzeichnis ist das vorbereitete ZMK-User-Config-Repo fuer das 8-Taster-Switch-Interface auf Basis des Seeed Studio XIAO nRF52840.

Schritt 0 ist eingetragen: Buchsen 1..8 liegen auf D0..D7, die WS2812-Datenleitung liegt auf D8.

## Naechster Schritt

1. Repo nach GitHub schieben.
2. GitHub Actions Build ausfuehren.
3. UF2-Artefakt per Bootloader-Laufwerk auf den XIAO ziehen.
4. Bei Pairing-Problemen zuerst die `settings_reset`-UF2 flashen, danach wieder die normale Firmware.

## Geplantes MVP

- Board: `seeeduino_xiao_ble` mit gepinntem ZMK `v0.3.0`
- Shield: `bleadapter_8switch`
- Kscan: `zmk,kscan-gpio-direct`
- Taster: aktiv LOW mit internem Pull-Up
- BLE-Name: `BLEAdapter8`
- Keycodes: Space, Return, Tab, Escape, Left, Right, Up, Down
- Buchse 8: kurz = Pfeil runter, halten = Wartungsebene.
- Pairing neu starten: Buchse 8 halten, Buchse 1 druecken. Danach `BLEAdapter8` am Handy neu suchen.
- BLE-Profil wechseln: Buchse 8 halten, Buchse 2 = naechstes Profil, Buchse 3 = vorheriges Profil.
- LED manuell: Buchse 8 halten, Buchse 4 = blau, Buchse 5 = gruen, Buchse 6 = aus.
- Bootloader: Buchse 8 halten, Buchse 7 druecken.
- RGB: konfiguriert, aber beim Start aus, damit der Pairing-Test nicht durch Effekte irritiert.
- Ziel: BLE-Tastatur fuer iOS Switch Control und Android Switch Access

## Shield-Dateien

Das eigene Shield liegt unter `config/boards/shields/bleadapter_8switch/`, damit der GitHub-User-Config-Build es beim Aufruf mit `-DZMK_CONFIG=config` findet.
