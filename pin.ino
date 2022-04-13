#include "pin.h"
#include "set.h"
#include "pulse.h"
#include "trigger.h"
#include "logging.h"

SysLog::Logger pinLogger(syslogClient, "pin");

Pin* Pin::get(pin_size_t pin) {
    static Pin pin1(1),
               pin2(2),
               pinA1(A1),
               pinA2(A2);

    switch (pin) {
        case 1:
            return &pin1;
        case 2:
            return &pin2;
        case A1:
            return &pinA1;
        case A2:
            return &pinA2;

        default:
            return NULL;
    }
}

void Pin::setup(PinMode mode) {
    pinMode(pin, mode);
    if (mode == OUTPUT) {
        set(false);
    }
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
    pinLogger.debug("%d: setting to %d", pin, (value ? 1 : 0));
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

void Pin::trigger(long duration, Process* child) {
    clearProcess();
    process = new Trigger(*this, duration, child);
}

void Pin::triggerPulse(Pin& dest) {
    clearProcess();
    Pulse* child = new Pulse(dest, 200, 300, 1, 1000);
    process = new Trigger(*this, 100, child);
}

void Pin::clearProcess() {
    delete process;
    process = NULL;
}

Pin::Pin(pin_size_t pin) : pin(pin) {
}
