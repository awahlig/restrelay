// Comment out to use Ethernet instead.
#define NETWORK_USE_WIFI

// Ethernet MAC address. Unused if using WiFi.
// https://ssl.crox.net/arduinomac/
#define ETHERNET_MAC { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// WiFi configuration. Unused if using Ethernet.
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
// Host name sent to the DHCP server.
#define WIFI_HOSTNAME "restrelay"

// On which port the http server will be served.
#define HTTP_PORT 80

// An array of allowed remote IPs (IPAddress objects).
// If empty, allows connections from any IP.
#define HTTP_ALLOWED_IPS {}

// Address and port of a SysLog server, where log messages
// will be sent. Empty IPAddress disables SysLog logging.
#define SYSLOG_IP IPAddress()
#define SYSLOG_PORT 514
