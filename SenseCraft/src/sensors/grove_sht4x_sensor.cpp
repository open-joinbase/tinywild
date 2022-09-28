#include "grove_sht4x_sensor.h"
#include "Arduino.h"
#include "SensorsUtils.h"
#include "SoftwareI2C.h"

Sht4x::Sht4x() : Thread("Sht4x", 128, 1) {
}
void Sht4x::Run() {
    softwarei2c.begin(SHT4X_SDAPIN, SHT4X_SCLPIN);
    sht4x_dev.setPort(softwarei2c);
    sht4x_dev.setChipType(SHT4X_CHIPTYPE_A);
    sht4x_dev.setMode(SHT4X_CMD_MEAS_HI_PREC);
    sht4x_dev.checkSerial();
    while (true) {
        softwarei2c.begin(SHT4X_SDAPIN, SHT4X_SCLPIN);
        if (sht4x_dev.measure() != SHT4X_STATUS_OK) {
            status = false;
            goto next;
        }
        if (sht4x_dev.TcrcOK) {
            status  = true;
            data[0] = (int32_t)(sht4x_dev.TtoDegC() * 100);
        } else {
            data[0] = 0xFF;
        }
        if (sht4x_dev.RHcrcOK) {
            status  = true;
            data[1] = (int32_t)(sht4x_dev.RHtoPercent() * 100);
        } else {
            data[1] = 0xFF;
        }
    next:
        Delay(Ticks::MsToTicks(SENSOR_READ_DELAY));
    }
}

grove_sht4x_sensor::grove_sht4x_sensor() {
    sht4x = new Sht4x();
}
void grove_sht4x_sensor::init() {
    sht4x->Start();
}

bool grove_sht4x_sensor::read(struct sensor_data *sdata) {

    sdata->data      = sht4x->data;
    sdata->data_type = SENSOR_DATA_TYPE_FLOAT;
    sdata->size      = sizeof(sht4x->data);
    sdata->id        = GROVE_SHT4X;
    sdata->name      = get_name();
    sdata->status    = sht4x->status;
    sdata->ui_type   = SENSOR_UI_TYPE_NORMAL;
    sdata->data_unit = data_unit;
    return sdata->status;
}

const char *grove_sht4x_sensor::get_name() {
    return "sht4x";
}

// grove_sht4x_sensor  buildin_sht4x;