# Sensor monitoring with Arduino, Prometheus and Grafana

## Docker Install
Starting with a clean installation on a centOS 7 from the *minimal image* with latest updates installed.

	$ sudo yum remove \
		docker \
		docker-client \
		docker-client-latest \
		docker-common \
		docker-latest \
		docker-latest-logrotate \
		docker-logrotate \
		docker-engine



We continue by install a few dependencies required by Docker.

```
$ sudo yum install -y yum-utils device-mapper-persistent-data lvm2
```


And a stable repository for Docker.

	$ sudo yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo


Install Docker CE (Community Edition)

	$ sudo yum -y install docker-ce


Start Docker and enable it to start automatically after reboot.

	$ sudo systemctl start docker && sudo systemctl enable docker


Add current user to the docker group.

	$ sudo usermod -aG docker [username]


Finally check Docker is up and running. Log out and log back in for the user to receive the correct permissions.

	$ docker version

Should output:

	Client:
	 Version:           18.09.6
	 API version:       1.39
	 Go version:        go1.10.8
	 Git commit:        481bc77156
	 Built:             Sat May  4 02:34:58 2019
	 OS/Arch:           linux/amd64
	 Experimental:      false
	
	Server: Docker Engine - Community
	 Engine:
	  Version:          18.09.6
	  API version:      1.39 (minimum version 1.12)
	  Go version:       go1.10.8
	  Git commit:       481bc77
	  Built:            Sat May  4 02:02:43 2019
	  OS/Arch:          linux/amd64
	  Experimental:     false



---
## Docker Network

Create a private network for running all the monitoring containers. We need to define own network to allow for DNS name resolution which is not available in the default bridge network. We can also assign static IP addresses and specify our own subnet.

```
$ docker network create \
	--driver=bridge \
	--subnet 10.10.10.0/24 \
	--ip-range 10.10.10.0/24 \
	--gateway 10.10.10.1 \
	metrics
```



---
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



---
## Node Exporter

Deploy a node_exporter container to obtain the metrics for the host server.

	$ docker container run \
		--detach \
		--restart always \
		--name node-exporter \
		--network metrics \
		--hostname node-exporter \
		--publish 9100:9100 \
		prom/node-exporter

Test deployment by sending a request to `http://server_address:9100/metrics` in a web browser.

Verify that Prometheus can access the data by accessing the `http://server_address:9090/targets` in a web browser.



---
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

---
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