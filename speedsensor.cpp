#include <stdio.h>
#include "ws_config.h"
#include "speedsensor.h"
#include "common.h"
#include "filtering.cpp" // c compiler and arduino is garbage with templates

/////////////////////////////////////////////////////////////////////////////////////
/// Tacho
/////////////////////////////////////////////////////////////////////////////////////

Tacho::Tacho(int _pin_num, uint64_t _time_out) {
	dprint("pin %d", _pin_num);
	pin_num = _pin_num;
	time_out = _time_out;
	pinMode(pin_num, INPUT);
	value = 0;
//	time_ellapsed = new Filtered<uint64_t>();
	time_of_change = 0;
}

Tacho::~Tacho() {
//	delete time_ellapsed;
}

bool Tacho::Diff(uint64_t new_time) {
	new_value = digitalRead(pin_num);

	if (new_value != value) {	// only measure once
		value = new_value;

		if (!value) {
			time_ellapsed.Add(diff(new_time, time_of_change));
			time_of_change = new_time;
			return true;
		}
	}

	return false;
}

bool Tacho::GetValue() {
	return value;
}

struct vfloat Tacho::GetRPM() {
	uint64_t time = micros();
	struct vfloat rpm = { 0.0f, false };

	if (diff(time, time_of_change) < time_out) {
		rpm.value = w_minute / (float)time_ellapsed.GetFilteredValut();
		rpm.is_valid = true;
	}

	return rpm;
}

uint64_t Tacho::GetTimeEllapsed() {
	return time_ellapsed.GetFilteredValut();
}

uint64_t Tacho::GetTimeEllapsedUntil(uint64_t until) {
	return diff(until, time_of_change);
}


/////////////////////////////////////////////////////////////////////////////////////
/// WSSpeed
/////////////////////////////////////////////////////////////////////////////////////

WSSpeed::WSSpeed(Initializer* _init_parameters) : SensorBase(_init_parameters) {
	dprint("");
	t0 = new Tacho(SS_TACHO_0, w_timeout);
	t1 = new Tacho(SS_TACHO_1, w_timeout);
	t2 = new Tacho(SS_TACHO_2, w_timeout);

	// TODO: Init from EEPROM
	struct map smap = { 0.0f, 0.1f };
	mapper = new Speedmapper(&smap, 1);

	time	  = micros();
	speed_rpm = 0.0f;
}

WSSpeed::~WSSpeed() {
	delete t0;
	delete t1;
	delete t2;
}

int WSSpeed::Handle() {
	time = micros();
	
	t0->Diff(time);
	t1->Diff(time);
	t2->Diff(time);

	speed_buf[0] = t0->GetRPM();
	speed_buf[1] = t1->GetRPM();
	speed_buf[2] = t2->GetRPM();

	// TODO: Make error correction if one of the buffers are waay off...
	//		 This is ignored for the time beeing..
	//dprint("Read rpm[%6.2f %6.2f %6.2f]: ", speed_buf[0], speed_buf[1], speed_buf[2]);
	
	sum = 0;
	cnt = 0;
	for (i = 0; i < 3; i++) {
		if (speed_buf[i].is_valid) {
			sum += speed_buf[i].value;
			cnt++;
		}
	}
	if (cnt) {
		speed_rpm = sum / cnt;
	}
	else {
		speed_rpm = 0.0f;
	}
	//dprint("%6.2f.\n", speed_rpm);

	return HANDLER_OK;
}

/*
* TODO: return times or etc...
*/
char* WSSpeed::Serialize() {
	buflen = sprintf(serial_buffer, "0b%u%u%u: rpm: %6.2f, kmh: %6.2f", t0->GetValue(), t1->GetValue(), t2->GetValue(), speed_rpm, mapper->Map(speed_rpm));
	serial_buffer[buflen] = 0;
	return serial_buffer;
}

char* WSSpeed::SerializeJSON() {
	buflen = sprintf(serial_buffer, "{\"rpm\":%6.2f,\"wind_speed\":%6.2f}", speed_rpm, mapper->Map(speed_rpm));
	serial_buffer[buflen] = 0;
	return serial_buffer;
}

struct buf* WSSpeed::SerializeBLE() {
	wbuf.pinups = t0->GetValue();
	wbuf.pinups |= t1->GetValue() << 1;
	wbuf.pinups |= t2->GetValue() << 2;
	wbuf.rpm = speed_rpm;
	wbuf.kmh = mapper->Map(speed_rpm);

	sbuf.len = sizeof(struct speedbuf);
	sbuf.buf = &wbuf;

	return &sbuf;
}

void WSSpeed::SetMapper(struct map* _mappings, int _mapsize, bool _deep_copy) {
	delete mapper;
	mapper = new Speedmapper(_mappings, _mapsize, _deep_copy);
}

// Data should be [mapsize:uint32_t][map[mapsize]:map]
void WSSpeed::ExecuteCommand(uint8_t cmd, const char* buffer, uint32_t length) {
	uint16_t mapsize = *((uint16_t*)buffer);
	struct map* maps = (struct map*)(buffer + sizeof(mapsize));

	if ((sizeof(mapsize) + mapsize * sizeof(struct map)) > length) {
		dprint("Expected lapsize is larger than buffer. (%u - %u)", sizeof(mapsize) + mapsize * sizeof(struct map), length);
		return;
	}

#ifdef DEBUG
	for (int i = 0; i < mapsize; i++) {
		dprint("Got value: [%.2f %.2f]", maps[i].limit, maps[i].coeffitiant);
	}
#endif

	// TODO: Also set from text input?? Is it really needed?
	SetMapper(maps, mapsize);

	// Write to EEPROM
	init_parameters->SetSpeedmapSize(mapsize);
	init_parameters->Save();
	init_parameters->Save(maps);
}
