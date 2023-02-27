/* functions to communicate to a DGT3000 using I2C
 * version 0.8
 *
 * Copyright (C) 2015 DGT
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


/* Return codes for all funcitons are at the bottom of this doccument.
 * All functions try three times, the error is the reason why the third
 * try failed.
 */


/* Get direct access to BCM2708/9 chip.
 *   Run this first and only once (or again after a dgtpicom_stop())
 */
int dgtpicom_init(void);

/* Configure the dgt3000: turn it on, set central control and set
 * mode 25. If neccesary reset the I2C hardware.
 *   Run this before any command and if commands fail
 */
int dgtpicom_configure();

/* Send set and run command to the dgt3000.
 *   lr/rr = left/right run mode, 0=stop, 1=count down, 2=count up
 *   lh/rh = left/right hours
 *   lm/rm = left/right minutes
 *   ls/rs = left/right seconds
 */
int dgtpicom_set_and_run(char lr,
	char lh,
	char lm,
	char ls,
	char rr,
	char rh,
	char rm,
	char rs);

/* Send set and run command to the dgt3000 with current clock values.
 *   lr/rr = left/right run mode, 0=stop, 1=count down, 2=count up
 */
int dgtpicom_run(char lr, char rr);				

/* Set a text message on the dgt3000.
 *   text = message to display
 *   beep = beep length (/62.5ms) max 48 (3s)
 *   ld/rd = left/right icons and dots:
 *     1=flag,
 * 	   2=white king,
 * 	   4=black king,
 * 	   8=colon,
 * 	   16=dot,
 * 	   32=extra dot (left only)
 */
int dgtpicom_set_text(char text[], char beep, char ld, char rd);

/* End a text message on the dgt3000 and return to clock mode.
 */
int dgtpicom_end_text();

/* Put the last received time message in time[].
 *   time[] = 6 byte time descriptor
 */
void dgtpicom_get_time(char time[]);

/* Get a button message from the buffer and put it in buttons and time
 * returns number of messages in the buffer or an error code if a
 * receive error has occurd since you last check.
 *   buttons = buttons pressed:
 *     binary:
 *       0x01 < back
 *       0x02 - minus
 *       0x04 p play/pause
 *       0x08 + plus
 *       0x10 > forward
 *     special:
 *       0x20 o off
 *       0xa0 o on
 *       0x40 \ Lever changed, right side down
 *       0xc0 / Lever changed, left side down
 *   time = time the buttons were pressed in 100ms
 */
int dgtpicom_get_button_message(char *buttons, char *time);

/* Return current button state.
 *   returns:
 *     binary:
 *       0x01 < back
 *       0x02 - minus
 *       0x04 p play/pause
 *       0x08 + plus
 *       0x10 > forward
 *       0x20 o on/off button
 *       0x40 \ Lever position, (right side down = 0x40)
 */
int dgtpicom_get_button_state();

/* Turn off the dgt3000.
 *   returnMode = timing method the clock will start in when turned on
 */
int dgtpicom_off(char returnMode);

/* Disable the I2C hardware.
 *   run this before you end the program
 */
void dgtpicom_stop();


/* return codes:
 *   -10= no direct access to memory, run as root
 *   -9 = receive failed, software buffer overrun, should not happen
 *   -8 = receive failed, packet to small, hardware buffer overrun
 *   -7 = receive failed, CRC error, probably noise
 *   -6 = sending failed, hardware timeout, probably hardware fault
 *   -5 = sending failed, lines low, probably collision
 *   -4 = sending failed, clock stretch timeout, probably collision
 *   -3 = sending failed, no response, probably clock off
 *   -2 = no ack received
 *   -1 = negative ack received, 
 *    0 = succes!
 */
