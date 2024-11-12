# CopyNFC
Copies NFC Tag to a UID-Writable Tag

## Hardware Checklist
![IMG_2891](https://github.com/user-attachments/assets/2d2da27f-95de-4562-a5e4-f78bd3591e3e)
- Arduino MEGA 2560
- Breadboard
- 9V Battery
- RFID-RC522 Module
- Button
- Male-Female Jumper Wires (7x)
- Male-Male Jumper Wires (2x)

## Arduino to RC522 Module Pinout (https://www.instructables.com/Interfacing-RFID-RC522-With-Arduino-MEGA-a-Simple-/)
RC522 MODULE    Uno/Nano     MEGA
SDA             D10          D9
SCK             D13          D52
MOSI            D11          D51
MISO            D12          D50
IRQ             N/A          N/A
GND             GND          GND
RST             D9           D8
3.3V            3.3V         3.3V
