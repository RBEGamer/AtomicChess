# HARDWARE_DEV


This folder contains all files created for the HWREV_DK version of the table.

There are no CAD Files available, but the PCB design and the software for the control electronics.

The control electronic includes the TMC5160 stepper motor drivers and a microcontroller controlling the electro magnets and ws2811 led strip over SPI.
It also connects the PN532 NFC Reader over I2C and make it controllable over the same SPI bus.

The PCB contains a 40pin header for direct attachment of a RaspberryPi. It also includes a two step power supply (18Vin->12Vmotor->5Varduino).

Some jumpers are also added to enable/disable specific funktion sections (Stepperdrivers/Coildrivers/LED,NFC)

## FOLDER STRUCTURE

* `PCB` => Contains the PCB schematic, board bom and gerber files, including PDF,DFX drawing and Autodesk Eagle project files
* `src_io_controller` => Contains the firmware for the Arduino Nano which controls the WS2811 LED, Coil-Control and NFC Read
* `documentation_iages` => Contains a photo series of the complete build process