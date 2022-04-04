#include "pulse.h"
#include "pin.h"

Pulse::Pulse(Pin& pin, long delay, long duration, int count, long interval)
: Process(pin, delay), duration(duration), count(count), interval(interval) {
}

int Pulse::run() {
    PT_BEGIN(&pts);

    if (delay > 0) {
        _PT_SLEEP(delay);
    }

    for (i = 0; i < count; i++) {
        if (i > 0) {
            _PT_SLEEP(interval);
        }

        pin.set(true);
        _PT_SLEEP(duration);
        pin.set(false);
    }

    PT_END(&pts);
}
