#include <WiFiNINA.h>
#include <aWOT.h>

#include "constants.h"

WiFiServer server(80);
Application app;

int pinNumberFromRoute(Request &req) {
    char buf[10];
    req.route("pin", buf, 10);
    int pin = String(buf).toInt();
    if (pin < 1 || pin > 2) {
        return -1;
    }
    return pin;
}

void printPinState(int pin, Response &res) {
    res.print(digitalRead(pin) ? "1" : "0");
}

void handleIndex(Request &req, Response &res) {
    res.print("hello");
}

void handleState(Request &req, Response &res) {
    int pin = pinNumberFromRoute(req);
    if (pin < 0) {
        res.sendStatus(404);
        return;
    }
    printPinState(pin, res);
}

void handleOn(Request &req, Response &res) {
    int pin = pinNumberFromRoute(req);
    if (pin < 0) {
        res.sendStatus(404);
        return;
    }
    digitalWrite(pin, HIGH);
    printPinState(pin, res);
}

void handleOff(Request &req, Response &res) {
    int pin = pinNumberFromRoute(req);
    if (pin < 0) {
        res.sendStatus(404);
        return;
    }
    digitalWrite(pin, LOW);
    printPinState(pin, res);
}

void handleToggle(Request &req, Response &res) {
    int pin = pinNumberFromRoute(req);
    if (pin < 0) {
        res.sendStatus(404);
        return;
    }
    digitalWrite(pin, digitalRead(pin) ? LOW : HIGH);
    printPinState(pin, res);
}

void handlePulse(Request &req, Response &res) {
    int pin = pinNumberFromRoute(req);
    if (pin < 0) {
        res.sendStatus(404);
        return;
    }
    char buf[10];
    req.query("delay", buf, 10);
    int d = String(buf).toInt();
    if (d <= 0) {
        d = 250;
    }
    digitalWrite(pin, HIGH);
    delay(d);
    digitalWrite(pin, LOW);
    printPinState(pin, res);
}

void setupPin(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
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

    setupPin(1);
    setupPin(2);

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
