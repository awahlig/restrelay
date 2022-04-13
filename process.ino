#include "process.h"

Process::Process(Pin& pin)
: pin(pin) {
    PT_INIT(&pts);
}

Process::~Process() {
}
