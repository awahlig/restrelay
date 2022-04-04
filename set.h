#ifndef SET_H
#define SET_H

#include <protothreads.h>

#include "process.h"

class Set : public Process {
public:
    Set(Pin& pin, long delay, bool value);
    int run();

private:
    bool value;
};

#endif /* SET_H */
