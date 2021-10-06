#include "inipp.h"
#include "httplib.h"
#include "udp_discovery_peer.hpp"
#include "udp_discovery_ip_port.hpp"
#include "udp_discovery_protocol.hpp"
#include "udp_discovery_peer_parameters.hpp"
#define EXPECTED_DISCOVERY_USER_DATA "ATCTABLE"
#define OWN_DISCOVERY_USER_DATA "ATCCLOCK"

#define debug
#define DISPLAY_CHARS 11
#define MAIN_LOOP_SPPED 100
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>

#include "dgtpicom.h"
#include "dgtpicom_dgt3000.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

int ww;
const int kPort = 12021;
const uint64_t kApplicationId = 7681412;
const unsigned int kMulticastAddress = (224 << 24) + (0 << 16) + (0 << 8) + 123;  // 224.0.0.123


// while loop
void *wl(void *x) {
	int e;
	int i = 0;
	while (1)
		if (ww) {
			e = dgtpicom_configure();
			if (e < 0)
				printf("%d: Configure failed!\n", i);
			e = dgtpicom_set_text("Goeindag", 0, 0, 0);
			if (e < 0)
				printf("%d: Display failed!\n", i);
			e = dgtpicom_set_and_run(1, 0, 10, 0, 2, 0, 10, 0);
			if (e < 0)
				printf("%d: SetNRun failed!\n", i);
			i++;
#ifdef debug
			bug.sendTotal++;
#endif
		}
		else {
			usleep(10000);
		}
	return 0;
}





void scrol_text(std::string _msg, bool _beep, unsigned int _scrol_speed = 300)
{
		
	if (_msg.size() <= DISPLAY_CHARS)
	{
		char msg[_msg.size()];
		strncpy(msg, _msg.c_str(), sizeof(msg));
		msg[sizeof(msg) - 1] = 0;
		
		dgtpicom_set_text(msg, _beep, 0, 0);
		return;
	}
	
	_msg += " ";
	char msg_sm[DISPLAY_CHARS];
	int os = 0;
	const int overlen = _msg.size() - DISPLAY_CHARS;

	for (int i = 0; i < overlen; i++)
	{
		os = 0;
		for (int j = i; j < (DISPLAY_CHARS+i); j++)
		{
			msg_sm[os] = _msg.at(j);
			os++;
		}
		
		dgtpicom_set_text(msg_sm, _beep, 0, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(_scrol_speed));
	
	}
	
}



//GET REQUEST
struct request_result
{
	bool request_failed;
	std::string body;
	httplib::Error error;
	int status_code;
	std::string uri;
};


request_result make_request(std::string _base_url,std::string _path, std::string _interface)
{
	//PORT AND PROTOCOL IS ALREADY INCLUDED IN BASE URL LIKE http://127.0.0.1:3000
	httplib::Client cli(_base_url.c_str());
	cli.set_connection_timeout(10);
	cli.set_follow_location(true);    //AUTO REDIRECT IF GOT AN 300
	//SEND REQUEST OVER SELECTED INTERFACE
	if(!_interface.empty())
	{
		cli.set_interface(_interface.c_str());
	}
	
	
	request_result req_res;
	req_res.uri =  _base_url + _path;
	req_res.request_failed = false;
	//PERFORM REQUEST
    httplib::Result res = cli.Get(_path.c_str());
	if (res && res->status >= 200 && res->status < 300) {
		req_res.status_code = res->status;
		//CHECK STATUS CODE 200 IS VALID
		req_res.body = res->body;
		req_res.request_failed = false;
	}
	else {
		req_res.error = res.error();
		req_res.request_failed = true;
		req_res.body = "";
	}
	
	return req_res;
}


