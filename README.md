# ZMK-Config fuer BLEAdapter

Dieses Verzeichnis ist das vorbereitete ZMK-User-Config-Repo fuer das 8-Taster-Switch-Interface auf Basis des Seeed Studio XIAO nRF52840.

Schritt 0 ist eingetragen: Buchsen 1..8 liegen auf D0..D7, die WS2812-Datenleitung liegt auf D8.

## Naechster Schritt

1. Repo nach GitHub schieben.
2. GitHub Actions Build ausfuehren.
3. UF2-Artefakt per Bootloader-Laufwerk auf den XIAO ziehen.

## Geplantes MVP

- Board: `xiao_ble//zmk`
- Shield: `bleadapter_8switch`
- Kscan: `zmk,kscan-gpio-direct`
- Taster: aktiv LOW mit internem Pull-Up
- Keycodes: Space, Return, Tab, Escape, Left, Right, Up, Down
- Ziel: BLE-Tastatur fuer iOS Switch Control und Android Switch Access
