#include "pin.h"

long longFromRoute(Request &req, const char *name) {
    char buf[16];
    req.route(name, buf, 16);
    return atol(buf);
}

long longFromQuery(Request &req, const char *name) {
    char buf[16];
    req.query(name, buf, 16);
    return atol(buf);
}

Pin* Pin::get(pin_size_t pin) {
    if (pin < 1 || pin > 2) {
        return NULL;
    }
    static Pin pins[] = {
        Pin(1),
        Pin(2),
    };
    return &pins[pin - 1];
}

Pin* Pin::fromRoute(Request &req) {
    return get(longFromRoute(req, "pin"));
}

void Pin::setup() {
    pinMode(pin, OUTPUT);
    set(false);
}

void Pin::loop() {
    if (pulse) {
        if (!PT_SCHEDULE(pulse->run())) {
            clearPulse();
        }
    }
}

bool Pin::get() const {
    return (digitalRead(pin) == HIGH);
}

void Pin::set(bool value) {
    digitalWrite(pin, (value ? HIGH : LOW));
}

void Pin::on() {
    clearPulse();
    set(true);
}

void Pin::off() {
    clearPulse();
    set(false);
}

void Pin::toggle() {
    clearPulse();
    set(!get());
}

void Pin::pulseFromQuery(Request& req) {
    int count = longFromQuery(req, "count");
    long dur = longFromQuery(req, "duration");
    long ival = longFromQuery(req, "interval");

    clearPulse();
    pulse = new Pulse(*this,
        (count > 0 ? count : 1),
        (dur > 0 ? dur : 300),
        (ival > 0 ? ival : 1000));
}

void Pin::clearPulse() {
    delete pulse;
    pulse = NULL;
}

void Pin::respondState(Response &res) const {
    res.print(get() ? "1" : "0");
}

Pin::Pin(pin_size_t pin) : pin(pin) {
}
