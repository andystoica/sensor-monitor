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