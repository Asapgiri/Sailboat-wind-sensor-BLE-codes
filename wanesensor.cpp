#include "ws_config.h"
#include "wanesensor.h"
#include "filtering.cpp" // c compiler and arduino is garbage with templates

WSWane::WSWane(Initializer* _init_parameters) : SensorBase(_init_parameters) {
    i2c_wire = new TwoWire(WANE_I2C_WIRE);
    i2c_wire->begin(WANE_I2C_SDA, WANE_I2C_SCL);

    as_sensor = new AS5601(i2c_wire);

//    value_adc = new Filtered<uint32_t>();
    value_deg = 0.0f;
    CalibrateToRaw(init_parameters->GetWaneOffset());
}

WSWane::~WSWane() {
//    delete value_adc;
    delete as_sensor;
    delete i2c_wire;
}

void WSWane::CalibrateToRaw(uint16_t raw_pos) {
    offset = raw_pos;
    as_sensor->setZeroPosition(offset);
}

void WSWane::CalibrateToCurrentPos() {
    offset = as_sensor->getRawAngle();
    as_sensor->setZeroPosition(offset);
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

// Data should be raw uint16_t or empty...
void WSWane::ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length) {
    uint16_t raw;
    switch (cmd)
    {
    case CMD_SET_WANE_TO_OFFSET:
        raw = *((uint16_t*)buffer);
        dprint("Calibrate to %u", raw);
        CalibrateToRaw(raw);
        break;
    case CMD_SET_WANE_TO_CORRENT:
        CalibrateToCurrentPos();
        break;

    default:
        break;
    }
    dprint("Calibrated to %u", GetCalibratedOffset());

    // Write to EEPROM
    init_parameters->SetWaneOffset(GetCalibratedOffset());
    init_parameters->Save();
}
