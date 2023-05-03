#include "ws_config.h"
#include "wanesensor.h"
#include "filtering.cpp" // c compiler and arduino is garbage with templates

WSWane::WSWane() {
    i2c_wire = new TwoWire(WANE_I2C_WIRE);
    i2c_wire->begin(WANE_I2C_SDA, WANE_I2C_SCL);

    as_sensor = new AS5601(i2c_wire);

//    value_adc = new Filtered<uint32_t>();
    value_deg = 0.0f;
    offset = 0;
}

WSWane::~WSWane() {
//    delete value_adc;
    delete as_sensor;
    delete i2c_wire;
}

void WSWane::CalibrateToRaw(uint16_t raw_pos) {
    offset = raw_pos;
    as_sensor->setZeroPosition(raw_pos);
}

void WSWane::CalibrateToCurrentPos() {
    as_sensor->setZeroPosition();
}

uint16_t WSWane::GetCalibratedOffset() {
    return offset;
}

int WSWane::Handle() {
    value_adc.Add(as_sensor->getAngle());

    value_deg = (float)(value_adc.GetFilteredValut() * WANE_MAX_ANGLE_DEG) / (float)(WANE_MAX_ANGLE_ADC);

	return HANDLER_OK;
}

char* WSWane::Serialize() {
    buflen = sprintf(serial_buffer, "adc: %4u, deg: %6.2f", value_adc.GetFilteredValut(), value_deg);
    serial_buffer[buflen] = 0;
    return serial_buffer;
}

char* WSWane::SerializeJSON() {
    buflen = sprintf(serial_buffer, "{\"adc\":%u,\"deg\":%.2f}", value_adc.GetFilteredValut(), value_deg);
    serial_buffer[buflen] = 0;
    return serial_buffer;
}