int main(int argc, char *argv[]) {
	
	int e;
	if(dgtpicom_init()) {
		return ERROR_MEM;
	}

	e = dgtpicom_configure(); //SET MODE 25
	if (e < 0) {
		return e;
	}
			
	//READ CONFIG FILE
	INIReader reader("/usr/ATC/atcdgt3000ext.ini");
	bool readres = true;
	if (reader.ParseError() != 0) {
		std::cout << "READING /usr/ATC/atcdgt3000ext.ini FAILED";
		scrol_text("READING /usr/ATC/atcdgt3000ext.ini FAILED", true, 500);
		return -1;
	}
	
	
	std::string BASE_URL = ""; //HOLDS THE TBALE REST API HOST http://xxx:port
	std::string ENABLE_UDP_DISCOVERY = "";
	bool enudbdis = true;
	bool device_discoered = false;
	ENABLE_UDP_DISCOVERY = reader.Get("GENERAL", "ENABLE_UDP_DISCOVERY", "1");
	if (ENABLE_UDP_DISCOVERY.empty()) {
		enudbdis = false;
	}
	else {
		if (ENABLE_UDP_DISCOVERY == "1" || ENABLE_UDP_DISCOVERY == "true" || ENABLE_UDP_DISCOVERY == "TRUE")
		{
			enudbdis = true;
		}
		else
		{
			enudbdis = false;
		}
	}	
	//ENABLE DISCOVERY
	if (enudbdis) {
		scrol_text("WAIT DISC", false, 100);
		udpdiscovery::PeerParameters parameters;
		parameters.set_port(kPort);
		parameters.set_application_id(kApplicationId);
		parameters.set_can_discover(true);
		parameters.set_can_use_broadcast(true);
		parameters.set_multicast_group_address(kMulticastAddress);
		parameters.set_can_use_multicast(true);
		
		scrol_text("-DISCOVERY-", false, 800);
		
		udpdiscovery::Peer peer;
		if (!peer.Start(parameters, OWN_DISCOVERY_USER_DATA)) {
			return 1;
			scrol_text("-DISCOVERY FAILED RETRY-", true, 800);
		}
		std::list<udpdiscovery::DiscoveredPeer> discovered_peers;
		std::map<udpdiscovery::IpPort, std::string> last_seen_user_datas;
		
		while (!device_discoered) {
			std::list<udpdiscovery::DiscoveredPeer> new_discovered_peers = peer.ListDiscovered();
			discovered_peers = new_discovered_peers;

			last_seen_user_datas.clear();
			for (std::list<udpdiscovery::DiscoveredPeer>::const_iterator it = discovered_peers.begin(); it != discovered_peers.end(); ++it) {
				last_seen_user_datas.insert(std::make_pair((*it).ip_port(), (*it).user_data()));
			}
			if(discovered_peers.size()){
				std::cout << "Discovered peers: " << discovered_peers.size() << std::endl;
			}
			for (std::list<udpdiscovery::DiscoveredPeer>::const_iterator it = discovered_peers.begin(); it != discovered_peers.end(); ++it) {
				std::cout << " - " << udpdiscovery::IpPortToString((*it).ip_port()) << ", " << (*it).user_data() << std::endl;
				//DEVICE DISCOCERED
				if ((*it).user_data() == EXPECTED_DISCOVERY_USER_DATA)
				{
					device_discoered = true;
					const std::string ip = udpdiscovery::IpToString((*it).ip_port().ip());
					
					BASE_URL = "http://" + ip + ":" + reader.Get("GENERAL", "BASE_URL_PORT_AFTER_DISCOVERY", "5000"); 
					
					//scrol_text("-DISCOVERY SUCCESS-", false, 800);
					break;
				}
			}
			
			int b = 0;
		}
	}
	
	
	
	if (!enudbdis || !device_discoered) {
		BASE_URL = reader.Get("GENERAL", "BASE_URL", "http://127.0.0.1:5000");
	}
	if (BASE_URL.empty()) {
		scrol_text("BASE_URL EMPTY", true, 300);
		return -3;
	}
	else {
		scrol_text(BASE_URL, false, 100);
	}
	
	
	std::string BTN_CHANGE_URL = "";
	BTN_CHANGE_URL = reader.Get("GENERAL", "BTN_CHANGE_URL", "/btn_changed");
	if (BTN_CHANGE_URL.empty()) {
		scrol_text("BTN_CHANGE_URL EMPTY", true, 300);
	}else {
		//scrol_text(BTN_CHANGE_URL, false, 300);
	}
	
	std::string GET_TEXT_URL = "";
	GET_TEXT_URL = reader.Get("GENERAL", "GET_TEXT_URL", "/get_text");
	if (GET_TEXT_URL.empty()) {
		scrol_text("GET_TEXT_URL EMPTY", true, 300);
		std::cout << "GET_TEXT_URL: " <<GET_TEXT_URL << std::endl;
	}
	else {
		//scrol_text(GET_TEXT_URL, false, 300);
		std::cout << "GET_TEXT_URL: " <<GET_TEXT_URL << std::endl;
	}
	
	std::string VERSION_URL = "";
	VERSION_URL = reader.Get("GENERAL", "VERSION_URL", "/version");
	if (VERSION_URL.empty()) {
		scrol_text("VERSION_URL EMPTY", true, 300);
		std::cout << "VERSION_URL: " <<VERSION_URL << std::endl;
	}
	else {
		std::cout << "VERSION_URL: " <<VERSION_URL << std::endl;
	//	scrol_text(VERSION_URL, false, 300);
	}
	
	std::string INTERFACE = "";
	GET_TEXT_URL = reader.Get("GENERAL", "INTERFACE", "");
	
	
	//TEST CONNECTION
	
	scrol_text("--- ATC ---", false, 300);
	
	
	request_result tres =  make_request(BASE_URL, VERSION_URL + "?devive=ATC_DGT300", INTERFACE);
	if (tres.request_failed)
	{
		std::cout << "TEST REQUEST FAILED";
		scrol_text("T REQ ERR", true, 300);
		return -2;
	}
	else
	{
		//scrol_text(tres.body, false, 300);
	}
		
		
	
	int old_btn_state = -1;
	int curr_btn_state = 0;
	std::string old_text = "";
	
	while (true) {
		
		//PROCESS BTN EVENT
		curr_btn_state =  dgtpicom_get_button_state();
		if (curr_btn_state != old_btn_state)
		{
			old_btn_state = curr_btn_state; 
			const bool back_btn = curr_btn_state & 0x01;
			const bool minus_btn = curr_btn_state & 0x02;
			const bool play_btn = curr_btn_state & 0x04;
			const bool plus_btn = curr_btn_state & 0x08;
			const bool forward_btn = curr_btn_state & 0x10;
			const bool on_btn = curr_btn_state & 0x20;
			const bool lever_right_down = curr_btn_state & 0x40;
			
			
			request_result res =  make_request(BASE_URL, BTN_CHANGE_URL + "?devive=ATC_DGT300&raw=" + std::to_string(curr_btn_state) + "&back_btn=" + std::to_string(back_btn) + "&minus_btn=" + std::to_string(minus_btn) + "&play_btn=" + std::to_string(play_btn) + "&plus_btn=" + std::to_string(plus_btn) + "&forward_btn=" + std::to_string(forward_btn) + "&on_btn=" + std::to_string(on_btn) + "&lever_right_down=" + std::to_string(lever_right_down), INTERFACE);
			if (res.request_failed)
			{
				std::cout << res.request_failed << " FAILED BTN_CHANGE_URL";
			}
		}
		
		//std::cout << BASE_URL << GET_TEXT_URL << "?devive=ATC_DGT300"<<std::endl;
		request_result textres =  make_request(BASE_URL,  "/get_text?devive=ATC_DGT300", INTERFACE);
		if (!textres.request_failed)
		{
			if (old_text != textres.body) {
				old_text = textres.body;
			scrol_text(textres.body, false, 300);
			std::cout << textres.body  << " " << std::endl;
			}
		}else{
			std::cout << textres.request_failed << " FAILED GET_TEXT_URL ";
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_SPPED));
	}
	/*
		} else {
		#ifdef debug
				printf("  %.3f ", (float)*timer / 1000000);
				printf("started\n");
				usleep(10000);
				ww = 1;
				pthread_t w;
				pthread_create(&w, NULL, wl, NULL);
				#endif
						if (dgtpicom_off(1) < 0)
							dgtpicom_off(1);
					but = tim = 0;
					while (1) {


						//	printf("rxMaxBuf=%d  ",bug.rxMaxBuf);

						//	dgt3000GetTime(t);
						//	printf("time=%d:%02d.%02d %d:%02d.%02d\n",t[0],t[1],t[2],t[3],t[4],t[5]);

							if(dgtpicom_get_button_message(&but, &tim)) {
							if (but & 0x40) {
								if (ww)
									ww = 0;
				else
					ww = 1;
	}
	}
	if (but == 0x20) {
		break;
	}
	printf("%.3f ", (float)*timer / 1000000);
	printf("button=%02x, time=%d\n", but, tim);
	//	dgt3000EndDisplay();
}

usleep(10000);
}
//		dgtpicom_configure();
//		dgtpicom_set_text("Booting",0,0,0);
}
*/
	dgtpicom_stop();

#ifdef debug
	printf("%.3f ", (float)*timer / 1000000);
	printf("After %d messages:\n", bug.sendTotal);
	printf("Send failed: display=%d, endDisplay=%d, changeState=%d, setCC=%d, setNRun=%d\n",
		bug.displaySF,
		bug.endDisplaySF,
		bug.changeStateSF,
		bug.setCCSF,
		bug.setNRunSF);
	printf("Ack failed : display=%d, endDisplay=%d, changeState=%d, setCC=%d, setNRun=%d\n",
		bug.displayAF,
		bug.endDisplayAF,
		bug.changeStateAF,
		bug.setCCAF,
		bug.setNRunAF);
	printf("Recieve Errors: timeout=%d, wrongAdr=%d, bufferFull=%d, sizeMismatch=%d, CRCFault=%d\n",
		bug.rxTimeout,
		bug.rxWrongAdr,
		bug.rxBufferFull,
		bug.rxSizeMismatch,
		bug.rxCRCFault);
	printf("Max recieve buffer size=%d\n", bug.rxMaxBuf);
