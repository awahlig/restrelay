#ifndef PROCESS_H
#define PROCESS_H

#include <pt.h>

class Pin;

class Process {
public:
    Process(Pin& pin);
    virtual ~Process();
    int schedule();
    void reset();

protected:
    virtual int run() = 0;

protected:
    Pin& pin;
    pt pts;
    unsigned long sleep; // PT_SLEEP()
};

#endif /* PROCESS_H */
