# Sensor monitoring with Arduino, Prometheus and Grafana


## Prometheus

### Configuration file
Create a configuration a YAML configuration file for Prometheus and call it `prometheus.yml`.

	$ cd ~
	$ mkdir -p config/prometheus
	$ vi config/prometheus/prometheus.yml

Paste the configuration bellow and edit job names and targets as necessary.

	global:
	  scrape_interval:     5s
	
	scrape_configs:
	  - job_name: 'prometheus'
	    static_configs:
	      - targets: ['localhost:9090']
	
	  - job_name: 'node_exporter'
	    static_configs:
	      - targets: ['node_exporter:9100']
	
	  - job_name: 'environmental-sensors'
	    static_configs:
	      - targets: ['iot-sensor:9100']


### Docker volume
Create a Docker volume to keep Prometheus persistent data.

	$ docker volume create prometheus-data


### Docker container
Run Prometheus inside a Docker container using the latest available image on the prom repository on Docker Hub.

We mount the newly created volume under /prometheus. Configuration file is bind-mounted to root as /prometheus.yml.

Container will run detached on the Docker default bridge network exposing port 9090 and mapping it to port 9090 of the host. It is set to restart automatically on host reboot, or docker daemon restart.

Parameters can be passed to Prometheus at the end of the command. Here we increase the default retention time from 15 to 45 days.

	$ docker container run \
		--detach \
		--restart always \
		--name prometheus \
		--volume prometheus-data:/prometheus \
		--volume ~/config/prometheus/prometheus.yml:/prometheus.yml \
		--volume ~/config/prometheus/hosts:/etc/hosts:ro \
		--network metrics \
		--hostname prometheus \
		--publish 9090:9090 \
		prom/prometheus:latest \
		--config.file /prometheus.yml \
		--storage.tsdb.retention.time 45d

To locate, backup and restore prometheus persistent data, inspect the Docker volume to obtain the mount point:

	$ docker volume inspect prometheus-data

which returns

	[
		{
			"CreatedAt": "2019-01-01T01:01:01+01:00",
			"Driver": "local",
			"Labels": {},
			"Mountpoint": "/var/lib/docker/volumes/prometheus-data/_data",
			"Name": "prometheus-data",
			"Options": {},
			"Scope": "local"
		}
	]