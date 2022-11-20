#!/bin/bash
#This script runs the given params on the docker machine. Make sure you build Dockerfile and name it build-avikernel before running.
#To build Dockerfile: docker build . -t build-avikernel:latest
#Example: ./run-docker.sh make run

docker run -it -v$(pwd):/avikernel --rm build-avikernel "$@"
#

