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
 
#include <pthread.h>

/* return codes:
 *   -10= no direct access to memory, run as root
 *   -9 = receive failed, software buffer overrun, should not happen
 *   -8 = receive failed, Hardware buffer overrun, load to hi
 *   -7 = receive failed, CRC error, probably noise
 *   -6 = sending failed, hardware timeout, probably hardware fault
 *   -5 = sending failed, lines low, probably collision
 *   -4 = sending failed, clock stretch timeout, probably collision
 *   -3 = sending failed, no response, probably clock off
 *   -2 = no ack received
 *   -1 = negative ack received, 
 *    0 = succes!
 */
 

#define	ERROR_MEM		-10
#define	ERROR_SWB_FULL	-9
#define	ERROR_HWB_FULL	-8
#define	ERROR_CRC		-7
#define	ERROR_TIMEOUT	-6
#define	ERROR_LINES		-5
#define	ERROR_CST		-4
#define	ERROR_SILENT	-3
#define	ERROR_NOACK		-2
#define	ERROR_NACK		-1
#define	ERROR_OK		0
 


#define GPIO_BASE  0x200000
#define TIMER_BASE 0x003000
#define I2C_SLAVE_BASE 0x214000
#define I2C_MASTER_BASE 0x804000

#define SDA1IN (*gpioin & (1 << 2))    // SDA1 = GPIO 2
#define SCL1IN (*gpioin & (1 << 3))    // SCL1 = GPIO 3


// receive buffer length, longest package is program 51,
// debug can be modified in the future to max length of 255
#define RECEIVE_BUFFER_LENGTH 256

// enable debug pins
#ifdef debug
#define WAIT_FOR_FREE_BUS_PIN_HI *gpioset = (1 << 17)  // GPIO 17
#define WAIT_FOR_FREE_BUS_PIN_LO *gpioclr = (1 << 17)
#define RECEIVE_THREAD_RUNNING_PIN_HI *gpioset = (1 << 27)  // GPIO 27
#define RECEIVE_THREAD_RUNNING_PIN_LO *gpioclr = (1 << 27)
#define ERROR_PIN_HI *gpioset = (1 << 22)  // GPIO 22
#define ERROR_PIN_LO *gpioclr = (1 << 22)
#endif

// pointers to BCM2708/9 registers
volatile unsigned *gpio, *gpioset, *gpioclr, *gpioin;
volatile unsigned *i2cSlave, *i2cSlaveRSR, *i2cSlaveSLV, *i2cSlaveCR, *i2cSlaveFR;
volatile unsigned *i2cMaster, *i2cMasterS, *i2cMasterDLEN, *i2cMasterA, *i2cMasterFIFO, *i2cMasterDiv;
long long int *timer;

// variables for debug stats
#ifdef debug
typedef struct {
	int displaySF;
	int displayAF;
	int endDisplaySF;
	int endDisplayAF;
	int changeStateSF;
	int changeStateAF;
	int setCCSF;
	int setCCAF;
	int setNRunSF;
	int setNRunAF;

	int rxTimeout;
	int rxWrongAdr;
	int rxBufferFull;
	int rxSizeMismatch;
	int rxCRCFault;
	int rxMaxBuf;

	int sendTotal;

} debug_t;
debug_t bug;
//int wakes, setccs, resets, clears, clears2, hellos, hellos2, totals, overflows, maxs;
#endif

#define DGTRX_BUTTON_BUFFER_SIZE 16
typedef struct {
	char on;
	char ack[2];
	char hello;
	char buttonPres[DGTRX_BUTTON_BUFFER_SIZE];
	char buttonTime[DGTRX_BUTTON_BUFFER_SIZE];
	int buttonStart;
	int buttonEnd;
	long long int buttonRepeatTime;
	char buttonCount;
	char buttonState;
	char lastButtonState;
	char time[6];
	int error;
} dgtReceive_t;

dgtReceive_t dgtRx;

pthread_t receiveThread;
pthread_mutex_t receiveMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t receiveCond = PTHREAD_COND_INITIALIZER;

char startMode = 0;

