#include <WiFiNINA.h>

#include "network.h"
#include "constants.h"
#include "logging.h"

SysLog::Logger netLogger(syslogClient, "net");

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
            setLED(true);
            Serial.print("Connecting to WiFi .");

            while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
                setLED(false);

                sleep = millis();
                PT_WAIT_UNTIL(&pts, millis() - sleep > 300);

                setLED(true);
                Serial.print(".");
            }

            Serial.println(" OK");

            PrintString ip;
            ip.reserve(16);
            ip.print(WiFi.localIP());

            netLogger.info("connected to %s as %s", WIFI_SSID, ip.c_str());
        }

        PT_YIELD(&pts);
    }

    PT_END(&pts);
}
