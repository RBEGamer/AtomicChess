#!/bin/bash
docgen build -i ./ATC_Backend.json -o ./exported/ATC_Backend.html
docgen build -i ./ATC_MoveValidator.json -o ./exported/ATC_MoveValidator.html


docgen build -i ./ATC_Backend.json -o ./exported/ATC_Backend.md -m
docgen build -i ./ATC_MoveValidator.json -o ./exported/ATC_MoveValidator.md -m

docgen build -i ./ATC_VoiceRecognition.json -o ./exported/ATC_VoiceRecognition.md -m
docgen build -i ./ATC_VoiceRecognition.json -o ./exported/ATC_VoiceRecognition.md -m