#ifndef WS_SPEEDSENDOR
#define WS_SPEEDSENDOR

#include <Arduino.h>
#include "common.h"
#include "filtering.h"
#include "speedmapper.h"

struct speedbuf {
	uint32_t pinups;
	float    rpm;
	float    kmh;
};

struct vfloat {
	float value;
	bool  is_valid;
};

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
	struct vfloat GetRPM();
	uint64_t GetTimeEllapsed();
	uint64_t GetTimeEllapsedUntil(uint64_t until);
	Tacho(int _pin_num, uint64_t _time_out);
	~Tacho();
};

class WSSpeed : public SensorBase {
private:
	struct buf      sbuf;
	struct speedbuf wbuf;
	Tacho *t0;
	Tacho *t1;
	Tacho *t2;
	uint64_t time;
	struct vfloat speed_buf[3] = { {0.0f,false}, {0.0f,false}, {0.0f,false} };
	float speed_rpm;
	float sum;
	int i, cnt;
	Speedmapper* mapper;

public:
	WSSpeed(Initializer* _init_parameters);
	~WSSpeed();
	int Handle();
	char* Serialize();
	char* SerializeJSON();
	struct buf* SerializeBLE();
	void SetMapper(struct map* _mappings, int _mapsize, bool _deep_copy = true);
	void ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length);
};


#endif