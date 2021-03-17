#!/bin/bash

# BUILD DOCUMENTS
bash ./build_thesis.sh
# COPY TO SERVER DIR
cp ./thesis.pdf ./thesis_server/protected/thesis_document.pdf
# START SERVER
cd thesis_server && docker-compose build && docker-compose down && docker-compose up -d && cd ..
