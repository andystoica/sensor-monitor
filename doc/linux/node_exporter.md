# Sensor monitoring with Arduino, Prometheus and Grafana


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