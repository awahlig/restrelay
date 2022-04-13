#include "logging.h"
#include "constants.h"

SysLog::Client syslogClient(SYSLOG_HOSTNAME, SYSLOG_PORT, WIFI_HOSTNAME);

size_t PrintString::write(const uint8_t* buffer, size_t size) {
    return (concat(buffer, size) ? size : 0);
}

size_t PrintString::write(uint8_t c) {
    return write(&c, 1);
}