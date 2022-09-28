
#include "buildin_light_sensor.h"
#include "Arduino.h"

buildin_light_sensor::buildin_light_sensor() {
}
void buildin_light_sensor::init() {
    pinMode(WIO_LIGHT, INPUT);
}

bool buildin_light_sensor::read(struct sensor_data *sdata) {
    light_value      = analogRead(WIO_LIGHT);
    sdata->data_type = SENSOR_DATA_TYPE_INT32;
    sdata->data      = &light_value;
    sdata->size      = sizeof(light_value);
    sdata->id        = BUILDIN_LIGHT;
    sdata->name      = get_name();
    sdata->ui_type   = SENSOR_UI_TYPE_NORMAL;
    sdata->data_unit = data_unit;
    sdata->status    = true;
    return true;
}

const char *buildin_light_sensor::get_name() {
    return "light";
}

// buildin_light_sensor  buildin_light;