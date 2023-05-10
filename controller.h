#ifndef WS_CONTROLLER
#define WS_CONTROLLER

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "ws_config.h"
#include "wanesensor.h"
#include "speedsensor.h"
#include "mpusensor.h"

typedef struct cmd {
	uint8_t cmd;
	SensorBase* executer;
} command;

class Controller;

class BLECallbacks : public BLECharacteristicCallbacks {
	Controller* parent;
public:
	BLECallbacks(Controller* _parent);
	void onWrite(BLECharacteristic* pCharacteristic);
};

class Controller {
private:
	initializer init_parameters;
	command commands[COMMAND_ARRAY_SIZE];
	uint8_t command_count;

	WSWane  *wane_sensor;
	WSSpeed *speed_sensor;
	WSMPU   *mpu_sensor;
	BLEServer*			ble_server;
	BLEService*			ble_service;
	BLECharacteristic*	ble_characteristic;
	BLEAdvertising*		ble_advertising;
	BLECallbacks*		ble_callbacks;

	uint32_t ble_count;
	uint32_t ble_count_temp;
	void BLEInit();
	void BLEReInit();
	void BLERemove();

	bool EEPROMCheckWaterMark();
	//void ReadInitParameters();
	//void WriteInitParameters();

	int CommandAdd(uint8_t _cmd, SensorBase* _executer);
	int CommandsInit();

	char serial_buffer[SERIAL_BUF_SIZE << 2];
	int  buflen = 0;

public:
	Controller();
	~Controller();
	void MeasureHandler();
	void HandleBLE();
	void PrintInitParameters();

	void ExecuteCommand(std::string buf);

#ifdef DEBUG
	char* Serialize();
#endif
};


#endif