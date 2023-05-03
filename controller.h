#ifndef WS_CONTROLLER
#define WS_CONTROLLER

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "ws_config.h"
#include "wanesensor.h"
#include "speedsensor.h"
#include "mpusensor.h"

class BLECallbacks : public BLECharacteristicCallbacks {
public:
	void onWrite(BLECharacteristic* pCharacteristic);
};

class Controller {
private:
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

#ifdef DEBUG
	char serial_buffer[SERIAL_BUF_SIZE << 2];
	int  buflen = 0;
#endif

public:
	Controller();
	~Controller();
	void MeasureHandler();
	void HandleBLE();
#ifdef DEBUG
	char* Serialize();
#endif
};


#endif