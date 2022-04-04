#ifndef PULSE_H
#define PULSE_H

#include <protothreads.h>

#include "process.h"

class Pulse : public Process {
public:
    Pulse(Pin& pin, long delay, long duration, int count, long interval);
    int run();

private:
    long duration;
    int count;
    long interval;
    int i;
};

#endif /* PULSE_H */