#endif

	// succes?
	return ERROR_OK;
}

// Get direct access to BCM2708/9 chip.
int dgtpicom_init() {
	int memfd, base;
	void *gpio_map, *timer_map, *i2c_slave_map, *i2c_master_map;
	struct sched_param params;

	memset(&dgtRx, 0, sizeof(dgtReceive_t));
#ifdef debug
	memset(&bug, 0, sizeof(debug_t));
#endif

	if (checkPiModel() == 1)
		base = 0x20000000;
	else
		base = 0x3f000000;

	memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (memfd < 0) {
#ifdef debug
		printf("/dev/mem open error, run as root\n");
#endif
		return ERROR_MEM;
	}

	gpio_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, GPIO_BASE + base);
	timer_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, TIMER_BASE + base);
	i2c_slave_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, I2C_SLAVE_BASE + base);
	i2c_master_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, I2C_MASTER_BASE + base);

	close(memfd);

	if (gpio_map == MAP_FAILED || timer_map == MAP_FAILED || i2c_slave_map == MAP_FAILED || i2c_master_map == MAP_FAILED) {
#ifdef debug
		printf("Map failed\n");
#endif
		return ERROR_MEM;
	}

	// GPIO pointers
	gpio =  (volatile unsigned *)gpio_map;
	gpioset = gpio + 7;       // set bit register offset 28
	gpioclr = gpio + 10;      // clr bit register
	gpioin = gpio + 13;       // read all bits register

	// timer pointer
	timer = (long long int *)((char *)timer_map + 4);

	// i2c slave pointers
	i2cSlave = (volatile unsigned *)i2c_slave_map;
	i2cSlaveRSR = i2cSlave + 1;
	i2cSlaveSLV = i2cSlave + 2;
	i2cSlaveCR = i2cSlave + 3;
	i2cSlaveFR = i2cSlave + 4;

	// i2c master pointers
	i2cMaster = (volatile unsigned *)i2c_master_map;
	i2cMasterS = i2cMaster + 1;
	i2cMasterDLEN = i2cMaster + 2;
	i2cMasterA = i2cMaster + 3;
	i2cMasterFIFO = i2cMaster + 4;
	i2cMasterDiv = i2cMaster + 5;

	// check wiring
	// configured as an output? probably in use for something else
	if((*gpio & 0x1c0) == 0x40) {
#ifdef debug
		printf("Error, GPIO02 configured as output, in use? We asume not a DGTPI\n");
#endif
		return ERROR_LINES;
	}
	if ((*gpio & 0xe00) == 0x200) {
#ifdef debug
		printf("Error, GPIO03 configured as output, in use? We asume not a DGTPI\n");
#endif
		return ERROR_LINES;
	}
	if ((*(gpio + 1) & 0x07000000) == 0x01000000) {
#ifdef debug
		printf("Error, GPIO18 configured as output, in use? We asume not a DGTPI\n");
#endif
		return ERROR_LINES;
	}
	if ((*(gpio + 1) & 0x38000000) == 0x08000000) {
#ifdef debug
		printf("Error, GPIO19 configured as output, in use? We asume not a DGTPI\n");
#endif
		return ERROR_LINES;
	}
	// pinmode GPIO2,GPIO3=input
	*gpio &= 0xfffff03f;
	// pinmode GPIO18,GPIO19=input
	*(gpio + 1) &= 0xc0ffffff;
	usleep(1);
	// all pins hi through pullup?
	if((*gpioin & 0xc000c) != 0xc000c) {
#ifdef debug
		printf("Error, pin(s) low, shortcircuit, or no connection?\n");
#endif
		return ERROR_LINES;
	}
	/*	// check SDA connection
		// gpio18 low
		*gpioclr = 1<<18;
		// gpio18 output
		*(gpio+1) |= 0x01000000;
		usleep(1);
		// check gpio 18 and 2
		if ((*gpioin & 0x40004)!=0) {
			#ifdef debug
			printf("Error, SDA not connected\n");
			#endif
			// gpio18 back to input
			*(gpio+1) &= 0xf8ffffff;
			return ERROR_LINES;
		}
		if ((*gpioin & 0x80008)!=0x80008) {
			#ifdef debug
			printf("Error, SDA connected to SCL\n");
			#endif
			// gpio18 back to input
			*(gpio+1) &= 0xf8ffffff;
			return ERROR_LINES;
		}
		// gpio18 back to input
		*(gpio+1) &= 0xf8ffffff;
		// check SCL connection
		// gpio19 low
		*gpioclr = 1<<19;
		// gpio19 output
		*(gpio+1) |= 0x08000000;
		usleep(1);
		// check gpio 19 and 3
		if ((*gpioin & 0x80008)!=0) {
			#ifdef debug
			printf("Error, SCL not connected\n");
			#endif
			// gpio19 back to input
			*(gpio+1) &= 0xc7ffffff;
			return ERROR_LINES;
		}
		// gpio19 back to input
		*(gpio+1) &= 0xc7ffffff;
		*/
	i2cReset();

	// set to I2CMaster destination adress
	*i2cMasterA = 8;

	dgtRx.on = 1;

	pthread_create(&receiveThread, NULL, dgt3000Receive, NULL);

	// give thread max priority
	params.sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_setschedparam(receiveThread, SCHED_FIFO, &params);

	return ERROR_OK;
}

// Configure the dgt3000.
int dgtpicom_configure() {
	int e;
	int wakeCount = 0;
	int setCCCount = 0;
	int resetCount = 0;


	// get the clock into the right state
	while(1) {
		// set to mode 25 and run
		e = dgt3000Mode25();
		if (e == ERROR_NACK || e == ERROR_NOACK) {
			// no postive ack, not in cc
			// set central controll
			// try 3 times
			setCCCount++;
			// setCC>3?
			if(setCCCount > 3) {
#ifdef debug
				ERROR_PIN_HI;
				printf("%.3f ", (float)*timer / 1000000);
				printf("sending setCentralControll failed three times\n\n");
				ERROR_PIN_LO;
#endif
				//return e;
			}
			usleep(10000);
			dgt3000SetCC();
		}
		else if (e == ERROR_TIMEOUT) {
			// timeout, line stay low -> reset i2c
			resetCount++;
			if (resetCount > 1) {
#ifdef debug
				ERROR_PIN_HI;
				printf("%.3f ", (float)*timer / 1000000);
				printf("I2C error, remove jack plug\n\n");
				ERROR_PIN_LO;
#endif
				return e;
			}
			i2cReset();
			continue;
		}
		else if (e == ERROR_CST || e == ERROR_LINES) {
			// message not acked, probably collision
			continue;
		}
		else if (e == ERROR_SILENT) {
			// message not acked, probably clock off -> wake
			// wake#++
			wakeCount++;

			// wake#>3? -> error
			if(wakeCount > 3) {
#ifdef debug
				ERROR_PIN_HI;
				printf("%.3f ", (float)*timer / 1000000);
				printf("sending wake command failed three times\n");
				ERROR_PIN_LO;
#endif
				return e;
			}
			dgt3000Wake();
			continue;
		}
		else {
			// succes!
			usleep(5000);
			break;
		}
	}
	return ERROR_OK;
}

