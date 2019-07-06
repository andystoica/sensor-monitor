# Sensor monitoring with Arduino, Prometheus and Grafana


## Open Weather Exporter

Deploy a node_exporter container to obtain metrics from the Open Weather Map API service converted to Open Metrics for Prometheus.

	$ docker container run \
		--detach \
		--restart always \
		--name ow-exporter \
		--network metrics \
		--hostname ow-exporter \
		--publish 9100:9100 \
		--env-file ~/config/open-weather-exporter/.env \
		andystoica/open-weather-exporter
		
The **`.env`** file need to contain the API key and Location ID obtained from [OpenWeatherMap.org](http://OpenWeatherMap.org)

	OPEN_WEATHER_API_KEY=openweathermapapikey123456789abc
	OPEN_WEATHER_LOCATION_ID=1234567	
	
### Test the metrics endpoint

	$ curl http://localhost:9100/metrics
>
	#HELP sensor_air_temperature Air temperature in degrees Celsius (˚C)
	#TYPE sensor_air_temperature guage
	sensor_air_temperature{location="OpenWeather"} 21.02
	#HELP sensor_air_relative_humidity Air relative humidity in percentage (%H)
	#TYPE sensor_air_relative_humidity guage
	sensor_air_relative_humidity{location="OpenWeather"} 72
	#HELP sensor_air_pressure Air pressure in hectopascals (hPa)
	#TYPE sensor_air_pressure guage
	sensor_air_pressure{location="OpenWeather"} 1014

### Test the sensor endpoint
	
	$ curl http://localhost:9100/sensors
>
	{
		"air_temperature": 21.02,
		"air_relative_humidity": 72,
		"air_pressure": 1014
	}