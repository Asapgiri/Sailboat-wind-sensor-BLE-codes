#include <EEPROM.h>
#include "common.h"
#include "speedmapper.h"

template <class T>
static void print_array(T* array, int size, const char* line_start, const char* line_end) {
    int i;
    for (i = 0; i < size; i++) {
        Serial.print(line_start);
        Serial.print(array[i]);
        Serial.print(line_end);
    }
}

Initializer::Initializer() {
    struct map* wspeed_map;
    int i, map_location;

    EEPROM.begin(EEPROM_SIZE);

    if (!WaterMarkExists()) {
        dprint("EEPROM FIRST INIT\n");

        EEPROM.put(EEPROM_LOC_WM, (uint32_t)EEPROM_WATERMARK);

        inits.filer_window_size = FILTER_WINDOW_SIZE;
        inits.wane_offset = 0;

        inits.mpu_acc_offset[0];

        inits.wspeedmap_size = 1;
        wspeed_map = new struct map[inits.wspeedmap_size];
        map_location = EEPROM_LOC_DATA + sizeof(inits);

        EEPROM.put(EEPROM_LOC_DATA, inits);
        for (i = 0; i < inits.wspeedmap_size; i++) {
            wspeed_map[i].limit = 0;
            wspeed_map[i].coeffitiant = 1;
            EEPROM.put(map_location + i, wspeed_map[i]);
        }
        delete[] wspeed_map;

        EEPROM.commit();
    }
}

bool Initializer::WaterMarkExists() {
    uint32_t wm;
    EEPROM.get(EEPROM_LOC_WM, wm);

    if (EEPROM_WATERMARK == wm) {
        return true;
    }
    else {
        return false;
    }
}

void Initializer::Load(struct map* map) {
    int i, map_location;

    if (nullptr == map) {
        EEPROM.get(EEPROM_LOC_DATA, inits);
    }
    else {
        map_location = EEPROM_LOC_DATA + sizeof(inits);
        for (i = 0; i < inits.wspeedmap_size; i++) {
            EEPROM.get(map_location + i, map[i]);
        }
    }
}

void Initializer::Save(struct map* map) {
    int i, map_location;

    if (nullptr == map) {
        EEPROM.put(EEPROM_LOC_DATA, inits);
    }
    else {
        map_location = EEPROM_LOC_DATA + sizeof(inits);
        for (i = 0; i < inits.wspeedmap_size; i++) {
            EEPROM.put(map_location + i, map[i]);
        }
    }
    EEPROM.commit();
}

void Initializer::Print() {
    struct map* wspeed_map;
    int i, map_location;

    wspeed_map = new struct map[inits.wspeedmap_size];
    map_location = EEPROM_LOC_DATA + sizeof(inits);

    for (i = 0; i < inits.wspeedmap_size; i++) {
        EEPROM.get(map_location + i, wspeed_map[i]);
    }

    Serial.println("=========================================");
    Serial.println("Calibration parameters:");
    Serial.println("=========================================");
    Serial.printf("Filter window:  [%u]\n", inits.filer_window_size);
    Serial.printf("Wane offset:    [%u]\n", inits.wane_offset);
    Serial.println("MPU acc offset: [");
    print_array(inits.mpu_acc_offset, 6, "                   ", "\n");
    Serial.println("                ]");
    Serial.println("MPU gyr offset: [");
    print_array(inits.mpu_gyr_offset, 3, "                   ", "\n");
    Serial.println("                ]");
    Serial.println("Speedmap: [");
    for (i = 0; i < inits.wspeedmap_size; i++) {
        Serial.print("             ");
        Serial.print(wspeed_map[i].limit);
        Serial.print(" ");
        Serial.println(wspeed_map[i].coeffitiant);
    }
    Serial.println("          ]");
    Serial.println("=========================================");

    delete[] wspeed_map;
}


/// Getters / Setters

uint16_t Initializer::GetFilterWindowSize() {
    return inits.filer_window_size;
}

uint16_t Initializer::GetWaneOffset() {
    return inits.wane_offset;
}

uint16_t Initializer::GetSpeedmapSize() {
    return inits.wspeedmap_size;
}


void Initializer::SetFilterWindowSize(uint16_t value) {
    inits.filer_window_size = value;
}

void Initializer::SetWaneOffset(uint16_t value) {
    inits.wane_offset = value;
}

void Initializer::SetSpeedmapSize(uint16_t value) {
    inits.wspeedmap_size = value;
}



void StopWatch::Start() {
    start = micros();
    running = true;
}

void StopWatch::Stop() {
    end = micros();
    running = false;
}

uint64_t StopWatch::EllapsedUS() {
    if (running) {
        diff = UDiffTime(micros(), start);
    }
    else {
        diff = UDiffTime(end, start);
    }
    return diff;
}

float StopWatch::EllapsedS() {
    EllapsedUS();
    return (float)diff / (float)w_second;
}
