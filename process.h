#ifndef PROCESS_H
#define PROCESS_H

#include <protothreads.h>

#define _PT_SLEEP(t) { \
    sleep = millis(); \
    PT_WAIT_UNTIL(&pts, millis() - sleep > (t)); }

class Pin;

class Process {
public:
    Process(Pin& pin, long delay);
    virtual int run() = 0;

protected:
    Pin& pin;
    pt pts;
    long delay;
    unsigned long sleep;
};

#endif /* PROCESS_H */
