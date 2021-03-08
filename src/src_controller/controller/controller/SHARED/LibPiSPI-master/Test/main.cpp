#include <PiSPI/PiSPI.h>
#include <wiringPiSPI.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

int main(void)
{
	uint8_t buffer[10] = { 0 };
	PiSPI piSPI(0, 10000000, 0);
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		buffer[0] = 0x80;
		piSPI.Read(0x80, &buffer[0], sizeof(buffer));
		this_thread::sleep_for(chrono::seconds(1));
	}
	return 0;
}