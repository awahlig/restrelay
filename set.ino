#include "set.h"
#include "pin.h"

Set::Set(Pin& pin, long delay, bool value)
: Process(pin, delay), value(value) {
}

int Set::run() {
    PT_BEGIN(&pts);

    if (delay > 0) {
        _PT_SLEEP(delay);
    }

    pin.set(value);

    PT_END(&pts);
}
