//
// Rename this file to Config.h
//

// WiFi Connection
#define WIFI_SSID "ssid_goes_here"    // Wifi network SSID
#define WIFI_PSK "passd_goes_here"    // WiFi network access password

// HTTP Server
#define HTTP_PORT 9100                // The HTTP port the sensor will be listening to

// Sensors
#define BME280_I2C_ADDR 0x76                // I2C Address for BME280 sensor. Can be 0x76 ot 0x77
#define BME280_CALIB_TEMPERATURE 0.0        // Temperature compesantion to reference sensor in degrees C
#define BME280_CALIB_HUMIDITY 0.0           // Humidty compensation to reference sensor in percentage %H
#define BME280_CALIB_PRESSURE 0.0           // Pressure compesation to reference sensor in hectopascals hPa
