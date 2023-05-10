#ifndef WS_SPEEDSENDOR
#define WS_SPEEDSENDOR

#include <Arduino.h>
#include "common.h"
#include "filtering.h"
#include "speedmapper.h"

#define diff(newt, oldt) UDiffTime(newt, oldt)
#define second			 1000000		// microseconds in a second
#define minute			 60000000.0f	// microseconds in a minute
#define timeout			 (.3f * second)

class Tacho {
private:
	bool new_value;
	int pin_num;
	bool value;
	uint64_t time_of_change;
	uint64_t time_out;
	Filtered<uint64_t> time_ellapsed;

public:
	bool Diff(uint64_t new_time);
	bool GetValue();
	float GetRPM();
	uint64_t GetTimeEllapsed();
	uint64_t GetTimeEllapsedUntil(uint64_t until);
	Tacho(int _pin_num, uint64_t _time_out);
	~Tacho();
};

class WSSpeed : public SensorBase {
private:
	Tacho *t0;
	Tacho *t1;
	Tacho *t2;
	uint64_t time;
	float speed_buf[3] = { 0, 0, 0 };
	float speed_rpm;
	Speedmapper* mapper;

public:
	WSSpeed(Initializer* _init_parameters);
	~WSSpeed();
	int Handle();
	char* Serialize();
	char* SerializeJSON();
	void SetMapper(struct map* _mappings, int _mapsize, bool _deep_copy = true);
	void ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length);
};


#endif