//
// Rename this file to Config.h
//

// WiFi Connection
#define WIFI_SSID "ssid_goes_here"    // Wifi network SSID
#define WIFI_PSK "passd_goes_here"    // WiFi network access password

// HTTP Server
#define HTTP_PORT 9100                // The HTTP port the sensor will be listening to

// Sensors
#define BME280_I2C_ADDR 0x76          // I2C Address for BME280 sensor. Can be 0x76 ot 0x77
#define SEN0193_CALIBRATION_MIN 360   // The SEN0193 minimum value at 100% water
#define SEN0193_CALIBRATION_MAX 760   // The SEN0193 maximum value at 100% dry
