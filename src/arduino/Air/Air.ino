#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <BMx280MI.h>
#include "Config.h"

ESP8266WebServer server(HTTP_PORT);
BMx280I2C bme280(BME280_I2C_ADDR);


/**
 * SENSORS
 * 
 * Obtain sersor readings
 * Sensors return an array of values as described in the comments
 * of each readSensorXXX function
 * 
 * BME280  - Temperature, Humidity and athmospheric Pressure
 */

void readSensorBME280(float data[3]) {
  // data[0] - Air Temperature in degress Celsius
  // data[1] - Air Relative Humidity in hPa
  // data[2] - Air Pressure in mHg
  
  if (!bme280.measure()) {
    Serial.println("Could not take BME280 measurement.");
    return;
  }
  
  do {
    delay(100);
  } while (!bme280.hasValue());
  
  data[0] = bme280.getTemperature() + BME280_CALIB_TEMPERATURE;
  data[1] = round(bme280.getHumidity() * 100) / 100 + BME280_CALIB_HUMIDITY;
  data[2] = round(bme280.getPressure()) / 100 + BME280_CALIB_PRESSURE;
  return;
}



/**
 * HTTP ROUTES
 * 
 * GET /        - handleRoot()
 * GET /metrics - handleGetMetrics() - OpenMetrics formatted output for Prometheus
 * GET /sensors - handleGetSensors() - JSON formatted output for API requests
 * 404          - handleNotFound()
 */
 
void handleRoot() {
  server.send(200, "text/plain", "ESP8266 IoT Sensor");
}



void handleGetSensors() {
  StaticJsonDocument<128> json;
  String output;
  float  data[3];

  readSensorBME280(data);
  
  json["air_temperature"] = data[0];
  json["air_relative_humidity"] = data[1];
  json["air_pressure"] = data[2];

  serializeJson(json, output);
  server.send(200, "application/json", output);
}



void handleGetMetrics() {
  String output;
  float  BME280Data[3];

  readSensorBME280(BME280Data);
  
  output  = "#HELP sensor_air_temperature Air temperature in degrees Celsius\n";
  output += "#TYPE sensor_air_temperature gauge\n";
  output += "sensor_air_temperature" + String(LABELS) + " " + String(BME280Data[0]) + "\n";

  output += "#HELP sensor_air_relative_humidity Air relative humidity in %H\n";
  output += "#TYPE sensor_air_relative_humidity gauge\n";
  output += "sensor_air_relative_humidity" + String(LABELS) + " " + String(BME280Data[1]) + "\n";

  output += "#HELP sensor_air_pressure Air pressure in hPa\n";
  output += "#TYPE sensor_air_pressure gauge\n";
  output += "sensor_air_pressure" + String(LABELS) + " " + String(BME280Data[2]) + "\n";

  server.send(200, "text/plain", output);
}



void handleNotFound() {
  String message  = "";
  
  message += "Not Found (404)\n\n";
  message += "URI: " + server.uri() + "\n";

  server.send(404, "text/plain", message);
}



/**
 * INITIAL SETUP
 * 
 * Open serial communication with 9600 boud rate for diagnistic data
 * Connect to WiFI and output connection details once established
 * Attach routes handlers and initialise the HTTP server
 * Initialise the I2C communication interface for reading sensor data
 * Initialise the BME280 sensor
 * 
 */
void setup(void) {

  // SERIAL PORT
  // Initialise serial output for debugging purposes
  // The serial console baud rate must match 9600

  Serial.begin(9600);
  Serial.println("");

 
  // WIFI CONNECTION
  // Initialise the WiFi adatpter and connect using ssid and password
  // Wait until connection is established
  // Output IP address once obtained
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  Serial.print("Waiting for WiFi link ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\r\nConnected to: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  // HTTP SERVER
  // Register HTTP routers and route handlers
  // Initialise HTTP server

  server.on("/", handleRoot);
  server.on("/metrics", handleGetMetrics);
  server.on("/sensors", handleGetSensors);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("HTTP server listening on port ");
  Serial.println(HTTP_PORT);


  // WIRE I2C
  // Initialise I2C Communication

  Wire.begin();
  

  // BME280 Temperature / Humidity / Pressure Sensor
  // Initialises the BME communication
  // Sets oversampling values to the sensor

  if (!bme280.begin())
  {
    Serial.println("BME280 sensor not found.");
    while (1);
  }
  
  bme280.resetToDefaults();
  bme280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
  bme280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
  bme280.writeOversamplingPressure(BMx280MI::OSRS_P_x16); 
}



/**
 * MAIN LOOP
 * 
 * Listen to incoming  http requests
 */
void loop(void) {
  server.handleClient();
}
