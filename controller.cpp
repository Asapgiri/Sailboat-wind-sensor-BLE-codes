#include "controller.h"

/////////////////////////////////////////////////////////////////////////////////////
/// BLE
/////////////////////////////////////////////////////////////////////////////////////

void BLECallbacks::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

#ifdef DEBUG
    if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
            Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
    }
#endif
}


/////////////////////////////////////////////////////////////////////////////////////
/// Controller
/////////////////////////////////////////////////////////////////////////////////////

/// Private

void Controller::BLEInit() {
    dprint("\n");
    BLEDevice::init(BLE_DEVICE_NAME);
    BLEDevice::setMTU(SERIAL_BUF_SIZE); // set up ble buffer size ...
    ble_server = BLEDevice::createServer();
    ble_service = ble_server->createService(BLE_SERVICE_UUID);
    ble_characteristic = ble_service->createCharacteristic(
        BLE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );
    dprint("Service uuid: %s\n", ble_service->getUUID().toString());

    ble_callbacks = new BLECallbacks();
    ble_characteristic->setCallbacks(ble_callbacks);
    ble_characteristic->setValue("{}");

    ble_service->start();

    ble_advertising = ble_server->getAdvertising();
    ble_advertising->start();

    ble_count = 0;
    ble_count_temp = 0;
}

void Controller::BLEReInit() {
    BLERemove();
    BLEInit();
}

void Controller::BLERemove() {
    ble_advertising->stop();
    ble_service->stop();
    BLEDevice::deinit(false);
    delete ble_callbacks;
}


/// Public

Controller::Controller() {
    wane_sensor = new WSWane();
    speed_sensor = new WSSpeed();
    mpu_sensor = new WSMPU();

    // Initialize filters
    dprint("Initializeing filters.");
    for (int i = 0; i < FILTER_WINDOW_SIZE; i++) {
        this->MeasureHandler();
        delay(10);
    }

    BLEInit();
}

Controller::~Controller() {
    BLERemove();

    delete wane_sensor;
    delete speed_sensor;
    delete mpu_sensor;
}

void Controller::MeasureHandler() {
    wane_sensor->Handle();
    speed_sensor->Handle();
    mpu_sensor->Handle();
}

void Controller::HandleBLE() {
    ble_count_temp = ble_server->getConnectedCount();
    if (ble_count > ble_count_temp) {
        dprint("Reinit...\n");
        BLEReInit();
    }
    else {
        ble_count = ble_count_temp;
    }

    if (ble_count > 0) {
        char* speedbuf = speed_sensor->SerializeJSON();
        char* wanebuf = wane_sensor->SerializeJSON();
        char* mpubuf = mpu_sensor->SerializeJSON();

        buflen = sprintf(serial_buffer, "{\"speed\":%s,\"wane\":%s,\"mpu\":%s}", speedbuf, wanebuf, mpubuf);
        serial_buffer[buflen] = 0;

        //Serial.println(serial_buffer);
        ble_characteristic->setValue(serial_buffer);
        ble_characteristic->notify();
    }
}

#ifdef DEBUG
char* Controller::Serialize() {
    char* speedbuf = speed_sensor->Serialize();
    char* wanebuf  = wane_sensor->Serialize();
    char* mpubuf   = mpu_sensor->Serialize();

    buflen = sprintf(serial_buffer, "wane: (%s) -- mpu: (%s) -- wind speed: (%s)", wanebuf, mpubuf, speedbuf);
    serial_buffer[buflen] = 0;

    return serial_buffer;
}
#endif
