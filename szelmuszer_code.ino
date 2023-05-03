#include "ws_config.h"
#include "common.h"
#include "controller.h"

Controller *controller;

#ifdef DEBUG
uint64_t timeing;
uint64_t loop_start;
uint64_t loop_end;
uint64_t handler_loop_start;
uint64_t handler_loop_end;
uint64_t ble_loop_start;
uint64_t ble_loop_end;
uint64_t serialize_start;
uint64_t serialize_end;
#endif

void setup() {
    // put your setup code here, to run once:
#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("\nStarting program...");
    timeing = millis();
#endif

    controller = new Controller();
}

void loop() {
    // put your main code here, to run repeatedly:
#ifdef DEBUG
    loop_start = dgettime();
    handler_loop_start = dgettime();
    controller->MeasureHandler();
    handler_loop_end = dgettime();
    ble_loop_start = dgettime();
    controller->HandleBLE();
    ble_loop_end = dgettime();
    loop_end = dgettime();
#else
    controller->MeasureHandler();
    controller->HandleBLE();
#endif

#ifdef DEBUG
    if ((timeing + DPRINT_TIMEOUT) < millis()) {
        Serial.print("Measurements: ");
        serialize_start = dgettime();
        Serial.print(controller->Serialize());
        serialize_end = dgettime();
        Serial.printf(" | loop time: %llu" DTIME_UNIT " (%llu" DTIME_UNIT "/%llu" DTIME_UNIT ") | serialize: %llu" DTIME_UNIT "\n",
            UDiffTime(loop_end, loop_start),
            UDiffTime(handler_loop_end, handler_loop_start),
            UDiffTime(ble_loop_end, ble_loop_start),
            UDiffTime(serialize_end, serialize_start));
        timeing = millis();
    }
#endif
}
