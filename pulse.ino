#include "pulse.h"
#include "pin.h"

Pulse::Pulse(Pin& pin, int count, long duration, long interval)
: pin(pin), count(count), duration(duration), interval(interval) {
    PT_INIT(&pts);
}

int Pulse::run() {
    PT_BEGIN(&pts);

    for (i = 0; i < count; i++) {
        if (i > 0) {
            sleep = millis();
            PT_WAIT_UNTIL(&pts, millis() - sleep > interval);
        }

        pin.set(true);

        sleep = millis();
        PT_WAIT_UNTIL(&pts, millis() - sleep > duration);

        pin.set(false);
    }

    PT_END(&pts);
}
