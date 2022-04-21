#ifndef NETWORK_H
#define NETWORK_H

#include "constants.h"

#ifdef NETWORK_USE_WIFI

#include <WiFiNINA.h>

typedef WiFiClient NetworkClient;
typedef WiFiServer NetworkServer;
typedef WiFiUDP NetworkUDP;

#else

#include <Ethernet.h>

typedef EthernetClient NetworkClient;
typedef EthernetServer NetworkServer;
typedef EthernetUDP NetworkUDP;

#endif

#include <pt.h>

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
