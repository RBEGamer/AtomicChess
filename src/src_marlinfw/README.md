# SRC_MARLINFW

This directory contains the used Marlin-Firmware for the SKR1.3 (HWREV_PROD1) and SKR1.4_TURBO (HWREV_PROD2).

The simplest way to compile and flash the firmware is with Visual Code and the Plugins

* PlatformIO
* Auto Marlin


# PRECOMPILED MARLIN FIRMWARE

Simply copy the .bin file to the SD Card of the board, or via the USB mount.


* SKR1.3 => ./SKR_V1.3_BOARD_FIRMWARE/ATC_HWREV_PROD1_MODIFIED_FIRMWARE/
* SKR1.4_TURBO => ./SKR_V1.4_BOARD_FIRMWARE/ATC_HWREV_PROD1_MODIFIED_FIRMWARE/

# MARLIN SOURCE PROJECTS

* SKR1.3 => ./SKR_V1.3_BOARD_FIRMWARE/BTT SKR V1.3/firmware/Marlin-bugfix-2.0.x
* SKR1.4_TURBO=> ./SKR_V1.4_BOARD_FIRMWARE/BTT SKR V1.4/firmware/Marlin-2.0.x-SKR-V1.4-Turbo/


# IMPORTANT CONFIGRATION SETTINGS

The main marlin configuration files are the `Configuration.h` and `Configuration_adv.h` located in the the Marlin folder.

For the ATC Project the following settings are changed in comparision with the default config.

## Configuration.h

* `664 | #define X_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.`
* `665 | #define Y_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.`

* `0750 | #define DEFAULT_AXIS_STEPS_PER_UNIT   { 80, 80, 400, 93 } // FOR HWREV_PROD2 USE { 100, 100, 400, 93 }`

* `1096 | #define INVERT_X_DIR false //INVERT ENDSTOP SIGNAL`
* `1097 | #define INVERT_Y_DIR false` 

* `1132 | #define X_BED_SIZE 700`
* `1133 | #define Y_BED_SIZE 700`

* `0615 | #define COREYZ //ON HWREV_PROD2`  

* `2359 | #define NEOPIXEL_LED //ENABLE NEXOPIXEL LED SUPPORT`
* `2365 | #define NEOPIXEL_PIXELS 60       // Number of LEDs in the strip.`

* `2412 | #define NUM_SERVOS 1 // Servo index starts with 0 for M280 command ON SKR BOARD THE SERVO PIN IS P2_00`
* `2417 | #define SERVO_DELAY { 500 } //(ms) Delay  before the next move will start`