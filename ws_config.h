#ifndef WS_PINCONFIG
#define WS_PINCONFIG

#define DEBUG
#define DPRINT_TIMEOUT  1000
#define DTIME_UNIT      "us"

/////////////////////////////////////////////////////////////////////////////////////
/// Config
/////////////////////////////////////////////////////////////////////////////////////

#define BLE_DEVICE_NAME         "WS_TEST_ESP"
#define BLE_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define WANE_I2C_WIRE	    0
#define WANE_MAX_ANGLE_ADC	0x1000   // 4096, cannot reach.
#define WANE_MAX_ANGLE_DEG  360

#define SS_ANGLES_DEG	90

#define MPU_NUM	2

#define U_SEC_PER_SEC	    1000000
#define SERIAL_BUF_SIZE     256
#define FILTER_WINDOW_SIZE  5



/////////////////////////////////////////////////////////////////////////////////////
/// Pin config
/////////////////////////////////////////////////////////////////////////////////////

#define WANE_I2C_SDA	18
#define WANE_I2C_SCL	19

#define SS_TACHO_0	5
#define SS_TACHO_1	8
#define SS_TACHO_2	9

#define MPU_SPI_MOSI	7
#define MPU_SPI_MISO	2
#define MPU_SPI_CLK		6
#define MPU_SPI_CS		10
#define MPU_USE_SPI     true



#endif