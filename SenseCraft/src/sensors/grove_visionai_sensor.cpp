#include "grove_visionai_sensor.h"
#include "Arduino.h"
#include "SensorsUtils.h"

const char *OBJECTS[5] = {"Spider", "Duck", "Magpie", "Butterfly", "Unknown"};

Visionai::Visionai() : Thread("Visionai", 256, 1) {
}

void Visionai::pushlog(const char *msg) {
    log_data log;
    strcpy(log.data, msg);
    log.time = millis();
    if (ai_log.size() > LOG_MAX_SIZE) {
        ai_log.erase(ai_log.begin());
    }
    ai_log.push_back(log);
}

void Visionai::Run() {
    char log[32];
    softwarei2c.begin(VISIONAI_SDAPIN, VISIONAI_SCLPIN);
    ai.begin(ALGO_OBJECT_DETECTION, MODEL_EXT_INDEX_1); // Object detection and externel model 1
    while (true) {
        softwarei2c.begin(VISIONAI_SDAPIN, VISIONAI_SCLPIN);
        uint32_t tick = millis();
        if (ai.invoke()) // begin invoke
        {
            while (1) // wait for invoking finished
            {
                CMD_STATE_T ret = ai.state();
                if (ret == CMD_STATE_IDLE) {
                    break;
                } else if (ret == CMD_STATE_ERROR) {
                    pushlog("Vision Ai is disconnect.");
                    status = false;
                    goto next;
                }
                Delay(Ticks::MsToTicks(100));
            }
            uint8_t len = ai.get_result_len(); // receive how many people detect
            dsize       = len;
            if (len > 0 && len <= MAX_DETECTION) {
                int time1 = millis() - tick;
                sprintf(log, "Time consuming: %d", time1);
                pushlog(log);
                sprintf(log, "Number of people: %d", len);
                pushlog(log);
                object_detection_t g_data; // get data
                for (int i = 0; i < len; i++) {
                    sprintf(log, "Detecting and calculating: %d", i + 1);
                    pushlog(log);
                    VisionaiData ai_data;
                    ai.get_result(i, (uint8_t *)&g_data, sizeof(object_detection_t)); // get result
                    if (g_data.confidence < 100) {
                        ai_data.confidence = g_data.confidence;
                    } else {
                        ai_data.confidence = 100; // in most cases, the result is abnormal
                    }
                    if (g_data.target < 4) {
                        // data[i] = g_data.confidence;
                        ai_data.obj = OBJECTS[g_data.target];
                        // if (g_data.target == 0) {
                        //     sprintf(log, "g_data.target: %d", g_data.target);
                        //     pushlog(log);
                        // }
                    } else {
                        ai_data.obj = OBJECTS[4];
                    }
                    sprintf(log, "confidence: %d", ai_data.confidence);
                    pushlog(log);
                    sprintf(log, "    object: %s", ai_data.obj);
                    pushlog(log);

                    data[i] = ai_data;
                }
            } else {
                dsize = 0;
                // data[0] = 0;
                pushlog("No identification");
            }
            status = true;
        } else {
            pushlog("Vision Ai is disconnect.");
            status = false;
        }
    next:
        Delay(Ticks::MsToTicks(SENSOR_READ_DELAY));
    }
}

grove_visionai_sensor::grove_visionai_sensor() {
    visionai = new Visionai();
}
void grove_visionai_sensor::init() {
    visionai->Start();
}

bool grove_visionai_sensor::read(struct sensor_data *sdata) {
    sdata->size = visionai->dsize; // NOTE we just use num of elements as size rather than bytes
    sdata->data_type            = SENSOR_DATA_TYPE_INT32;
    sdata->data                 = &visionai->data;
    sdata->id                   = GROVE_VISIONAI;
    sdata->name                 = get_name();
    sdata->status               = visionai->status;
    sdata->ui_type              = SENSOR_UI_TYPE_AVERAGE;
    sdata->data_unit            = data_unit;
    grove_visionai_sensor::slog = visionai->ai_log;
    return sdata->status;
}

const char *grove_visionai_sensor::get_name() {
    return "visionai";
}

// grove_visionai_sensor  buildin_visionai;