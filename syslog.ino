#include <stdarg.h>

#include "syslog.h"

namespace SysLog {

Client::Client(UDP& udp, String server, uint16_t port, String hostname)
: udp(udp),
  server(server),
  port(port),
  hostname(hostname),
  level(INFORMATIONAL) {
}

void Client::setLogLevel(Severity severity) {
    level = severity;
}

bool Client::log(uint8_t facility, Severity severity, const char* appname, const char* message) {
    if (severity > level) {
        return true;
    }

    int priority = (facility << 3) | (severity & 7);

    char line[256];
    int len = snprintf(line, sizeof(line),
        "[%d] %s %s",
        severity, appname, message);
    if (len > 0) {
        Serial.println(line);
    }

    len = snprintf(line, sizeof(line),
        "<%d>1 - %s %s - - - %s",
        priority, hostname.c_str(), appname, message);
    if (len < 0) {
        return false;
    }

    return send((uint8_t*)line, min(len, sizeof(line) - 1));
}

bool Client::send(uint8_t* buffer, size_t size) {
    if (server.length() == 0) {
        return true;
    }

    int result = udp.begin(0);
    if (result) {
        result = udp.beginPacket(server.c_str(), port);
        if (result) {
            udp.write(buffer, size);
            result = udp.endPacket();
        }
        udp.stop();
    }

    return (result != 0);
}

Logger::Logger(Client& client, String appname, uint8_t facility)
: client(client),
  appname(appname),
  facility(facility) {
}

Logger::Logger(Logger& parent, String modname)
: client(parent.client),
  appname(parent.appname + "." + modname),
  facility(parent.facility) {
}

bool Logger::log(Severity severity, const char* message) {
    return client.log(facility, severity, appname.c_str(), message);
}

bool Logger::logf(Severity severity, const char* format, ...) {
    va_list args;
    va_start(args, format);
    bool result = vlog(severity, format, args);
    va_end(args);
    return result;
}

bool Logger::error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    bool result = vlog(ERROR, format, args);
    va_end(args);
    return result;
}

bool Logger::warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    bool result = vlog(WARNING, format, args);
    va_end(args);
    return result;
}

bool Logger::info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    bool result = vlog(INFORMATIONAL, format, args);
    va_end(args);
    return result;
}

bool Logger::debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    bool result = vlog(DEBUG, format, args);
    va_end(args);
    return result;
}

bool Logger::vlog(Severity severity, const char* format, va_list args) {
    char message[200];
    vsnprintf(message, 200, format, args);
    return log(severity, message);
}

}; // namespace SysLog
