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
    char buf[10];
    req.query("delay", buf, 10);
    int d = String(buf).toInt();
    if (d <= 0) {
        d = 250;
    }
    pin.set(true);
    delay(d);
    pin.set(false);
    pin.respondState(res);
}

void ensureConnectedToWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println(WiFi.localIP());
    }
}

void setup() {
    Serial.begin(115200);

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
