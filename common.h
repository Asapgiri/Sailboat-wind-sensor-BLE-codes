#ifndef WS_COMMON
#define WS_COMMON

#include <Arduino.h>
#include "ws_config.h"

#define HANDLER_OK	  0
#define COMMON_ERROR -1

#ifdef DEBUG
#define dprint(str, ...) Serial.printf("\n%s: " str, __PRETTY_FUNCTION__, ##__VA_ARGS__ )
#define derrprint(str, ...) Serial.printf("\n[ERROR]:: %s: " str "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__ )
//#define dgettime()		 ESP.getCycleCount()
#define dgettime()		 micros()
#else
#define dprint(str, ...)
#define derrprint(str, ...)
#define dgettime()
#endif


typedef struct initer {
	uint16_t filer_window_size;
	uint16_t wane_offset;
	float	 mpu_acc_offset[6];
	float	 mpu_gyr_offset[3];
	uint16_t wspeed_map_size;
} initializer;

class SensorBase
{
protected:
	initializer* init_parameters;
	char serial_buffer[SERIAL_BUF_SIZE];
	int  buflen = 0;
public:
	SensorBase(initializer* _init_parameters) { init_parameters = _init_parameters; }
	virtual char* Serialize() = 0;
	virtual char* SerializeJSON() = 0;
	virtual int Handle() = 0;
	virtual void ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length) = 0;
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