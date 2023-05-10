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
	WSMPU(initializer* _init_parameters);
	~WSMPU();
	int Handle();
	char* Serialize();
	char* SerializeJSON();
	void ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length);
};


#endif