// send set and run command to dgt3000
int dgtpicom_set_and_run(char lr,
	char lh,
	char lm,
	char ls,
	char rr,
	char rh,
	char rm,
	char rs) {
	int e;
	int sendCount = 0;

	setnrun[4] = lh;
	setnrun[5] = ((lm / 10) << 4) | (lm % 10);
	setnrun[6] = ((ls / 10) << 4) | (ls % 10);
	setnrun[7] = rh;
	setnrun[8] = ((rm / 10) << 4) | (rm % 10);
	setnrun[9] = ((rs / 10) << 4) | (rs % 10);
	setnrun[10] = lr | (rr << 2);

	crc_calc(setnrun);

	while (1) {
		sendCount++;
		if (sendCount > 3) {
#ifdef debug
			ERROR_PIN_HI;
			printf("%.3f ", (float)*timer / 1000000);
			printf("sending SetNRun failed three times on error%d\n\n", e);
			ERROR_PIN_LO;
#endif
			return e;
		}

		e = dgt3000SetNRun(setnrun);

		// succes?
		if(e == ERROR_OK)
			return ERROR_OK;
	}
}

// Send set and run command to the dgt3000 with current clock values.
int dgtpicom_run(char lr, char rr) {
	return dgtpicom_set_and_run(
				lr,
		dgtRx.time[0],
		((dgtRx.time[1] & 0xf0) >> 4)*10 + (dgtRx.time[1] & 0x0f),
		((dgtRx.time[2] & 0xf0) >> 4)*10 + (dgtRx.time[2] & 0x0f),
		rr,
		dgtRx.time[3],
		((dgtRx.time[4] & 0xf0) >> 4)*10 + (dgtRx.time[4] & 0x0f),
		((dgtRx.time[5] & 0xf0) >> 4)*10 + (dgtRx.time[5] & 0x0f));
}

// Set a text message on the DGT3000.
int dgtpicom_set_text(char text[], char beep, char ld, char rd) {
	int i, e;
	int sendCount = 0;


	for (i = 0; i < 11; i++) {
		if (text[i] == 0) break;
		display[i + 4] = text[i];
	}

	for (; i < 11; i++) {
		display[i + 4] = 32;
	}

	display[16] = beep;
	display[18] = ld;
	display[19] = rd;

	crc_calc(display);

	while (1) {
		sendCount++;
		if (sendCount > 3) {
#ifdef debug
			ERROR_PIN_HI;
			printf("%.3f ", (float)*timer / 1000000);
			printf("sending clear display failed three times on error%d\n\n", e);
			ERROR_PIN_LO;
#endif
			return e;
		}

		e = dgt3000EndDisplay();
		// succes?
		if(e == ERROR_OK)
			break;
	}

	sendCount = 0;
	while (1) {
		sendCount++;
		if (sendCount > 3) {
#ifdef debug
			ERROR_PIN_HI;
			printf("%.3f ", (float)*timer / 1000000);
			printf("sending display command failed three times on error%d\n\n", e);
			ERROR_PIN_LO;
#endif
			return e;
		}
		// succes?
		e = dgt3000Display(display);
		if (e == ERROR_OK)
			break;
	}
	return ERROR_OK;
}

// End a text message on the DGT3000 an return to clock mode.
int dgtpicom_end_text() {
	int e;
	int sendCount = 0;

	while (1) {
		sendCount++;
		if (sendCount > 3) {
#ifdef debug
			ERROR_PIN_HI;
			printf("%.3f ", (float)*timer / 1000000);
			printf("sending end display failed three times on error%d\n\n", e);
			ERROR_PIN_LO;
#endif
			return e;
		}

		e = dgt3000EndDisplay();
		// succes?
		if(e == ERROR_OK)
			return ERROR_OK;
	}
}

// Put the last received time message in time[].
void dgtpicom_get_time(char time[]) {
	time[0] = dgtRx.time[0];
	time[1] = ((dgtRx.time[1] & 0xf0) >> 4) * 10 + (dgtRx.time[1] & 0x0f);
	time[2] = ((dgtRx.time[2] & 0xf0) >> 4) * 10 + (dgtRx.time[2] & 0x0f);
	time[3] = dgtRx.time[3];
	time[4] = ((dgtRx.time[4] & 0xf0) >> 4) * 10 + (dgtRx.time[4] & 0x0f);
	time[5] = ((dgtRx.time[5] & 0xf0) >> 4) * 10 + (dgtRx.time[5] & 0x0f);
}

// Get a button message from the buffer returns number of messages in
// the buffer or recieve error if one occured.
int dgtpicom_get_button_message(char *buttons, char *time) {
	int e = dgtRx.error;
	dgtRx.error = 0;
	if (e < 0)
		return e;

	//button availible?
	if(dgtRx.buttonStart != dgtRx.buttonEnd) {
		*buttons = dgtRx.buttonPres[dgtRx.buttonStart];
		*time = dgtRx.buttonTime[dgtRx.buttonStart];
		dgtRx.buttonStart = (dgtRx.buttonStart + 1) % DGTRX_BUTTON_BUFFER_SIZE;
		return (dgtRx.buttonEnd - dgtRx.buttonStart) % DGTRX_BUTTON_BUFFER_SIZE + 1;
	} else {
		return ERROR_OK;
	}
}

// Return the current button state.
int dgtpicom_get_button_state() {
	return dgtRx.lastButtonState;
}

// Turn off the dgt3000.
int dgtpicom_off(char returnMode) {
	int e;

	mode25[4] = 32 + returnMode;
	crc_calc(mode25);


	/*	// send mode 25 message
		e=i2cSend(mode25,0x10);

		// send succesful?
		if (e<0) {
			#ifdef debug
			bug.changeStateSF++;
			#endif
			return e;
		}

		// listen to our own adress an get Reply
		e=dgt3000GetAck(0x10,0x0b,10000);

		// ack received?
		if (e<0) {
			#ifdef debug
			bug.changeStateAF++;
			#endif
			return e;
		}

		// negetive ack not in CC
		if (dgtRx.ack[1]!=8)
			return ERROR_NACK;
			*/
	mode25[4] = 0;
	crc_calc(mode25);


	// send mode 25 message
	e = i2cSend(mode25, 0x00);

	// send succesful?
	if(e < 0) {
#ifdef debug
		bug.changeStateSF++;
#endif
		return e;
	}

	return ERROR_OK;
}

