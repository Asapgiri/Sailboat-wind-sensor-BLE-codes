#include "controller.h"

/////////////////////////////////////////////////////////////////////////////////////
/// BLE
/////////////////////////////////////////////////////////////////////////////////////

BLECallbacks::BLECallbacks(Controller* _parent) : BLECharacteristicCallbacks() {
    parent = _parent;
}

void BLECallbacks::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

#ifdef DEBUG
    if (value.length() > 0) {
        Serial.println("=========================================");
        Serial.print("Incoming value: ");
        for (int i = 0; i < value.length(); i++)
            Serial.printf("%02x", (uint8_t)value[i]);

        Serial.println();
        Serial.println("=========================================");
    }
#endif

    // Handling incoming command ...
    // 
    // Command structure:
    // [command:uint8_t]#data ...
    parent->ExecuteCommand(value);

    dprint();
}


/////////////////////////////////////////////////////////////////////////////////////
/// Controller
/////////////////////////////////////////////////////////////////////////////////////

/// Private

void Controller::ExecuteCommand(std::string buf) {
    int i;
    uint8_t cmd;

    if (COMMAND_SEPARATOR != buf[1]) {
        derrprint("Failed to execure command: No separator!");
        return;
    }

    cmd = (uint8_t)buf[CMD_CMD_LOC];
    for (i = 0; i < COMMAND_ARRAY_SIZE; i++) {
        if (commands[i].cmd == cmd) {
            break;
        }
    }

    if (nullptr != commands[i].executer) {
        commands[i].executer->ExecuteCommand(cmd, buf.c_str() + CMD_DAT_LOC, buf.length() - CMD_DAT_LOC + 1);
    }
}

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
    dprint("Service uuid: %s\n", ble_service->getUUID().toString().c_str());

    ble_callbacks = new BLECallbacks(this);
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

int Controller::CommandAdd(uint8_t _cmd, SensorBase* _executer) {
    if (command_count >= COMMAND_ARRAY_SIZE) {
        derrprint("Commands array is full!!");
        return 0;
    }
    
    commands[command_count].cmd      = _cmd;
    commands[command_count].executer = _executer;
    command_count++;

    return 1;
}

int Controller::CommandsInit() {
    uint8_t success_count = 0;

    command_count = 0;
    success_count += CommandAdd(CMD_PING, nullptr);
    success_count += CommandAdd(CMD_SET_FILTER_WINDOW, nullptr);
    success_count += CommandAdd(CMD_SET_WANE_TO_OFFSET, wane_sensor);
    success_count += CommandAdd(CMD_SET_WANE_TO_CORRENT, wane_sensor);
    success_count += CommandAdd(CMD_MPU_CALIBRATE, mpu_sensor);
    success_count += CommandAdd(CMD_SET_SPEEDMAP, speed_sensor);

    dprint("Success with: %u", success_count);

    return success_count;
}


/// Public

Controller::Controller() {
    struct map* wspeed_map;

    init_parameters.Load();
    init_parameters.Print();

    wane_sensor  = new WSWane(&init_parameters);
    speed_sensor = new WSSpeed(&init_parameters);
    mpu_sensor   = new WSMPU(&init_parameters);

    wspeed_map = new struct map[init_parameters.GetSpeedmapSize()];
    init_parameters.Load(wspeed_map);

    speed_sensor->SetMapper(wspeed_map, init_parameters.GetSpeedmapSize());

    delete[] wspeed_map;

    // Initialize filters
    //dprint("Initializeing filters.");
    //for (int i = 0; i < FILTER_WINDOW_SIZE; i++) {
    //    this->MeasureHandler();
    //    delay(10);
    //}

    CommandsInit();

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
    ble_count = ble_count_temp;

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
