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
    //sw_time.Stop();
    //gValue  = mpu->getGValues();
    //gyr = mpu->getGyrValues();
    temp = mpu->getTemperature();
    //resultantG = mpu->getResultantG(gValue);
    angles = mpu->getAngles();
    ori.roll = mpu->getRoll();
    ori.pitch = mpu->getPitch();

    // Copied from: https://github.com/natanaeljr/esp32-MPU-driver/blob/master/examples/mpu_real/main/mpu_real.cpp
    //constexpr double kRadToDeg = 57.2957795131;
    //constexpr float kDeltaTime = 1.f / kSampleRate;
    //float gyroRoll = roll + mpud::math::gyroDegPerSec(rawGyro.x, kGyroFS) * kDeltaTime;
    //float gyroPitch = pitch + mpud::math::gyroDegPerSec(rawGyro.y, kGyroFS) * kDeltaTime;
    //float gyroYaw = yaw + mpud::math::gyroDegPerSec(rawGyro.z, kGyroFS) * kDeltaTime;
    //float accelRoll = atan2(-rawAccel.x, rawAccel.z) * kRadToDeg;
    //float accelPitch = atan2(rawAccel.y, sqrt(rawAccel.x * rawAccel.x + rawAccel.z * rawAccel.z)) * kRadToDeg;
    //// Fusion
    //roll = gyroRoll * 0.95f + accelRoll * 0.05f;
    //pitch = gyroPitch * 0.95f + accelPitch * 0.05f;
    //yaw = gyroYaw;


    //dt = sw_time.EllapsedS();
    //dprint("dt: %fs", dt);
    //if (dt < w_timeout_s) {
    //    gyro.roll = ori.roll + deg_per_sec(gyr.x) * dt;
    //    gyro.pitch = ori.pitch + deg_per_sec(gyr.y) * dt;
    //    gyro.yaw = ori.yaw + deg_per_sec(gyr.z) * dt;
    //    accel.roll = atan2(-gValue.x, gValue.z) * RAD_TO_DEG;
    //    accel.pitch = atan2(gValue.x, sqrt(gValue.x * gValue.x + gValue.z * gValue.z)) * RAD_TO_DEG;
    //    
    //    ori.roll = gyro.roll * 0.95f + accel.roll * 0.05f;
    //    ori.pitch = gyro.pitch * 0.95f + accel.pitch * 0.05f;
    //    ori.yaw = gyro.yaw;
    //    dprint("grpy[%f %f %f] acc[%f %f] orient[%f %f %f]",
    //        gyro.roll, gyro.pitch, gyro.yaw, accel.roll, accel.pitch, ori.roll, ori.pitch, ori.yaw);
    //}

    //sw_time.Start();
    return HANDLER_OK;
}

char* WSMPU::Serialize() {
    //buflen = sprintf(serial_buffer, "[%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %6.2f], temp: %6.2fdegC",
    //    gValue.x, gValue.y, gValue.z, gyr.x, gyr.y, gyr.z, resultantG, temp);
    //serial_buffer[buflen] = 0;
    serial_buffer[0] = 0;
    return serial_buffer;
}

char* WSMPU::SerializeJSON() {
    //buflen = sprintf(serial_buffer, "{\"acc\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f],\"temp\":%.2f,\"resultantG\":%.2f}",
    //    gValue.x, gValue.y, gValue.z, gyr.x, gyr.y, gyr.z, temp, resultantG);
    //serial_buffer[buflen] = 0;
    serial_buffer[0] = 0;
    return serial_buffer;
}

struct buf* WSMPU::SerializeBLE() {
    wbuf.temp = temp;
    wbuf.angles.x = angles.x;
    wbuf.angles.z = angles.y;
    wbuf.angles.y = angles.z;
    wbuf.orient.roll  = ori.roll;
    wbuf.orient.pitch = ori.pitch;
    //wbuf.orient.yaw   = ori.yaw;
    //wbuf.gval.x = gValue.x;
    //wbuf.gval.y = gValue.y;
    //wbuf.gval.z = gValue.z;
    //wbuf.gyr.x = gyr.x;
    //wbuf.gyr.y = gyr.y;
    //wbuf.gyr.z = gyr.z;
    //wbuf.resg = resultantG;

    sbuf.len = sizeof(struct mpubuf);
    sbuf.buf = &wbuf;

    return &sbuf;
}

void WSMPU::ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length) {
    mpu->autoOffsets();

    // Write to EEPROM
    //init_parameters->wane_offset = GetCalibratedOffset();
    //EEPROM.put(EEPROM_LOC_DATA, *init_parameters);
    //EEPROM.commit();
}
