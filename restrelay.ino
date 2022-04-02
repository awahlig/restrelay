#include <WiFiNINA.h>
#include <aWOT.h>

#include "constants.h"

WiFiServer server(80);
Application app;

class Pin {
public:
    Pin(pin_size_t number) : number(number) {
    }

    Pin(Request &req) {
        char buf[10];
        req.route("pin", buf, 10);
        this->number = String(buf).toInt();
    }

    bool isValid() const {
        pin_size_t pin = this->number;
        return (pin == 1 || pin == 2);
    }

    void setup() {
        pinMode(this->number, OUTPUT);
        this->set(false);
    }

    bool get() const {
        return digitalRead(this->number);
    }

    void set(bool value) {
        digitalWrite(this->number, value ? HIGH : LOW);
    }

    void respondState(Response &res) const {
        res.print(this->get() ? "1" : "0");
    }

private:
    pin_size_t number;
};

long longFromQuery(Request &req, const char *name) {
    char buf[16];
    req.query(name, buf, 16);
    return atol(buf);
}

void handleIndex(Request &req, Response &res) {
    res.print("hello");
}

void handleState(Request &req, Response &res) {
    Pin pin(req);
    if (!pin.isValid()) {
        res.sendStatus(404);
        return;
    }
    pin.respondState(res);
}

void handleOn(Request &req, Response &res) {
    Pin pin(req);
    if (!pin.isValid()) {
        res.sendStatus(404);
        return;
    }
    pin.set(true);
    pin.respondState(res);
}

void handleOff(Request &req, Response &res) {
    Pin pin(req);
    if (!pin.isValid()) {
        res.sendStatus(404);
        return;
    }
    pin.set(false);
    pin.respondState(res);
}

void handleToggle(Request &req, Response &res) {
    Pin pin(req);
    if (!pin.isValid()) {
        res.sendStatus(404);
        return;
    }
    pin.set(!pin.get());
    pin.respondState(res);
}

void handlePulse(Request &req, Response &res) {
    Pin pin(req);
    if (!pin.isValid()) {
        res.sendStatus(404);
        return;
    }
    long count = longFromQuery(req, "count");
    if (count <= 0) {
        count = 1;
    }
    long dur = longFromQuery(req, "duration");
    if (dur <= 0) {
        dur = 300;
    }
    long ival = longFromQuery(req, "interval");
    if (ival <= 0) {
        ival = 1000;
    }
    for (long i = 0; i < count; i++) {
        if (i > 0) {
            delay(ival);
        }
        pin.set(true);
        delay(dur);
        pin.set(false);
    }
    pin.respondState(res);
}

void ensureConnectedToWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Pin led(LED_BUILTIN);
        led.set(false);

        while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
            led.set(true);
            delay(300);
            led.set(false);
            Serial.print(".");
        }

        led.set(true);
        Serial.println(WiFi.localIP());
    }
}

void setup() {
    Serial.begin(115200);

    Pin(LED_BUILTIN).setup();
    Pin(1).setup();
    Pin(2).setup();

    WiFi.setHostname(WIFI_HOSTNAME);
    ensureConnectedToWiFi();

    app.get("/", &handleIndex);
    app.get("/:pin", &handleState);
    app.post("/:pin/on", &handleOn);
    app.post("/:pin/off", &handleOff);
    app.post("/:pin/toggle", &handleToggle);
    app.post("/:pin/pulse", &handlePulse);

    server.begin();
}

void loop() {  
    ensureConnectedToWiFi();

    WiFiClient client = server.available();
    if (client.connected()) {
        app.process(&client);
        client.stop();
    }
}