// I2C message descriptors
char ping[] = { 80, 32, 5, 13, 70 };
char centralControll[] = { 16, 32, 5, 15, 72 };
char mode25[] = { 16, 32, 6, 11, 57, 185 };
char endDisplay[] = { 16, 32, 5, 7, 112 };
char noAutoMessage[] = { 16, 32, 6, 3, 209, 135 };
char display[] = { 16, 32, 21, 6, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 255, 0, 3, 0, 0, 0 };
char setnrun[] = { 16, 32, 12, 10, 0, 1, 0, 0, 1, 0, 1, 0 };

const char* packetDescriptor[] = { "Ack", "Hello", "Debug", "Time", "Button", "Display", "End Display", "Current Program", "Program", "Set And Run", "Change State", "Send Hello", "Ping", "Time Correlation", "Set Central Control", "Release Central Control", "Trigger Boot Loader" };

// pre-calculated CRC ATM-8 (x^8 + x^2 + x^1 + x^0)
const char crc_table[256] = {
	0x00,
	0x07,
	0x0E,
	0x09,
	0x1C,
	0x1B,
	0x12,
	0x15,
	0x38,
	0x3F,
	0x36,
	0x31,
	0x24,
	0x23,
	0x2A,
	0x2D,
	0x70,
	0x77,
	0x7E,
	0x79,
	0x6C,
	0x6B,
	0x62,
	0x65,
	0x48,
	0x4F,
	0x46,
	0x41,
	0x54,
	0x53,
	0x5A,
	0x5D,
	0xE0,
	0xE7,
	0xEE,
	0xE9,
	0xFC,
	0xFB,
	0xF2,
	0xF5,
	0xD8,
	0xDF,
	0xD6,
	0xD1,
	0xC4,
	0xC3,
	0xCA,
	0xCD,
	0x90,
	0x97,
	0x9E,
	0x99,
	0x8C,
	0x8B,
	0x82,
	0x85,
	0xA8,
	0xAF,
	0xA6,
	0xA1,
	0xB4,
	0xB3,
	0xBA,
	0xBD,
	0xC7,
	0xC0,
	0xC9,
	0xCE,
	0xDB,
	0xDC,
	0xD5,
	0xD2,
	0xFF,
	0xF8,
	0xF1,
	0xF6,
	0xE3,
	0xE4,
	0xED,
	0xEA,
	0xB7,
	0xB0,
	0xB9,
	0xBE,
	0xAB,
	0xAC,
	0xA5,
	0xA2,
	0x8F,
	0x88,
	0x81,
	0x86,
	0x93,
	0x94,
	0x9D,
	0x9A,
	0x27,
	0x20,
	0x29,
	0x2E,
	0x3B,
	0x3C,
	0x35,
	0x32,
	0x1F,
	0x18,
	0x11,
	0x16,
	0x03,
	0x04,
	0x0D,
	0x0A,
	0x57,
	0x50,
	0x59,
	0x5E,
	0x4B,
	0x4C,
	0x45,
	0x42,
	0x6F,
	0x68,
	0x61,
	0x66,
	0x73,
	0x74,
	0x7D,
	0x7A,
	0x89,
	0x8E,
	0x87,
	0x80,
	0x95,
	0x92,
	0x9B,
	0x9C,
	0xB1,
	0xB6,
	0xBF,
	0xB8,
	0xAD,
	0xAA,
	0xA3,
	0xA4,
	0xF9,
	0xFE,
	0xF7,
	0xF0,
	0xE5,
	0xE2,
	0xEB,
	0xEC,
	0xC1,
	0xC6,
	0xCF,
	0xC8,
	0xDD,
	0xDA,
	0xD3,
	0xD4,
	0x69,
	0x6E,
	0x67,
	0x60,
	0x75,
	0x72,
	0x7B,
	0x7C,
	0x51,
	0x56,
	0x5F,
	0x58,
	0x4D,
	0x4A,
	0x43,
	0x44,
	0x19,
	0x1E,
	0x17,
	0x10,
	0x05,
	0x02,
	0x0B,
	0x0C,
	0x21,
	0x26,
	0x2F,
	0x28,
	0x3D,
	0x3A,
	0x33,
	0x34,
	0x4E,
	0x49,
	0x40,
	0x47,
	0x52,
	0x55,
	0x5C,
	0x5B,
	0x76,
	0x71,
	0x78,
	0x7F,
	0x6A,
	0x6D,
	0x64,
	0x63,
	0x3E,
	0x39,
	0x30,
	0x37,
	0x22,
	0x25,
	0x2C,
	0x2B,
	0x06,
	0x01,
	0x08,
	0x0F,
	0x1A,
	0x1D,
	0x14,
	0x13,
	0xAE,
	0xA9,
	0xA0,
	0xA7,
	0xB2,
	0xB5,
	0xBC,
	0xBB,
	0x96,
	0x91,
	0x98,
	0x9F,
	0x8A,
	0x8D,
	0x84,
	0x83,
	0xDE,
	0xD9,
	0xD0,
	0xD7,
	0xC2,
	0xC5,
	0xCC,
	0xCB,
	0xE6,
	0xE1,
	0xE8,
	0xEF,
	0xFA,
	0xFD,
	0xF4,
	0xF3
};


