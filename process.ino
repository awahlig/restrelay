#include "process.h"

Process::Process(Pin& pin, long delay)
: pin(pin), delay(delay) {
    PT_INIT(&pts);
}
