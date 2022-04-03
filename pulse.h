#ifndef PULSE_H
#define PULSE_H

#include <protothreads.h>

class Pin;

class Pulse {
public:
    Pulse(Pin& pin, int count, long duration, long interval);
    int run();

private:
    Pin& pin;
    pt pts;
    int count;
    long duration;
    long interval;
    int i;
    unsigned long sleep;
};

#endif /* PULSE_H */