// Disable the I2C hardware.
void dgtpicom_stop() {
	// stop listening to broadcasts
	*i2cSlaveSLV = 16;

	// stop thread
	dgtRx.on = 0;

	// wait for thread to finish
	pthread_join(receiveThread, NULL);

	// disable i2cSlave device
	*i2cSlaveCR = 0;

	// pinmode GPIO2,GPIO3=input
	*gpio &= 0xfffff03f;
	// pinmode GPIO18,GPIO19=input
	*(gpio + 1) &= 0xc0ffffff;
}





// send a wake command to the dgt3000
int dgt3000Wake() {
	int e;
	long long int t;



	// send wake
	*i2cMasterA = 40;
	e = i2cSend(ping, 0x00);
	*i2cMasterA = 8;

	// succes? -> error. Wake messages should never get an Ack
	if(e == ERROR_OK) {
#ifdef debug
		ERROR_PIN_HI;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending wake command failed, received Ack, this should never hapen\n");
		ERROR_PIN_LO;
#endif
		return ERROR_NACK;
	}

	// Get Hello message (in max 10ms, usualy 5ms)
	t = *timer + 10000;
	while (*timer < t) {
		if (dgtRx.hello == 1)
			return ERROR_OK;
		usleep(100);
	}

#ifdef debug
	ERROR_PIN_HI;
	printf("%.3f ", (float)*timer / 1000000);
	printf("sending wake command failed, no hello\n");
	ERROR_PIN_LO;
#endif

	return ERROR_NOACK;
}

// send set central controll command to dgt3000
int dgt3000SetCC() {
	int e;

	// send setCC, error? retry
	e = i2cSend(centralControll, 0x10);

	// send succedfull?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.setCCSF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending SetCentralControll command failed, sending failed\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// listen to our own adress and get Reply

	e = dgt3000GetAck(0x10, 0x0f, 10000);

	// ack received?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.setCCAF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending SetCentralControll command failed, no ack\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// is positive ack?
	if((dgtRx.ack[1] & 8) == 8)
		return ERROR_OK;

#ifdef debug
	ERROR_PIN_HI;
	printf("%.3f ", (float)*timer / 1000000);
	printf("sending SetCentralControll command failed, negative ack, clock running\n");
	ERROR_PIN_LO;
#endif

	// nack clock running
	return ERROR_NACK;
}

// send set mode 25 to dgt3000
int dgt3000Mode25() {
	int e;

	mode25[4] = 57;
	crc_calc(mode25);

	// send mode 25 message
	e = i2cSend(mode25, 0x10);

	// send succesful?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.changeStateSF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending mode25 command failed, sending failed\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// listen to our own adress an get Reply
	e = dgt3000GetAck(0x10, 0x0b, 10000);

	// ack received?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.changeStateAF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending mode25 command failed, no ack\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	if (dgtRx.ack[1] == 8) return ERROR_OK;

#ifdef debug
	ERROR_PIN_HI;
	printf("%.3f ", (float)*timer / 1000000);
	printf("sending mode25 command failed, negative ack, not in Central Controll\n");
	ERROR_PIN_LO;
#endif

	// negetive ack not in CC
	return ERROR_NACK;
}

// send end display to dgt3000 to clear te display
int dgt3000EndDisplay() {
	int e;

	// send end Display
	e = i2cSend(endDisplay, 0x10);

	// send succesful?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.endDisplaySF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending end display command failed, sending failed\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// get fast Reply = already empty
	e = dgt3000GetAck(0x10, 0x07, 1200);

	// display already empty
	if(e == ERROR_OK) {
		if ((dgtRx.ack[1] & 0x07) == 0x05) {
			return ERROR_OK;
		}
		else {
#ifdef debug
			ERROR_PIN_HI;
			printf("%.3f ", (float)*timer / 1000000);
			printf("sending end display command failed, negative specific ack:%02x\n", dgtRx.ack[1]);
			ERROR_PIN_LO;
#endif
			return ERROR_NACK;
		}
	}

	//get slow broadcast Reply = display changed
	e = dgt3000GetAck(0x00, 0x07, 10000);

	// ack received?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.endDisplayAF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending end display command failed, no ack\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// display emptied
	if((dgtRx.ack[1] & 0x07) == 0x00)
		return ERROR_OK;

#ifdef debug
	ERROR_PIN_HI;
	printf("%.3f ", (float)*timer / 1000000);
	printf("sending end display command failed, negative broadcast ack:%02x\n", dgtRx.ack[1]);
	ERROR_PIN_LO;
#endif

	return ERROR_NACK;
}

// send display command to dgt3000
int dgt3000Display(char dm[]) {
	int e;

	// send the message
	e = i2cSend(dm, 0x00);

	// send succesful?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.displaySF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending display command failed, sending failed\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// get (broadcast) reply
	e = dgt3000GetAck(0x00, 0x06, 10000);

	// no reply
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.displayAF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending display command failed, no ack\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// nack, already displaying message
	if((dgtRx.ack[1] & 0xf3) == 0x23) {
#ifdef debug
		ERROR_PIN_HI;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending display command failed, display already busy\n");
		ERROR_PIN_LO;
#endif
		return ERROR_NACK;
	}

	return ERROR_OK;
}

// send set and run command to dgt3000
int dgt3000SetNRun(char srm[]) {
	int e;

	e = i2cSend(srm, 0x10);

	// send succesful?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.setNRunSF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending SetNRun command failed, sending failed\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// listen to our own adress an get Reply
	e = dgt3000GetAck(0x10, 0x0a, 10000);

	// ack received?
	if(e < 0) {
#ifdef debug
		ERROR_PIN_HI;
		bug.setNRunAF++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("sending SetNRun command failed, no ack\n");
		ERROR_PIN_LO;
#endif
		return e;
	}

	// Positive Ack?
	if(dgtRx.ack[1] == 8)
		return ERROR_OK;

	// nack
#ifdef debug
	ERROR_PIN_HI;
	printf("%.3f ", (float)*timer / 1000000);
	printf("sending SetNRun command failed, not in mode 25\n");
	ERROR_PIN_LO;
#endif
	return ERROR_NACK;
}



