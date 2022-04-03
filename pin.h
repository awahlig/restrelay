#ifndef PIN_H
#define PIN_H

#include <aWOT.h>

#include "pulse.h"

class Pin {
public:
    static Pin* get(pin_size_t pin);
    static Pin* fromRoute(Request &req);
    void setup();
    void loop();
    bool get() const;
    void set(bool value);
    void on();
    void off();
    void toggle();
    void pulseFromQuery(Request& req);
    void respondState(Response &res) const;

private:
    Pin(pin_size_t pin);
    void clearPulse();

private:
    pin_size_t pin;
    Pulse* pulse;
};

#endif /* PIN_H */