//*** helping functions ***//

/* find out wich pi
	returns:
	0 = error
	1 = Pi b+
	2 = Pi 2 */
int checkPiModel();

/* calculate checksum and put it in the last byte
	*buffer = pointer to buffer */
char crc_calc(char *buffer);

/* print hex values
	bytes = array of bytes
	length is number of bytes to print */
void hexPrint(char bytes[], int length);


//*** Low level I2C communication ***//

/* configure IO pins and I2C Master and Slave
	*/
void i2cReset();

/* get message from I2C receive buffer
	m[] = message buffer of 256 bytes
	timeOut = time to wait for packet in us (0=dont wait)
	returns:
	-6 = CRC Error
	-5 = I2C buffer overrun, at least 16 bytes received succesfully. rest is missing.
	-4 = our buffer overrun (should not happen)
	-3 = timeout
	-2 = I2C Error
	>0 = packet length*/
int i2cReceive(char m[]);

/* send message using I2CMaster
	 message[] = the message to send
	 returns:
	 -7 = message not Acked, probably clock off
	 -3 = message not Acked, probably collision
	 -2 = I2C Error
	 0 = Succes */
int i2cSend(char message[], char ackAdr);



//*** dgt3000 commands ***//

/* send a wake command to the dgt3000
	returns:
	-3 = wake ack error
	-1 = no hello message received
	0 = succes */
int dgt3000Wake();

/* send set central controll command to dgt3000
	returns:
	-3 = sending failed, clock off (or collision)
	-2 = sending failed, I2C error
	-1 = no (positive)ack received, not in CC
	0 = succes */
int dgt3000SetCC();

/* send set mode 25 to dgt3000
	returns:
	-3 = sending failed, clock off (or collision)
	-2 = sending failed, I2C error
	-1 = no (positive)ack received, not in CC
	0 = succes */
int dgt3000Mode25();

/* send set and run command to dgt3000
     */
int dgt3000SetNRun(char srm[]);

/* send end display to dgt3000 to clear te display
	returns:
	-3 = sending failed, clock off (or collision)
	-2 = sending failed, I2C error
	-1 = no (positive)ack received, not in CC
	0 = succes */
int dgt3000EndDisplay();

/* send set display command to dgt3000
	dm = display message to send
	returns:
	-3 = sending failed, clock off (or collision)
	-2 = sending failed, I2C error
	-1 = no (positive)ack received, not in CC
	0 = succes */
int dgt3000Display(char dm[]);

/* check for messages from dgt3000
	returns:
	0 = nothing is received
	1 = something is received
	2 = off button message is received */
void *dgt3000Receive(void *);

/* wait for an Ack message
	adr = adress to listen for ack
	cmd = command to ack
	timeOut = time to wait for ack
	returns:
	-3 = no Ack
	0 = Ack */
int dgt3000GetAck(char adr, char cmd, long long int timeOut);

