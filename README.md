# Homework-CSAPP

My solutions to labs of CSAPP.
---------------------------------------
Environment setup:

```
docker container run -it -v F:\csapp\homework:/csapp --name=csapp_env ubuntu:18.04 /bin/bash
apt-get update
apt-get install sudo
sudo apt-get install build-essential
sudo apt-get install gcc-multilib
sudo apt-get install gdb
```
open container again:
```
docker exec -it container_id /bin/bash
```
