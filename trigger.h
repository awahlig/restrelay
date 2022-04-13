#ifndef TRIGGER_H
#define TRIGGER_H

#include "process.h"

class Trigger : public Process {
public:
    Trigger(Pin& pin, long minDuration, Process* child);
    ~Trigger();
    int run();

private:
    long minDuration;
    Process* child;
    unsigned long startTime;
};

#endif /* TRIGGER_H */
