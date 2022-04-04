#include <WiFiNINA.h>
#include <aWOT.h>
#include <protothreads.h>

#include "network.h"
#include "pin.h"

Network network;
WiFiServer server(80);
Application app;

Pin* pinFromRoute(Request& req) {
    return Pin::get(longFromRoute(req, "pin"));
}

long longFromRoute(Request& req, const char* name) {
    char buf[16];
    req.route(name, buf, 16);
    return atol(buf);
}

long longFromQuery(Request& req, const char* name) {
    char buf[16];
    req.query(name, buf, 16);
    return atol(buf);
}

void handleIndex(Request& req, Response& res) {
    res.print("hello");
}

void handleState(Request& req, Response& res) {
    Pin* pin = pinFromRoute(req);
    if (pin) {
        res.print(pin->get() ? "1" : "0");
    } else {
        res.sendStatus(404);
    }
}

void handleOn(Request& req, Response& res) {
    Pin* pin = pinFromRoute(req);
    if (pin) {
        pin->on(longFromQuery(req, "delay"));
        res.print("ok");
    } else {
        res.sendStatus(404);
    }
}

void handleOff(Request& req, Response& res) {
    Pin* pin = pinFromRoute(req);
    if (pin) {
        pin->off(longFromQuery(req, "delay"));
        res.print("ok");
    } else {
        res.sendStatus(404);
    }
}

void handleToggle(Request& req, Response& res) {
    Pin* pin = pinFromRoute(req);
    if (pin) {
        pin->toggle(longFromQuery(req, "delay"));
        res.print("ok");
    } else {
        res.sendStatus(404);
    }
}

void handlePulse(Request& req, Response& res) {
    Pin* pin = pinFromRoute(req);
    if (pin) {
        long delay = longFromQuery(req, "delay");
        long duration = longFromQuery(req, "duration");
        int count = longFromQuery(req, "count");
        long interval = longFromQuery(req, "interval");

        pin->pulse(delay,
            (duration > 0 ? duration : 300),
            (count > 0 ? count : 1),
            (interval > 0 ? interval : 1000));

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
