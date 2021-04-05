# AtomicChessOS

# ATC CONTROLLER

## COMMANDLINE ARGUMENTS




### SELECT HARDWARE REVISION HWREV

Normally the controller software detects its hardware revision automatically. In order to select a hardware revision manually the following commandline arguments can be choosen:


* `./atc_controller -writeconfig -hwvirtual` - selects the virtual hardware which enables testing without hardware
* `./atc_controller -writeconfig -hwdk` - selects the development kit DK hardware (RPI SPI interface)
* `./atc_controller -writeconfig -hwprod1` - selects the production hardware version 1 (Marlin XY)
* `./atc_controller -writeconfig -hwprod2` - selects the production hardware version 2 (Marlin CoreXY)

This option always overrides the existing config file!

### SET AUTOMATIC PLAY

* `./atc_controller -autoplayer` - enables the automatic player mode - table will make random moves


### CREATE NEW CONFIG FILE

* `./atc_controller -writeconfig` - overrides existing config file with defaults


### SKIP NFC read

* `./atc_controller -skipplacementdialog` - skips the initial chess board NFC scan and load the default fen from the config file
