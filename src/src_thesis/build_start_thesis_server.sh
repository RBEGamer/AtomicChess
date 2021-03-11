#!/bin/bash

# BUILD DOCUMENTS
bash ./build_thesis.sh
bash ./build_report.sh
# COPY TO SERVER DIR
cp ./thesis_document.pdf ./thesis_server/protected/
cp ./report_document.pdf ./thesis_server/protected/
# START SERVER
cd thesis_server && docker-compose build && docker-compose down && docker-compose up -d && cd ..