#include "logging.h"
#include "constants.h"
#include "network.h"

NetworkUDP udp;
SysLog::Client syslogClient(udp, SYSLOG_IP, SYSLOG_PORT);

size_t PrintString::write(const uint8_t* buffer, size_t size) {
    return (concat(buffer, size) ? size : 0);
}

size_t PrintString::write(uint8_t c) {
    return write(&c, 1);
}
