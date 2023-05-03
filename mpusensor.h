#ifndef WS_MPUSENDOR
#define WS_MPUSENDOR

#include "common.h"
#include <MPU6500_WE.h>
#include <Arduino.h>

class WSMPU : public SensorBase {
private:
	MPU6500_WE* mpu;
	xyzFloat gValue;
	xyzFloat gyr;
	float temp;
	float resultantG;

public:
	WSMPU();
	~WSMPU();
	int Handle();
	char* Serialize();
	char* SerializeJSON();
};


#endif