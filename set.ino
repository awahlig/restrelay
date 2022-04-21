#include <pt.h>

#include "set.h"
#include "pin.h"
#include "ptsleep.h"

Set::Set(Pin& pin, long delay, bool value)
: Process(pin), delay(delay), value(value) {
}

int Set::run() {
    PT_BEGIN(&pts);

    if (delay > 0) {
        PT_SLEEP(&pts, sleep, delay);
    }

    pinLogger.info("%s changing to %d", pin.name(), (value ? 1 : 0));

    pin.set(value);

    PT_END(&pts);
}
