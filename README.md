# Kobo Mini Kit

Turn your kobo mini into a complete flight instrument :-)
This project allows you to build an extension card for Kobo which runs XCSoar. The extension card includes a GPS and a pressure sensor. It acts as a sound variometer as it sends data (Vz, temperature, absolute pressure) to XCSoar. In this repo you will find:
- Eagle files to realize the electronic card
- Firmware source (and precompiled hex binaries) for the mcu
- STL parts for the Kobo Mini to receive electronic card (and a bigger battery for long flights)
- Bill of materials

## Hardware

The extension card is based on PIC24F16KA101. It also includes a GPS patch antenna chip (PA6H) and a high resolution pressure sensor [(MS5611)](https://infusionsystems.com/support/MS5611-01BA03.pdf)

## Firmware

Compiled with Microchip XC16 compiler

## Software

XCSoar
