#ifndef WS_COMMON
#define WS_COMMON

#include <Arduino.h>
#include "ws_config.h"

#define HANDLER_OK	  0
#define COMMON_ERROR -1

#ifdef DEBUG
#define dprint(str, ...) Serial.printf("\n%s: " str, __PRETTY_FUNCTION__, ##__VA_ARGS__ )
//#define dgettime()		 ESP.getCycleCount()
#define dgettime()		 micros()
#else
#define dprint(str, ...)
#define dgettime()
#endif


class SensorBase
{
protected:
	char serial_buffer[SERIAL_BUF_SIZE];
	int  buflen = 0;
public:
	virtual char* Serialize() = 0;
	virtual char* SerializeJSON() = 0;
	virtual int Handle() = 0;
};

static uint64_t UDiffTime(uint64_t newTime, uint64_t oldTime) {
	uint64_t diff;
	
	if (newTime > oldTime) {
		diff = newTime - oldTime;
	}
	else {
		diff = ~((uint64_t)0) - oldTime + newTime;
	}

	return diff;
}


#endif