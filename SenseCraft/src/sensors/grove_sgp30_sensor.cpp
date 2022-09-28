#include "grove_sgp30_sensor.h"
#include "Arduino.h"
#include "SensorsUtils.h"

Sgp30::Sgp30() : Thread("Sgp30", 128, 1) {
}
void Sgp30::Run() {
    softwarei2c.begin(SGP30_SDAPIN, SGP30_SCLPIN);
    // Initialize sensor
    mySensor.begin(softwarei2c);
    mySensor.initAirQuality();
    while (true) {
        softwarei2c.begin(SGP30_SDAPIN, SGP30_SCLPIN);
        if (mySensor.measureAirQuality() == SGP30_SUCCESS) {
            if (status == false)
                mySensor.initAirQuality();
            status  = true;
            data[0] = mySensor.CO2;
            data[1] = mySensor.TVOC;
        } else {
            status = false;
        }
        Delay(Ticks::MsToTicks(SENSOR_READ_DELAY));
    }
}

grove_sgp30_sensor::grove_sgp30_sensor() {
    sgp30 = new Sgp30();
}
void grove_sgp30_sensor::init() {
    sgp30->Start();
}

bool grove_sgp30_sensor::read(struct sensor_data *sdata) {

    sdata->data      = sgp30->data;
    sdata->data_type = SENSOR_DATA_TYPE_INT32;
    sdata->size      = sizeof(sgp30->data);
    sdata->id        = GROVE_SGP30;
    sdata->name      = get_name();
    sdata->status    = sgp30->status;
    sdata->ui_type   = SENSOR_UI_TYPE_NORMAL;
    sdata->data_unit = data_unit;
    return sdata->status;
}

const char *grove_sgp30_sensor::get_name() {
    return "sgp30";
}

// grove_sgp30_sensor  buildin_sgp30;       // Serial.println("Measurement failed!");