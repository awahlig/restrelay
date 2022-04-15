#ifndef SYSLOG_H
#define SYSLOG_H

namespace SysLog {

enum Severity {
    EMERGENCY = 0,
    ALERT,
    CRITICAL,
    ERROR,
    WARNING,
    NOTICE,
    INFORMATIONAL,
    DEBUG
};

class Client {
public:
    Client(UDP& udp, String server, uint16_t port, String hostname);

    void setLogLevel(Severity severity);
    bool log(uint8_t facility, Severity severity, const char* appname, const char* message);

protected:
    bool send(uint8_t* buffer, size_t size);

private:
    UDP& udp;
    String server;
    uint16_t port;
    String hostname;
    Severity level;
};

class Logger {
public:
    Logger(Client& client, String appname, uint8_t facility = 16);
    Logger(Logger& parent, String modname);

    bool log(Severity severity, const char* message);
    bool logf(Severity severity, const char* format, ...);

    bool error(const char* format, ...);
    bool warn(const char* format, ...);
    bool info(const char* format, ...);
    bool debug(const char* format, ...);

protected:
    bool vlog(Severity severity, const char* format, va_list args);

private:
    Client& client;
    String appname;
    uint8_t facility;
};

}; // namespace SysLog

#endif /* SYSLOG_H */
