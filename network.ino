#include <pt.h>

#include "network.h"
#include "constants.h"
#include "logging.h"

SysLog::Logger netLogger(syslogClient, "net");

Network::Network() {
    PT_INIT(&pts);
}

void Network::setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

#ifdef NETWORK_USE_WIFI
    WiFi.setHostname(WIFI_HOSTNAME);

#else // !NETWORK_USE_WIFI
    Serial.println("Configuring Ethernet via DHCP");

    static uint8_t mac[6] = ETHERNET_MAC;
    while (!Ethernet.begin(mac)) {
        setLED(true);
        delay(500);
        setLED(false);
    }

    setLED(true);
    PrintString ip;
    ip.reserve(16);
    ip.print(Ethernet.localIP());
    syslogClient.setHostname(ip);

    netLogger.info("configured via DHCP");
#endif
}

void Network::setLED(bool value) {
    digitalWrite(LED_BUILTIN, (value ? HIGH : LOW));
}

void Network::loop() {
    PT_SCHEDULE(run());
}

int Network::run() {
    PT_BEGIN(&pts);

#ifdef NETWORK_USE_WIFI

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
            syslogClient.setHostname(ip);

            netLogger.info("connected to %s", WIFI_SSID);
        }
        PT_YIELD(&pts);
    }

#else // !NETWORK_USE_WIFI

    for (;;) {
        int check = Ethernet.maintain();
        switch (check) {
            case 0:
                break;

            case 1:
            case 3:
                setLED(false);

                if (check == 1) {
                    netLogger.error("DHCP renew failed");
                } else {
                    netLogger.error("DHCP rebind failed");
                }
                break;

            case 2:
            case 4: {
                setLED(true);
                PrintString ip;
                ip.reserve(16);
                ip.print(Ethernet.localIP());
                syslogClient.setHostname(ip);

                if (check == 2) {
                    netLogger.info("DHCP renewed");
                } else {
                    netLogger.info("DHCP rebound");
                }
                break;
            }

            default:
                netLogger.warn("DHCP unknown check result: %d", check);
        }
        PT_YIELD(&pts);
    }

#endif

    PT_END(&pts);
}