// check for messages from dgt3000
void *dgt3000Receive(void *a) {
	char rm[RECEIVE_BUFFER_LENGTH];
	int e;
#ifdef debug2
	int i;
#endif

#ifdef debug
	RECEIVE_THREAD_RUNNING_PIN_HI;
#endif
	
	dgtRx.buttonRepeatTime = 0;

	while (dgtRx.on) {
		if ((*i2cSlaveFR & 0x20) != 0 || (*i2cSlaveFR & 2) == 0) {
			pthread_mutex_lock(&receiveMutex);

			e = i2cReceive(rm);

#ifdef debug2
			if (e > 0) {
				printf("<- ");
				//	printf("maxs=%d  ",maxs);
				//	printf("length=%d  ",e);
					for(i = 0 ; i < e ; i++)
						printf("%02x ", rm[i]);
			}
			else if (e < 0) {
				printf("<- ");
				for (i = 0; i < 16; i++)
					printf("%02x ", rm[i]);
			}
#endif

			if (e > 0) {
				switch (rm[3]) {
				case 1:		// ack
					dgtRx.ack[0] = rm[4];
					dgtRx.ack[1] = rm[5];
					pthread_cond_signal(&receiveCond);
#ifdef debug2
					printf("= Ack %s\n", packetDescriptor[rm[4] - 1]);
#endif
					break;
				case 2:		// hello
					dgtRx.hello = 1;
#ifdef debug2
					printf("= Hello\n");
#endif
					break;
				case 4:		// time
					dgtRx.time[0] = rm[5] & 0x0f;
					dgtRx.time[1] = rm[6];
					dgtRx.time[2] = rm[7];
					dgtRx.time[3] = rm[11] & 0x0f;
					dgtRx.time[4] = rm[12];
					dgtRx.time[5] = rm[13];
					// store (initial) lever state
					if((rm[19] & 1) == 1)
						dgtRx.lastButtonState |= 0x40;
					else
						dgtRx.lastButtonState &= 0xbf;
#ifdef debug2
					printf("= Time: %02x:%02x.%02x %02x:%02x.%02x\n", rm[5] & 0xf, rm[6], rm[7], rm[11] & 0xf, rm[12], rm[13]);
#endif
					if (rm[20] == 1) ; // no update
					break;
				case 5:		// button
					// new button pressed
					if(rm[4] & 0x1f) {
						dgtRx.buttonState |= rm[4] & 0x1f;
						dgtRx.lastButtonState = rm[4];
						dgtRx.buttonRepeatTime = *timer + 500000;
						dgtRx.buttonCount = 0;
							
						// buffer full?
						if((dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE == dgtRx.buttonStart) {
#ifdef debug
							printf("%.3f ", (float)*timer / 1000000);
							printf("Button buffer full, buttons ignored\n");
#endif
						} else {
							dgtRx.buttonPres[dgtRx.buttonEnd] = dgtRx.buttonState;
							dgtRx.buttonTime[dgtRx.buttonEnd] = dgtRx.buttonCount;
							dgtRx.buttonEnd = (dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE;
						}
					}
					// turned off/on
					if((rm[4] & 0x20) != (rm[5] & 0x20)) {
						// buffer full?
						if((dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE == dgtRx.buttonStart) {
#ifdef debug
							printf("%.3f ", (float)*timer / 1000000);
							printf("Button buffer full, on/off ignored\n");
#endif
						} else {
							dgtRx.buttonPres[dgtRx.buttonEnd] = 0x20 | ((rm[5] & 0x20) << 2);
							dgtRx.buttonTime[dgtRx.buttonEnd] = 0;
							dgtRx.buttonEnd = (dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE;
						}
					}

					// lever change?
					if((rm[4] & 0x40) != (rm[5] & 0x40)) {
						// buffer full?
						if((dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE == dgtRx.buttonStart) {
#ifdef debug
							printf("%.3f ", (float)*timer / 1000000);
							printf("Button buffer full, lever change ignored\n");
#endif
						} else {
							dgtRx.buttonPres[dgtRx.buttonEnd] = 0x40 | ((rm[4] & 0x40) << 1);
							dgtRx.buttonTime[dgtRx.buttonEnd] = 0;
							dgtRx.buttonEnd = (dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE;
						}
					}

					// buttons released
					if((rm[4] & 0x1f) == 0 && dgtRx.buttonState != 0) {
						dgtRx.buttonRepeatTime = 0;
						dgtRx.buttonState = 0;
					}
#ifdef debug2
					printf("= Button: 0x%02x>0x%02x\n", rm[5] & 0x7f, rm[4] & 0x7f);
#endif
					break;
#ifdef debug
				default:
					ERROR_PIN_HI;
					printf("%.3f ", (float)*timer / 1000000);
					printf("Receive Error: Unknown message from clock\n");
					ERROR_PIN_LO;
#endif
				}
			}
			else  if (e < 0) {
#ifdef debug2
				printf(" = Error: %d\n", e);
#endif
				dgtRx.error = e;
			}
			pthread_mutex_unlock(&receiveMutex);
		}
		else {
			if (dgtRx.buttonRepeatTime != 0 && dgtRx.buttonRepeatTime < *timer) {
				dgtRx.buttonRepeatTime += 250000;
				dgtRx.buttonCount++;
				
				// buffer full?
				if((dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE == dgtRx.buttonStart) {
#ifdef debug
					printf("%.3f ", (float)*timer / 1000000);
					printf("Button buffer full, repeated buttons ignored\n");
#endif
				} else {
					dgtRx.buttonPres[dgtRx.buttonEnd] = dgtRx.buttonState;
					dgtRx.buttonTime[dgtRx.buttonEnd] = dgtRx.buttonCount;
					dgtRx.buttonEnd = (dgtRx.buttonEnd + 1) % DGTRX_BUTTON_BUFFER_SIZE;
				}
			}
#ifdef debug
			RECEIVE_THREAD_RUNNING_PIN_LO;
#endif
			usleep(400);
#ifdef debug
			RECEIVE_THREAD_RUNNING_PIN_HI;
#endif

		}
	}
#ifdef debug
	RECEIVE_THREAD_RUNNING_PIN_LO;
#endif

	return ERROR_OK;
}

// wait for an Ack message
int dgt3000GetAck(char adr, char cmd, long long int timeOut) {
	struct timespec receiveTimeOut;


	pthread_mutex_lock(&receiveMutex);


	// listen to given adress
//	while ((*i2cSlaveFR&0x20) != 0 );
	*i2cSlaveSLV = adr;

	// check until timeout
	timeOut += *timer;
	receiveTimeOut.tv_sec = timeOut / 1000000;
	receiveTimeOut.tv_nsec = timeOut % 1000000;

	while (*timer < timeOut) {
		if (dgtRx.ack[0] == cmd) {
			pthread_mutex_unlock(&receiveMutex);
			return ERROR_OK;
		}
		pthread_cond_timedwait(&receiveCond, &receiveMutex, &receiveTimeOut);
	}

	// listen for broadcast again
	*i2cSlaveSLV = 0x00;
	pthread_mutex_unlock(&receiveMutex);

	if (dgtRx.ack[0] == cmd)
		return ERROR_OK;
	else
		return ERROR_NOACK;
}




// send message using I2CMaster
int i2cSend(char message[], char ackAdr) {
	int i, n;
	long long int timeOut;

	// set length
	*i2cMasterDLEN = message[2] - 1;

	// clear buffer
	*i2cMaster = 0x10;

#ifdef debug2
	printf("-> %02x ", message[0]);
#endif

	// fill the buffer
	for(n = 1 ; n < message[2] && *i2cMasterS & 0x10 ; n++) {
#ifdef debug2
		printf("%02x ", message[n]);
		if (n == message[2] - 1)
			printf("= %s\n", packetDescriptor[message[3] - 1]);
#endif
		*i2cMasterFIFO = message[n];
	}

	// check 256 times if the bus is free. At least for 50us because the clock will send waiting messages 50 us after the previeus one.
	timeOut = *timer + 10000;     // bus should be free in 10ms
#ifdef debug
	WAIT_FOR_FREE_BUS_PIN_HI;
#endif
	for (i = 0; i < 256; i++) {
		// lines low (data is being send, or plug half inserted, or PI I2C peripheral crashed or ...)
		if((SCL1IN == 0) || (SDA1IN == 0)) {
			i = 0;
		}
		if (((*i2cSlaveFR & 0x20) != 0) || ((*i2cSlaveFR & 2) == 0)) {
			i = 0;
		}
		// timeout waiting for bus free, I2C Error (or someone pushes 500 buttons/seccond)
		if(*timer > timeOut) {
#ifdef debug
			printf("%.3f ", (float)*timer / 1000000);
			printf("    Send error: Bus free timeout, waited more then 10ms for bus to be free\n");
			if (SCL1IN == 0)
				printf("                SCL low. Remove jack?\n");
			if (SDA1IN == 0)
				printf("                SDA low. Remove jack?\n");
			if ((*i2cSlaveFR & 0x20) != 0)
				printf("                I2C Slave receive busy, is the receive thread running?\n");
			if ((*i2cSlaveFR & 2) == 0)
				printf("                I2C Slave receive fifo not emtpy, is the receive thread running?\n");
#endif
			return ERROR_TIMEOUT;
		}
	}
#ifdef debug
	WAIT_FOR_FREE_BUS_PIN_LO;
#endif

	// clear ack and hello so we can receive a new ack or hello
	dgtRx.ack[0] = 0;
	dgtRx.hello = 0;

	// dont let the slave listen to 0 (wierd errors)?
	// listen to ack adress
	*i2cSlaveSLV = ackAdr;

	// start sending
	*i2cMasterS = 0x302;
	*i2cMaster = 0x8080;

	// write the rest of the message
	for(; n < message[2] ; n++) {
		// wait for space in the buffer
		timeOut = *timer + 10000;     // should be done in 10ms
		while((*i2cMasterS & 0x10) == 0) {
			if (*i2cMasterS & 2) {
				*i2cSlaveSLV = 0x00;
#ifdef debug
				printf("%.3f ", (float)*timer / 1000000);
				printf("    Send error: done before complete send\n");
#endif
				break;
			}
			if (*timer > timeOut) {
				*i2cSlaveSLV = 0x00;
#ifdef debug
				printf("%.3f ", (float)*timer / 1000000);
				printf("    Send error: Buffer free timeout, waited more then 10ms for space in the buffer\n");
#endif
				return ERROR_TIMEOUT;
			}
		}
		if (*i2cMasterS & 2)
			break;
#ifdef debug2
		printf("%02x ", message[n]);
		if (n == message[2] - 1)
			printf("= %s\n", packetDescriptor[message[3] - 1]);
#endif
		*i2cMasterFIFO = message[n];
	}

	// wait for done
	timeOut = *timer + 10000;     // should be done in 10ms
	while((*i2cMasterS & 2) == 0)
		if(*timer > timeOut) {
		*i2cSlaveSLV = 0x00;
#ifdef debug
		printf("%.3f ", (float)*timer / 1000000);
		printf("    Send error: done timeout, waited more then 10ms for message to be finished sending\n");
#endif
		return ERROR_TIMEOUT;
	}

	// succes?
	if((*i2cMasterS & 0x300) == 0) {
		//		*i2cSlaveSLV = ackAdr;
				return ERROR_OK;
	}

	*i2cSlaveSLV = 0x00;

	// collision or clock off
	if(*i2cMasterS & 0x100) {
		// reset error flags
		*i2cMasterS = 0x100;
#ifdef debug
		printf("%.3f ", (float)*timer / 1000000);
		printf("    Send error: byte not Acked\n");
#endif
	}
	if (*i2cMasterS & 0x200) {
		// reset error flags
		*i2cMasterS = 0x200;
#ifdef debug
		printf("%.3f ", (float)*timer / 1000000);
		printf("    Send error: collision, clock stretch timeout\n");
#endif

		// probably collision
		return ERROR_CST;
	}

	// clear fifo
	*i2cMaster |= 0x10;

	if ((SCL1IN == 0) || (SDA1IN == 0) || ((*i2cSlaveFR & 0x20) != 0) || ((*i2cSlaveFR & 2) == 0)) {
#ifdef debug
		printf("%.3f ", (float)*timer / 1000000);
		printf("    Send error: collision, lines busy after send.\n");
#endif

		// probably collision
		return ERROR_LINES;
	}

	// probably clock off
	return ERROR_SILENT;
}

// get message from I2C receive buffer
int i2cReceive(char m[]) {
	// todo implement end of packet check
	int i = 1;
	long long int timeOut;

	m[0] = *i2cSlaveSLV * 2;

	// a message should be finished receiving in 10ms
	timeOut = *timer + 10000;

#ifdef debug
	if (bug.rxMaxBuf<(*i2cSlaveFR & 0xf800)>>11)
		bug.rxMaxBuf = (*i2cSlaveFR & 0xf800) >> 11;
#endif

	// while I2CSlave is receiving or byte availible
	while(((*i2cSlaveFR & 0x20) != 0) || ((*i2cSlaveFR & 2) == 0)) {

		// timeout
		if(timeOut < *timer) {
#ifdef debug
			ERROR_PIN_HI;
			printf("%.3f ", (float)*timer / 1000000);
			printf("    Receive error: Timeout, hardware stays in receive mode for more then 10ms\n");
			bug.rxTimeout++;
			hexPrint(m, i);
			ERROR_PIN_LO;
#endif
			pthread_mutex_unlock(&receiveMutex);
			return ERROR_TIMEOUT;
		}

		// when a byte is availible, store it
		if((*i2cSlaveFR & 2) == 0) {
			m[i] = *i2cSlave & 0xff;
			i++;
			// complete packet
			if(i > 2 && i >= m[2])
				break;
			if (i >= RECEIVE_BUFFER_LENGTH) {
#ifdef debug
				ERROR_PIN_HI;
				bug.rxWrongAdr++;
				printf("%.3f ", (float)*timer / 1000000);
				printf("    Receive error: Buffer overrun, size to large for the supplied buffer %d bytes.\n", i);
				hexPrint(m, i);
				ERROR_PIN_LO;
#endif
				return ERROR_SWB_FULL;
			}
		} else {
			// no byte availible receiving a new one will take 70us
#ifdef debug
				RECEIVE_THREAD_RUNNING_PIN_LO;
#endif
			usleep(10);
#ifdef debug
			RECEIVE_THREAD_RUNNING_PIN_HI;
#endif
		}
	}

	// listen for broadcast again
	*i2cSlaveSLV = 0x00;

	m[i] = -1;

	// nothing?
	if(i == 1)
		return ERROR_OK;

	// dgt3000 sends to 0 bytes after some packets
	if(i == 3 && m[1] == 0 && m[2] == 0)
		return ERROR_OK;

	// not from clock?
	if(m[1] != 16) {
#ifdef debug
		ERROR_PIN_HI;
		bug.rxWrongAdr++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("    Receive error: Wrong adress, Received message not from clock (16) but from %d.\n", m[1]);
		hexPrint(m, i);
		ERROR_PIN_LO;
#endif
		return ERROR_NACK;
	}

	// errors?
	if(*i2cSlaveRSR & 1 || i < 5 || i != m[2]) {
#ifdef debug
		ERROR_PIN_HI;
		printf("%.3f ", (float)*timer / 1000000);
		if (*i2cSlaveRSR & 1) {
			printf("    Receive error: Hardware buffer full.\n");
			bug.rxBufferFull++;
		}
		else {
			if (i < 5)
				printf("    Receive Error: Packet to small, %d bytes.\n", i);
			else
				printf("    Receive Error: Size mismatch, packet length is %d bytes but received %d bytes.\n", m[2], i);
			bug.rxSizeMismatch++;
		}
		hexPrint(m, i);
		ERROR_PIN_LO;
#endif
		*i2cSlaveRSR = 0;
		return ERROR_HWB_FULL;
	}

	if (crc_calc(m)) {
#ifdef debug
		ERROR_PIN_HI;
		bug.rxCRCFault++;
		printf("%.3f ", (float)*timer / 1000000);
		printf("    Receive error: CRC Error\n");
		hexPrint(m, i);
		ERROR_PIN_LO;
#endif
		return ERROR_CRC;
	}

	return i;
}

// read register
static unsigned int dummyRead(volatile unsigned int *addr) {
	return *addr;
}

// configure IO pins and I2C Master and Slave
void i2cReset() {
	*i2cSlaveCR = 0;
	*i2cMaster = 0x10;
	*i2cMaster = 0x0000;

	// pinmode GPIO2,GPIO3=input (togle via input to reset i2C master(sometimes hangs))
	*gpio &= 0xfffff03f;
	// pinmode GPIO18,GPIO19=input (togle via input to reset)
	*(gpio + 1) &= 0x00ffffff;
	// send something in case master hangs
	//*i2cMasterFIFO = 0x69;
	*i2cMasterDLEN = 0;
	//	*i2cMaster = 0x8080;
		while((*i2cSlaveFR & 2) == 0) {
		dummyRead(i2cSlave);
	}
	usleep(2000);  	// not tested! some delay maybe needed
	*i2cSlaveCR = 0;
	*i2cMasterS = 0x302;
	*i2cMaster = 10;
	// pinmode GPIO2,GPIO3=ALT0
	*gpio |= 0x900;
	// pinmode GPIO18,GPIO19=ALT3
	*(gpio + 1) |= 0x3f000000;

	usleep(1000);  	// not tested! some delay maybe needed


#ifdef debug
	if((SDA1IN == 0) || (SCL1IN == 0)) {
		printf("I2C Master might be stuck in transfer?\n");
		printf("FIFO=%x\n", *i2cMasterFIFO);
		printf("C   =%x\n", *i2cMaster);
		printf("S   =%x\n", *i2cMasterS);
		printf("DLEN=%x\n", *i2cMasterDLEN);
		printf("A   =%x\n", *i2cMasterA);
		printf("FIFO=%x\n", *i2cMasterFIFO);
		printf("DIV =%x\n", *i2cMasterDiv);
		printf("SDA=%x\n", SDA1IN);
		printf("SCL=%x\n", SCL1IN);
	}
	// pinmode GPIO17,GPIO27,GPIO22=output for debugging
	*(gpio + 1) = (*(gpio + 1) & 0xff1fffff) | 0x00200000;  	// GIO17
	*(gpio + 2) = (*(gpio + 2) & 0xff1fffff) | 0x00200000;  	// GIO27
	*(gpio + 2) = (*(gpio + 2) & 0xfffffe3f) | 0x00000040;  	// GIO22
#endif

	// set i2c slave control register to break and off
	*i2cSlaveCR = 0x80;
	// set i2c slave control register to enable: receive, i2c, device
	*i2cSlaveCR = 0x205;
	// set i2c slave address 0x00 to listen to broadcasts
	*i2cSlaveSLV = 0x0;
	// reset errors
	*i2cSlaveRSR = 0;

	// set i2c master to 100khz
//	if ( checkPiModel() == 3 )
//		*i2cMasterDiv = 0x1072;
//	else
		*i2cMasterDiv = 0x0a47;  	// 95khz works better
//		*i2cMasterDiv = 0x09c4;	// 100khz
//		*i2cMasterDiv = 0x0271;	// 400khz
}

// print hex values
void hexPrint(char bytes[], int length) {
	int i;

	for (i = 0; i < length; i++)
		printf("%02x ", bytes[i]);
	printf("\n");
}

// calculate checksum and put it in the last byte
char crc_calc(char *buffer) {
	int i;
	char crc_result = 0;
	char length = buffer[2] - 1;

	for (i = 0; i < length; i++)
		crc_result = crc_table[crc_result ^ buffer[i]];   // new CRC will be the CRC of (old CRC XORed with data byte) - see http://sbs-forum.org/marcom/dc2/20_crc-8_firmware_implementations.pdf

	if(buffer[i] == crc_result)
		return ERROR_OK;
	buffer[i] = crc_result;
	return ERROR_CRC;
}

// find out wich pi
int checkPiModel() {
	FILE *cpuFd;
	char line[120];

	if ((cpuFd = fopen("/proc/cpuinfo", "r")) == NULL)
#ifdef debug
		printf("Unable to open /proc/cpuinfo") 
#endif
		;

	// looking for the revision....
	while(fgets(line, 120, cpuFd) != NULL)
		if(strncmp(line, "Revision", 8) == 0) {
		// See if it's BCM2708 or BCM2709
		if(strstr(line, "a01041") != NULL || strstr(line, "a21041") != NULL) {
			fclose(cpuFd);
			return 2;	// PI 2
		} else if(strstr(line, "a02082") != NULL || strstr(line, "a22082") != NULL) {
			fclose(cpuFd);
			return 3;	// PI 3
		} else {
			fclose(cpuFd);
			return 1;	// PI B+
		}
	}
	fclose(cpuFd);
	return 0;
}