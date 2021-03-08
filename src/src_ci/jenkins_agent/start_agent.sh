#!/bin/bash
java -jar agent.jar -jnlpUrl http://127.0.0.1:8080/computer/buildagent/slave-agent.jnlp -secret @secret-file -workDir "/var/jenkins"
