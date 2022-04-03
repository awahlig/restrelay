#include <WiFiNINA.h>
#include <aWOT.h>
#include <protothreads.h>

#include "network.h"
#include "pin.h"

Network network;
WiFiServer server(80);
Application app;

void handleIndex(Request &req, Response &res) {
    res.print("hello");
}

void handleState(Request &req, Response &res) {
    Pin* pin = Pin::fromRoute(req);
    if (pin) {
        pin->respondState(res);
    } else {
        res.sendStatus(404);
    }
}

void handleOn(Request &req, Response &res) {
    Pin* pin = Pin::fromRoute(req);
    if (pin) {
        pin->on();
        pin->respondState(res);
    } else {
        res.sendStatus(404);
    }
}

void handleOff(Request &req, Response &res) {
    Pin* pin = Pin::fromRoute(req);
    if (pin) {
        pin->off();
        pin->respondState(res);
    } else {
        res.sendStatus(404);
    }
}

void handleToggle(Request &req, Response &res) {
    Pin* pin = Pin::fromRoute(req);
    if (pin) {
        pin->toggle();
        pin->respondState(res);
    } else {
        res.sendStatus(404);
    }
}

void handlePulse(Request &req, Response &res) {
    Pin* pin = Pin::fromRoute(req);
    if (pin) {
        pin->pulseFromQuery(req);
        res.print("ok");
    } else {
        res.sendStatus(404);
    }
}

void setup() {
    Serial.begin(115200);

    Pin::get(1)->setup();
    Pin::get(2)->setup();

    network.setup();

    app.get("/", &handleIndex);
    app.get("/:pin", &handleState);
    app.post("/:pin/on", &handleOn);
    app.post("/:pin/off", &handleOff);
    app.post("/:pin/toggle", &handleToggle);
    app.post("/:pin/pulse", &handlePulse);

    server.begin();
}

void loop() {
    Pin::get(1)->loop();
    Pin::get(2)->loop();

    network.loop();

    WiFiClient client = server.available();
    if (client.connected()) {
        app.process(&client);
        client.stop();
    }
}
