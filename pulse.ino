#include <pt.h>

#include "pulse.h"
#include "pin.h"
#include "ptsleep.h"

Pulse::Pulse(Pin& pin, long delay, long duration, int count, long interval)
: Process(pin), delay(delay), duration(duration), count(count), interval(interval) {
}

int Pulse::run() {
    PT_BEGIN(&pts);

    if (delay > 0) {
        PT_SLEEP(&pts, sleep, delay);
    }

    pinLogger.info("%d pulsing %d time(s) for %d ms (with %d ms interval)",
                   pin.pinNumber(), count, duration, interval);

    for (i = 0; i < count; i++) {
        if (i > 0) {
            PT_SLEEP(&pts, sleep, interval);
        }

        pin.set(true);
        PT_SLEEP(&pts, sleep, duration);
        pin.set(false);
    }

    PT_END(&pts);
}
