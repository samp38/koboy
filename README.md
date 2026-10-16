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

The GPS (U5) symbol and footprint come from [Gekkio's KiCad libraries](https://github.com/Gekkio/gekkio-kicad-libs) by Joonas Javanainen, licensed under CC BY 4.0 (copy in `hardware/PCB/Vario Kobo/Button_version/kicad-koboy_button_side/libs/gekkio/`).

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
- Button version: the MAS6240 EN1/EN2 pins are wired to RA2/RA3, the reverse of `firmware/audio.c` (`EN1 = _RA3`, `EN2 = _RA2`). This dates back to the original Eagle design. As a result the low and medium volume levels are swapped (low gives 2x, medium gives 1x); high and off are unaffected. The taptap version matches the firmware. To be fixed in the firmware with per-version pin definitions.
- R7 (1Ω) in series with the piezo is nearly a jumper: the MAS6240 datasheet suggests an optional 1kΩ to 2kΩ output protection resistor.
- Taptap version: the PIC is programmed through the USB-A plug X1 with a custom pinout (D+ → MCLR, D− → PGC, 5V → PGD, which is also the GPS RX). Plugging the board into a real USB port would put 5V on PGD and the GPS RX.
- Taptap version: the PA6H GND pins 3, 12 and 19 are not connected (Eagle library), only pin 8 is. To be fixed with the Gekkio GPS footprint, as on the button version.

### PCB
- Button version, DRC: 2 silkscreen warnings, no errors: the BZ1 (buzzer) footprint outline from the Eagle import runs over its pads. The fab clips it.
- Taptap version, DRC: 12 errors, all pad-to-pad gaps of 0.195mm (rule 0.2mm) inside the Eagle MMA8452 footprint (0.305mm pads at 0.5mm pitch); to be fixed with a KiCad standard footprint. Silkscreen warnings from the Eagle PA6H and MMA8452 outlines and undersized texts.

### Firmware
- Auto-shutdown after inactivity is not implemented: `vario_shutdown_tick()` counts down but the `shutdown()` call is commented out (`firmware/vario.c`).

### Repository / documentation
- The hardware moved from Eagle to KiCad, with the same structure for both versions (`main` sheet plus a `Power supply` sub-sheet):
  - button version: `hardware/PCB/Vario Kobo/Button_version/kicad-koboy_button_side/main.kicad_pro`;
  - taptap version: `hardware/PCB/Vario Kobo/Taptap_version/kicad-koboy_taptap_side/main.kicad_pro` (migration in progress: references, silkscreen, GPS footprint and pin types still to do).

  The Eagle files are kept for history only; "Vario Kobo TOP" is to be removed.
- The README still says "Eagle files" in the description and needs updating for KiCad.
