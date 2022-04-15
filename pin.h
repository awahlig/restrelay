#ifndef PIN_H
#define PIN_H

#include "syslog.h"

extern SysLog::Logger pinLogger;

class Process;

class Pin {
public:
    static Pin* get(pin_size_t pin);
    static Pin* get(char* name);

    void setup(PinMode mode);
    void loop();

    bool get() const;
    void set(bool value);

    void on(long delay);
    void off(long delay);
    void toggle(long delay);
    void pulse(long delay, long duration, int count, long interval);

    void trigger(long duration, Process* child);
    void triggerPulse(Pin& dest);

    inline pin_size_t pinNumber() const { return pin; }

private:
    Pin(pin_size_t pin);
    void setProcess(Process* p);
    void setProcess_2(Process* p);

private:
    pin_size_t pin;
    Process* process;
    Process* process_2;
};

#endif /* PIN_H */
