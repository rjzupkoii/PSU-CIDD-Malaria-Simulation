#!/usr/bin/env bash

set -ex
# SET THE FOLLOWING VARIABLES
# docker hub username
USERNAME=merlinvn
# image name
IMAGE=cpp_tools

version=`cat DOCKER_VERSION`

docker build -t $USERNAME/$IMAGE:latest .

docker tag $USERNAME/$IMAGE:latest $USERNAME/$IMAGE:$version

# push it
docker push $USERNAME/$IMAGE:$version
docker push $USERNAME/$IMAGE:latest
