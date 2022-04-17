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

bool isIPAllowed(IPAddress ip) {
    static IPAddress allowed[] = HTTP_ALLOWED_IPS;
    int count = sizeof(allowed) / sizeof(IPAddress);

    if (count == 0) {
        return true;
    }
    for (int i = 0; i < count; i++) {
        if (ip == allowed[i]) {
            return true;
        }
    }
    return false;
}

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
    WiFiClient* client = static_cast<WiFiClient*>(req.stream());
    int status = res.statusSent();
    const char* query = req.query();

    PrintString ip;
    ip.reserve(16);
    ip.print(client->remoteIP());

    httpLogger.info("%s \"%s %s%s%s HTTP/1.%d\" %d",
                    ip.c_str(),
                    methodName(req.method()),
                    req.path(),
                    (query[0] ? "?" : ""),
                    query,
                    req.minorVersion(),
                    status ? status : 404);
}

void requireAuthorization(Request& req, Response& res) {
    WiFiClient* client = static_cast<WiFiClient*>(req.stream());
    if (!res.statusSent() && !isIPAllowed(client->remoteIP())) {
        res.sendStatus(401); // Unauthorized
        res.end();
    }
}

Pin* pinFromRoute(Request& req) {
    char buf[16];
    if (req.route("pin", buf, 16)) {
        return Pin::get(buf);
    }
    return nullptr;
}

long longFromQuery(Request& req, const char* name) {
    char buf[16];
    if (req.query(name, buf, 16)) {
        return atol(buf);
    }
    return 0;
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
    app.use(&requireAuthorization);
    app.get("/:pin", &handleState);
    app.post("/:pin/on", &handleOn);
    app.post("/:pin/off", &handleOff);
    app.post("/:pin/toggle", &handleToggle);
    app.post("/:pin/pulse", &handlePulse);
    app.finally(&logRequest);

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
