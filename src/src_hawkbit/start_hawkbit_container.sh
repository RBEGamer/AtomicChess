#!/bin/bash
cd ./jenkins_conf && docker container run --name jenkins-blueocean --restart always --env DOCKER_HOST=tcp://docker:2376 -v $(pwd):/var/jenkins_home -p 8080:8080 -p 50000:50000 jenkinsci/blueocean
