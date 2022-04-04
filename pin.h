#ifndef PIN_H
#define PIN_H

class Process;

class Pin {
public:
    static Pin* get(pin_size_t pin);
    void setup();
    void loop();
    bool get() const;
    void set(bool value);
    void on(long delay);
    void off(long delay);
    void toggle(long delay);
    void pulse(long delay, long duration, int count, long interval);

private:
    Pin(pin_size_t pin);
    void clearProcess();

private:
    pin_size_t pin;
    Process* process;
};

#endif /* PIN_H */
