#include "trigger.h"
#include "pin.h"

Trigger::Trigger(Pin& pin, long minDuration, Process* child)
: Process(pin), minDuration(minDuration), child(child) {
}

Trigger::~Trigger() {
    delete child;
}

int Trigger::run() {
    PT_BEGIN(&pts);

    for (;;) {
        // wait until input changes from high to low
        PT_WAIT_UNTIL(&pts, pin.get());
        PT_WAIT_UNTIL(&pts, !pin.get());

        // wait until input is high again
        startTime = millis();
        PT_WAIT_UNTIL(&pts, pin.get());
        long duration = millis() - startTime;

        // if input remained low for at least <minDuration> ms
        if (duration > minDuration) {
            pinLogger.info("%d was active for %d ms", pin.pinNumber(), duration);

            // spawn the child thread and wait until it exits
            PT_SPAWN(&pts, child->getPT(), child->run());
        }
    }

    PT_END(&pts);
}
