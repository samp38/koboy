# Kobo Mini to Flight Computer Kit

## Description
Turn your kobo mini into a complete flight instrument :-)
This project allows you to build an extension card for Kobo which runs XCSoar. The extension card includes a GPS and a pressure sensor. It acts as a sound variometer while it sends data (GPS NMEA data, Vz, temperature, absolute pressure) to XCSoar. In this repo you will find:
- Eagle files and guide to realize the electronic card
- Firmware source (and precompiled hex binaries) for the mcu
- STL parts for the Kobo Mini to receive electronic card (and a bigger battery for long flights)
- XCSoar software for Kobo

### How it works
The GPS chip is standalone. As long as it's powered, it runs and sends NMEA data through its TX output at a given (user definable) frequency. The microntroller manages the pressure sensing and computing: pressure is measured, numerically filtered and derivated at 50Hz to provide Vz. To work with the GPS chip, the microcontroller detects the beginning of the PA6H TX communication and waits the time needed to send variometer data between GPS data bursts. It can seem dubious but it works very well for years in my case. The variometer data sent by the microcontroller to XCSoar mimics a Digifly Leonardo. So you just have to select Digifly Leonardo in XCSoar to have the variometer data read and understood by XCSoar. All the steps of the XCSoar configuration is described in the __software__ part.


### Two versions for two interaction options
To control the sound vario volume and turn the extension card on/off (indeed, you might want to use your Kobo as a reader), there are two possibilities and thus two versions of the PCB:
- "button version": meaningful. Short press to loop in sound levels and long press to turn off.
- "tap-tap version": the button is replaced with a accelerometer which detects the tap. Single tap to loop in sound levels and double tap to turn on/off.

## Hardware

The extension card is based on a [PIC24F16KA101](http://ww1.microchip.com/downloads/en/DeviceDoc/39927c.pdf) microcontroller, a GPS patch antenna chip [(PA6H)](https://cdn-shop.adafruit.com/datasheets/GlobalTop-FGPMMOPA6H-Datasheet-V0A.pdf) and a high resolution pressure sensor [(MS5611)](https://infusionsystems.com/support/MS5611-01BA03.pdf).

## Firmware

Compiled with Microchip XC16 compiler

## Software

XCSoar

## Open issues

### Schematic
- No VDD pin on the programming connector: the PIC must be powered separately while flashing.
- MCLR: the series resistor R2 (100Ω to 470Ω) from the datasheet's reference circuit is missing (C6 100nF sits directly on the MCLR net). It limits the current dumped by the capacitor if the pin breaks down under ESD/EOS (PIC24F16KA102 datasheet, §2.3).
- No local decoupling capacitor on the MAS6240 VIN.
- KOBO_RX is shared between the PIC and the GPS TX (through R5).
- ERC: 11 warnings, no errors. The Eagle-imported symbols type their power pins (GND, VIN) as bidirectional, which ERC flags next to the PWR_FLAG.

### PCB
- DRC: 74 silkscreen warnings, no errors. The reference texts from the Eagle import are 0.31 mm high (below the 0.8 mm minimum, unreadable once printed), and some footprint outlines (PA6H, buzzer) overlap their pads.

### Firmware
- Auto-shutdown after inactivity is not implemented: `vario_shutdown_tick()` counts down but the `shutdown()` call is commented out (`firmware/vario.c`).

### Repository / documentation
- The hardware moved from Eagle to KiCad (`hardware/PCB/Vario Kobo/Button_version/kicad-koboy_button_side/main.kicad_pro`: `main` sheet plus a `Power supply` sub-sheet). The Eagle files are kept for history only; "Vario Kobo TOP" is to be removed.
- The README still says "Eagle files" in the description and needs updating for KiCad.
