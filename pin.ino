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
            return nullptr;
    }
}

Pin* Pin::get(char* name) {
    if (name[0] != 0 && name[1] == 0) {
        switch (name[0]) {
            case '1':
            case '2':
                return get(name[0] - '0');
        }
    } else if ((name[0] == 'A' || name[0] == 'a') && name[1] != 0 && name[2] == 0) {
        switch (name[1]) {
            case '1':
                return get(A1);
            case '2':
                return get(A2);
        }
    }
    return nullptr;
}

Pin::Pin(pin_size_t pin) : pin(pin) {
}

void Pin::setup(PinMode mode) {
    pinMode(pin, mode);
    if (mode == OUTPUT) {
        set(false);
    }
}

void Pin::loop() {
    if (process && !process->schedule()) {
        setProcess(nullptr);
    }
    if (process_2 && !process_2->schedule()) {
        setProcess_2(nullptr);
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
    setProcess(new Set(*this, delay, true));
}

void Pin::off(long delay) {
    setProcess(new Set(*this, delay, false));
}

void Pin::toggle(long delay) {
    setProcess(new Set(*this, delay, !get()));
}

void Pin::pulse(long delay, long duration, int count, long interval) {
    setProcess(new Pulse(*this, delay, duration, count, interval));
}

void Pin::trigger(long duration, Process* child) {
    setProcess_2(new Trigger(*this, duration, child));
}

void Pin::triggerPulse(Pin& dest) {
    trigger(100, new Pulse(dest, 200, 300, 1, 1000));
}

void Pin::setProcess(Process* p) {
    delete process;
    process = p;
}

void Pin::setProcess_2(Process* p) {
    delete process_2;
    process_2 = p;
}
