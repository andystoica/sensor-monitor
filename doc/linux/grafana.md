# Sensor monitoring with Arduino, Prometheus and Grafana


## Grafana

### Docker volume
Create a **docker volume** to keep Grafana persistent data: 

	$ docker volume create grafana-data


### Docker container
Create a container by mounting the docker volume under `/var/lib/grafana` on the container.

Container will run detached on the Docker bridge network exposing port 3000 and mapping it to port 3000 of the host.

	$ docker container run \
		--detach \
		--restart always \
		--name grafana \
		--volume grafana-data:/var/lib/grafana \
		--network metrics \
		--hostname grafana \
		--publish 3000:3000 \
		grafana/grafana:latest


### Add Prometheus as a source

In a web browser, connect to the host IP on port 3000 to initialise Grafana. Default username and password are admin/admin and you will be asked to change the password on first login.

On the main page, click on Add Data Sources and select Prometheus. Under URL add the prometheus hostname `http://prometheus:9090`. Click Save & Test and we're done.