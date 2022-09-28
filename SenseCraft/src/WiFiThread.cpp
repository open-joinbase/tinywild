
#include "WiFiThread.h"
#include "PubSubClient.h"
#include "RTC.h"
#include "utils.h"

WiFiThread::WiFiThread(SysConfig &config) : Thread("WiFiThread", 2048, 1), cfg(config) {
    Start();
}

void callback(char *topic, byte *payload, unsigned int length) {
    LOGSS.print("WIFI - Message arrived [");
    LOGSS.print(topic);
    LOGSS.print("] ");
    for (int i = 0; i < length; i++) {
        LOGSS.print((char)payload[i]);
    }
    LOGSS.println("");
}
void WiFiThread::reconnect() {
    // Loop until we're reconnected
    while (!client->connected()) {
        LOGSS.println("WIFI - Attempting MQTT connection...");

        // Attempt to connect
        auto id   = cfg.device_label.begin();
        auto user = cfg.mqtt_client_name.begin();
        auto pass = cfg.token.begin();
        if (client->connect(id, user, pass)) {
            LOGSS.println("WIFI - Attempting MQTT connected");
        } else {
            LOGSS.println(cfg.mqtt_client_name.begin());
            LOGSS.println(cfg.token.begin());
            LOGSS.print("WIFI - Attempting MQTT failed, rc=");
            LOGSS.print(client->state());
            LOGSS.println(" try again in 2 seconds");
            // Wait 2 seconds before retrying
            delay(2000);
        }
    }
}

// Sending data to JoinBase server
void WiFiThread::send_data() {
    // char        payload[800] = {0};
    const char *topic = "/iot_into_the_wild/sensors";
    if (!client->connected()) {
        reconnect();
    }
    // Builds the topic
    // sprintf(topic, "%s", ""); // Cleans the topic content
    // sprintf(topic, "%s%s", "/v2.0/devices/", cfg.device_label.begin());

    String payload;
    payload.reserve(128);
    // pack all into one json message at one time point
    // sprintf(payload, ,", now_ts());
    payload += "{\"ts\":\"" + now_ts() + "\"";
    for (auto data : wifi_data) {
        // Builds the payload

        // if (data.size / 4 <= 1) {
        //     sprintf(payload, "{\"%s\":", data.name); // Adds the variable label
        //     if (data.data_type == SENSOR_DATA_TYPE_FLOAT)
        //         sprintf(payload, "%s %f", payload, ((int32_t *)data.data)[0] / 100.0f);
        //     else
        //         sprintf(payload, "%s %d", payload, ((int32_t *)data.data)[0]); // Adds the value
        //     sprintf(payload, "%s}", payload); // Closes the dictionary brackets
        //     // client->publish(topic, payload);
        //     LOGSS.println(payload);
        // } else {
        //     for (int i = 0; i < data.size / 4; i++) {
        //         sprintf(payload, "%s", "");
        //         sprintf(payload, "{\"%s%d\":", data.name, i + 1); // Adds the variable label
        //         if (data.data_type == SENSOR_DATA_TYPE_FLOAT)
        //             sprintf(payload, "%s %f", payload, ((int32_t *)data.data)[i] / 100.0f);
        //         else
        //             sprintf(payload, "%s %d", payload, ((int32_t *)data.data)[i]); // Adds the
        //             value
        //         sprintf(payload, "%s}", payload); // Closes the dictionary brackets
        //         // client->publish(topic, payload);
        //         LOGSS.println(payload);
        //         delay(1000);
        //     }
        // }

        switch (data.id) {
            {
            case BUILDIN_LIGHT:
            case BUILDIN_MIC:
            case GROVE_SOIL:
                payload += ",\"";
                payload += data.name;
                payload += "\":";
                payload += ((int32_t *)data.data)[0];
                break;
            case LIS3DHTRSENSOR: {
                auto x = ((int32_t *)data.data)[0];
                auto y = ((int32_t *)data.data)[1];
                auto z = ((int32_t *)data.data)[2];
                payload += ",\"imu_x\":";
                payload += x;
                payload += ",\"imu_y\":";
                payload += y;
                payload += ",\"imu_z\":";
                payload += z;
                break;
            }
            case GROVE_VISIONAI: {
                // TODO now only sent the first VisionaiData for simplicity, but it is easy to
                //      support one-level dynamic array in the incoming message of JoinBase
                if (data.size != 0) {
                    auto vd = ((VisionaiData *)data.data)[0];
                    payload += ",\"animal\":\"";
                    payload += vd.obj;
                    payload += "\",\"confidence\":";
                    payload += vd.confidence;
                } else {
                    payload += ",\"animal\":\"Unknown\",\"confidence\":0";
                }
                break;
            }
            case GROVE_SGP30:
            case GROVE_SHT4X:
                // sdata.d0 = BSWAP16(static_cast<uint16_t>(((int32_t *)data.data)[0]));
                // sdata.d1 = BSWAP16(static_cast<uint16_t>(((int32_t *)data.data)[1]));
                // LOGSS.printf("temperature: %d, humidity: %d\r\n", sdata.d0, sdata.d1);
                break;
            default:
                break;
            }
        }
    }
    payload += "}";
    client->publish(topic, payload.begin());

    LOGSS.println(payload);
    // delay(1000);
    auto suc = client->loop();
    // LOGSS.print("MQTT client loop suc:");
    // LOGSS.println(suc);
}

void WiFiThread::Run() {
    LOGSS.println(cfg.ssid.begin());
    LOGSS.println(cfg.password.begin());
    client = new PubSubClient(wifiClient);
    client->setKeepAlive(UINT16_MAX);
    while (true) {
        if (cfg.wifi_on) {
            while (WiFi.status() != WL_CONNECTED) {
                LOGSS.println("WIFI - Connecting to WiFi...");
                WiFi.begin(cfg.ssid.begin(), cfg.password.begin());
                LOGSS.println("WIFI - Connecting to WiFi begined..."); // sometimes twice?
                Delay(Ticks::MsToTicks(1000));
                if (WiFi.status() == WL_CONNECTED) {
                    client->setServer(MQTT_BROKER, 1883);
                    client->setCallback(callback);
                }
            }
            LOGSS.println("WIFI -  wifi connected!");
            cfg.wificonnected = true;
            cfg.wifi_rssi     = WiFi.RSSI();
            wifi_data_ready   = false;

            // setup rtc
            for (static bool once = true; once; once = false) {
                rtc_setup_once();
            }

            send_data(); // Sending data to Ubidots
            wifi_data_ready = true;
            Delay(Ticks::SecondsToTicks(3));
        } else {
            WiFi.disconnect();
            cfg.wificonnected = false;
            Delay(Ticks::MsToTicks(1000));
        }
    }
}

// Store the received sensor data into a queue of length 30.
void WiFiThread::WiFiPushData(std::vector<sensor_data *> d) {
    // A loop to deep copy param of d vector into new wifi_data queue
    // by Iterative method
    if (wifi_data_ready) {
        wifi_data.clear();
        wifi_data.shrink_to_fit();
        for (auto data : d)
            wifi_data.push_back(*data);
    }
}