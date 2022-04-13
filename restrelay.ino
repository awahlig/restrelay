#include <WiFiNINA.h>
#include <aWOT.h>
#include <protothreads.h>

#include "constants.h"
#include "network.h"
#include "pin.h"
#include "logging.h"

Network network;
WiFiServer server(HTTP_PORT);
Application app;

SysLog::Logger httpLogger(syslogClient, "http");

const char* methodName(Request::MethodType type) {
    switch (type) {
        case Request::GET: return "GET";
        case Request::HEAD: return "HEAD";
        case Request::POST: return "POST";
        case Request::PUT: return "PUT";
        case Request::DELETE: return "DELETE";
        case Request::PATCH: return "PATCH";
        case Request::OPTIONS: return "OPTIONS";
        case Request::ALL: return "ALL";
        default: return "UNKNOWN";
    }
}

void logRequest(Request& req, Response& res) {
    int status = res.statusSent();
    const char* query = req.query();

    httpLogger.info("\"%s %s%s%s HTTP/1.%d\" %d",
                    methodName(req.method()),
                    req.path(),
                    (query[0] ? "?" : ""),
                    query,
                    req.minorVersion(),
                    status ? status : 404);
}

Pin* pinFromRoute(Request& req) {
    long pin = longFromRoute(req, "pin");
    if (pin < 1 || pin > 2) {
        return NULL;
    }
    return Pin::get(pin);
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
    syslogClient.setLogLevel(SysLog::INFORMATIONAL);

    Pin::get(1)->setup(OUTPUT);
    Pin::get(2)->setup(OUTPUT);
    Pin::get(A1)->setup(INPUT_PULLUP);
    Pin::get(A2)->setup(INPUT_PULLUP);

    // configure pulse trigger on pins A1 and A2
    Pin::get(A1)->triggerPulse(*Pin::get(1));
    Pin::get(A2)->triggerPulse(*Pin::get(2));

    network.setup();

    app.get("/", &handleIndex);
    app.get("/:pin", &handleState);
    app.post("/:pin/on", &handleOn);
    app.post("/:pin/off", &handleOff);
    app.post("/:pin/toggle", &handleToggle);
    app.post("/:pin/pulse", &handlePulse);
    app.use(&logRequest);

    server.begin();
}

void loop() {
    Pin::get(1)->loop();
    Pin::get(2)->loop();
    Pin::get(A1)->loop();
    Pin::get(A2)->loop();

    network.loop();

    WiFiClient client = server.available();
    if (client.connected()) {
        app.process(&client);
        client.stop();
    }
}
