#ifndef WS_MPUSENDOR
#define WS_MPUSENDOR

#include "common.h"
#include <MPU6500_WE.h>
#include <Arduino.h>

#define sample_rate_per_sec	250.f
#define sample_resolution	(1 / sample_rate_per_sec)
#define deg_per_sec(vari)	(vari * sample_resolution)

struct fxyz {
	float x;
	float y;
	float z;
};

struct frpy {
	float roll;
	float pitch;
	float yaw;
};

struct mpubuf {
	float temp;
	struct fxyz angles;
	struct frpy orient;
	//struct fxyz gval;
	//struct fxyz gyr;
	//float resg;
};

class WSMPU : public SensorBase {
private:
	struct buf    sbuf;
	struct mpubuf wbuf;
	MPU6500_WE* mpu;
	//xyzFloat gValue;
	//xyzFloat gyr;
	float temp;
	//float resultantG;
	//StopWatch sw_time;
	//float dt;
	struct frpy ori = { 0 };
	//struct frpy gyro;
	//struct frpy accel;
	xyzFloat angles;

public:
	WSMPU(Initializer* _init_parameters);
	~WSMPU();
	int Handle();
	char* Serialize();
	char* SerializeJSON();
	struct buf* SerializeBLE();
	void ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length);
};


#endif