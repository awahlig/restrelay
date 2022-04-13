#ifndef PROCESS_H
#define PROCESS_H

#include <pt.h>

class Pin;

class Process {
public:
    Process(Pin& pin);
    virtual ~Process();
    virtual int run() = 0;
    inline Pin& getPin() { return pin; }
    inline pt* getPT() { return &pts; }

protected:
    Pin& pin;
    pt pts;
    unsigned long sleep; // PT_SLEEP()
};

#endif /* PROCESS_H */
