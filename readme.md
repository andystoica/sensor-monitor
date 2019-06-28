# The Environmental Sensor Monitoring and Home Automation Project (ESMHAP)

This project is about learning, testing and documenting interconnected technology. It combines modern server platforms, popular monitoring tools, established programming languages and techniques to achieve a functional and modular IT system. It has a bit of everything: linux, networking, software development with RESTful API, electronics and automation.

On the server side there is centOS, Docker, Prometheus and Grafana. Software developments and RESTful APIs are written in REACT, node.js while the sensors are built using the D1 Mini Arduino IoT platform and written in C++.

On the fun side (optional), Illustrator was used for schematics and PCB layouts while Stepcraft 600 CNC machine for building the sensors modules.

## In development

###Platform

- [x] Linux server(centOS) running Docker platform
- [x] Prometheus for collecting data from an array of wireless sensors
- [x] Grafana for building gorgeous dashboards displaying the collected data

###Data sources

- [x] Node_exporter provides real time metrics for the running host
- [x] Array or Arduino based wireless sensors collecting information about air, light, humidity and noise
	- 	[x] Wireless indoor Air sensor with D1 Mini and BME280 (temperature, relative humidity and pressure)
	-  [x] Wireless outdoor Air, Light and Soil sensors with BME280, BH1750 and SEN0193
	-  [ ] Wireless outdoor UV and background noise sensor

###Home automation
- [ ] Control low voltage garden lights based on time of day and light intensity data
- [ ] Control data cabinet PWM fan based on environmental data
- [ ] Irrigate plants and flowers based on soil moisture data

## Links and resources
[https://www.centos.org](https://www.centos.org/download/)  
[https://prometheus.io](https://prometheus.io)  
[https://docs.docker.com/](https://docs.docker.com/)  
[https://grafana.com/](https://grafana.com/)  


