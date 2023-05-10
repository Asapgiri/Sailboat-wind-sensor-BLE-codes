#include "ws_config.h"
#include "mpusensor.h"

WSMPU::WSMPU(Initializer* _init_parameters) : SensorBase(_init_parameters) {
    SPI.begin(MPU_SPI_CLK, MPU_SPI_MISO, MPU_SPI_MOSI, MPU_SPI_CS);
    mpu = new MPU6500_WE(&SPI, MPU_SPI_CS, MPU_USE_SPI);

    while (!mpu->init()) {
        if (!Serial) {
            Serial.begin(115200);
            while (!Serial) {}
        }
        Serial.println("MPU init error!!");
        delay(1000);
    }

    // TODO: Load from EEPROM, and proper setup ...
    mpu->autoOffsets();
    //mpu->setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);
    //mpu->setGyrOffsets(45.0, 145.0, -105.0);

    mpu->enableGyrDLPF();
    //mpu->disableGyrDLPF(MPU6500_BW_WO_DLPF_8800); // bandwdith without DLPF
    mpu->setGyrDLPF(MPU6500_DLPF_6);
    mpu->setSampleRateDivider(5);
    mpu->setGyrRange(MPU6500_GYRO_RANGE_250);
    mpu->setAccRange(MPU6500_ACC_RANGE_2G);
    mpu->enableAccDLPF(true);
    mpu->setAccDLPF(MPU6500_DLPF_6);
    //mpu->enableAccAxes(MPU6500_ENABLE_XYZ);
    //mpu->enableGyrAxes(MPU6500_ENABLE_XYZ);
}

WSMPU::~WSMPU() {
    delete mpu;
}

int WSMPU::Handle() {
    gValue  = mpu->getGValues();
    gyr = mpu->getGyrValues();
    temp = mpu->getTemperature();
    resultantG = mpu->getResultantG(gValue);

    return HANDLER_OK;
}

char* WSMPU::Serialize() {
    buflen = sprintf(serial_buffer, "[%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %6.2f], temp: %6.2fdegC",
        gValue.x, gValue.y, gValue.z, gyr.x, gyr.y, gyr.z, resultantG, temp);
    serial_buffer[buflen] = 0;
    return serial_buffer;
}

char* WSMPU::SerializeJSON() {
    buflen = sprintf(serial_buffer, "{\"acc\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f],\"temp\":%.2f,\"resultantG\":%.2f}",
        gValue.x, gValue.y, gValue.z, gyr.x, gyr.y, gyr.z, temp, resultantG);
    serial_buffer[buflen] = 0;
    return serial_buffer;
}

void WSMPU::ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length) {
    mpu->autoOffsets();

    // Write to EEPROM
    //init_parameters->wane_offset = GetCalibratedOffset();
    //EEPROM.put(EEPROM_LOC_DATA, *init_parameters);
    //EEPROM.commit();
}
