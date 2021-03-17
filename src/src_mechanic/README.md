# SRC_MECHANIC

This folder contains all mechanical drawings (CAD, SVG, STL files). During development three hardware revisions were developed.

## USED TOOLS

* FDM 3D Printer (Ender 3 Pro, Ender 2)
* Soldering Iron (Weller PU81)
* Hex Keys / Allen Wrenchs
* Hotglue Gun


* [OPTIONAL] SLA 3D Printer (Elegoo Mars 2 Pro) for custom Chess Figures 
* [OPTIONAL] Crosscut Saw (Bosch GCM8) for Item cutting
* [OPTIONAL] Lasercutter (K40E) for acryl chess board markings

## HARDWARE REVISIONS

* HARDWARE_PROD_V2 => Contains the final version with CoreXY configuration. For this a control board (SKR) with Marlin needed. Firmware in `src_marlinfw`. 
* HARDWARE_PROD_V1 => Contains a all CAD files for the first generation with standard xy configuration; this version was used on the fh aachen hit 06.02.2021s.

* HARDWARE DEV => Contains the IKEA Lack Table Version of the chess table using self developed hardware and pcbs and a two coil system.
* HARDWARE_MOCKUP => Contains the first mockup and concept testing version; only images.

## NOTES

The software `atc_controller` and `atc_qtui` is compatible with all versions of the hardware. The only difference is the configuration file `atccontrollerconfig.ini` which need modification to the new board size and controller type (SPI or Marlin).
