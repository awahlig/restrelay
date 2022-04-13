#ifndef LOGGING_H
#define LOGGING_H

#include "syslog.h"

extern SysLog::Client syslogClient;

class PrintString : public String, public Print {
    size_t write(const uint8_t* buffer, size_t size);
    size_t write(uint8_t c);
};

#endif /* LOGGING_H */
