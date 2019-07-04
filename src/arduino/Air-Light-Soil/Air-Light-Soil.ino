#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <BMx280MI.h>
#include <BH1750FVI.h>
#include "Config.h"

ESP8266WebServer server(HTTP_PORT);
BMx280I2C bme280(BME280_I2C_ADDR);
BH1750FVI bh1750(BH1750FVI::k_DevModeContLowRes);


/**
 * SENSORS
 * 
 * Obtain sersor readings
 * Sensors return an array of values as described in the comments
 * of each readSensorXXX function
 * 
 * BME280  - Temperature, Humidity and athmospheric Pressure
 * BH1750  - Light Level
 * SEN0193 - Capacitive Soil Moisture
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



void readSensorBH1750(int data[1]) {
  // data[0] - Light intensity level in lumens
 
  data[0] = bh1750.GetLightIntensity();  
}



void readSensorSEN0193(int data[2]) {
  // data[0] - Soil moisture as a percentage
  // data[1] - Soil moisture RAW data
  // data[2] - Soil moisture minimum limit (100% water)
  // data[3] - Soil moisture maximum limit (100% dry)


  int sensor_data = 0;
  int sensor_min = SEN0193_CALIB_MIN;
  int sensor_max = SEN0193_CALIB_MAX;
  int sensor_range = 0;
  int sensor_reading = 0;
  int sensor_percentage = 0;
  
  sensor_data = analogRead(0);
  
  if (sensor_data > sensor_min && sensor_data < sensor_max) {
    sensor_range = sensor_max - sensor_min;
    sensor_reading = 100 - (sensor_data - sensor_min) * 100 / sensor_range;
    sensor_percentage = round(sensor_reading * 100) / 100;
  }
  else if (sensor_data <= sensor_min) {
    sensor_percentage = 100;
  }
  else if (sensor_data >= sensor_max) {
    sensor_percentage = 0;
  }

  data[0] = sensor_percentage;
  data[1] = sensor_data;
}



/**
 * HTTP Route Handlers
 * GET /        - handleRoot()
 * GET /sensors - handleGetSensors()
 * 404          - handleNotFound()
 */

void handleRoot() {
  server.send(200, "text/plain", "ESP8266 IoT microcontroller");
}



void handleGetSensors() {
  float  BME280Data[3];
  int    BH1750Data[1];
  int    SEN0193Data[4];

  StaticJsonDocument<256> json;
  String output;
 
  readSensorBME280(BME280Data);
  json["air_temperature"] = BME280Data[0];
  json["air_relative_humidity"] = BME280Data[1];
  json["air_pressure"] = BME280Data[2];

  readSensorBH1750(BH1750Data);
  json["light_intensity"] = BH1750Data[0];

  readSensorSEN0193(SEN0193Data);
  json["soil_moisture"] = SEN0193Data[0];
  json["soil_moisture_raw"] = SEN0193Data[1];

  serializeJson(json, output);
  server.send(200, "application/json", output);
}



void handleGetMetrics() {
  float  BME280Data[3];
  int    BH1750Data[1];
  int    SEN0193Data[2];

  StaticJsonDocument<256> json;
  String output;

  readSensorBME280(BME280Data);
  readSensorBH1750(BH1750Data);
  readSensorSEN0193(SEN0193Data);

  output  = "#HELP sensor_air_temperature Air temperature in degrees Celsius\n";
  output += "#TYPE sensor_air_temperature gauge\n";
  output += "sensor_air_temperature" + String(LABELS) + " " + String(BME280Data[0]) + "\n";

  output += "#HELP sensor_air_relative_humidity Air relative humidity in percentage\n";
  output += "#TYPE sensor_air_relative_humidity gauge\n";
  output += "sensor_air_relative_humidity" + String(LABELS) + " " + String(BME280Data[1]) + "\n";

  output += "#HELP sensor_air_pressure Air pressure in hPa\n";
  output += "#TYPE sensor_air_pressure gauge\n";
  output += "sensor_air_pressure" + String(LABELS) + " " + String(BME280Data[2]) + "\n";

  output += "#HELP sensor_light_intensity Light intensity in lux\n";
  output += "#TYPE sensor_light_intensity gauge\n";
  output += "sensor_light_intensity" + String(LABELS) + " " + String(BH1750Data[0]) + "\n";

  output += "#HELP sensor_soil_moisture Soil moisture in percentage\n";
  output += "#TYPE sensor_soil_moisture gauge\n";
  output += "sensor_soil_moisture" + String(LABELS) + " " + String(SEN0193Data[0]) + "\n";

  output += "#HELP sensor_soil_moisture_raw Soil moisture raw data\n";
  output += "#TYPE sensor_soil_moisture_raw gauge\n";
  output += "sensor_soil_moisture_raw" + String(LABELS) + " " + String(SEN0193Data[1]) + "\n";

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


  // BH1750 Light Intensity Sensor
  //
  // Initialises the sensor communication
  bh1750.begin();
}



/**
 * MAIN LOOP
 * 
 * Listen to incoming  http requests
 */
void loop(void) {
  server.handleClient();
}
