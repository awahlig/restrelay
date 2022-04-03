#include <WiFiNINA.h>

#include "network.h"
#include "constants.h"

Network::Network() {
    PT_INIT(&pts);
}

void Network::setup() {
    WiFi.setHostname(WIFI_HOSTNAME);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void Network::setLED(bool value) {
    digitalWrite(LED_BUILTIN, (value ? HIGH : LOW));
}

void Network::loop() {
    PT_SCHEDULE(run());
}

int Network::run() {
    PT_BEGIN(&pts);

    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
            setLED(false);

            while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
                setLED(true);

                sleep = millis();
                PT_WAIT_UNTIL(&pts, millis() - sleep > 300);

                setLED(false);
                Serial.print(".");
            }

            setLED(true);
            Serial.println(WiFi.localIP());
        }

        PT_YIELD(&pts);
    }

    PT_END(&pts);
}
