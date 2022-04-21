#include <pt.h>

#include "process.h"

Process::Process(Pin& pin)
: pin(pin) {
    PT_INIT(&pts);
}

int Process::schedule() {
    return PT_SCHEDULE(run());
}

void Process::reset() {
    PT_INIT(&pts);
}

Process::~Process() {
}
