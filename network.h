#ifndef NETWORK_H
#define NETWORK_H

#include <protothreads.h>

class Network {
public:
    Network();
    static void setup();
    void loop();

private:
    static void setLED(bool value);
    int run();

private:
    pt pts;
    unsigned long sleep;
};

#endif /* NETWORK_H */
