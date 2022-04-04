#include "pin.h"
#include "set.h"
#include "pulse.h"

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

void Pin::setup() {
    pinMode(pin, OUTPUT);
    set(false);
}

void Pin::loop() {
    if (process) {
        if (!PT_SCHEDULE(process->run())) {
            clearProcess();
        }
    }
}

bool Pin::get() const {
    return (digitalRead(pin) == HIGH);
}

void Pin::set(bool value) {
    digitalWrite(pin, (value ? HIGH : LOW));
}

void Pin::on(long delay) {
    clearProcess();
    process = new Set(*this, delay, true);
}

void Pin::off(long delay) {
    clearProcess();
    process = new Set(*this, delay, false);
}

void Pin::toggle(long delay) {
    clearProcess();
    process = new Set(*this, delay, !get());
}

void Pin::pulse(long delay, long duration, int count, long interval) {
    clearProcess();
    process = new Pulse(*this, delay, duration, count, interval);
}

void Pin::clearProcess() {
    delete process;
    process = NULL;
}

Pin::Pin(pin_size_t pin) : pin(pin) {
}
