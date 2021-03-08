# ATC_MoveValidator

# FEATURES
The MV (ATC_MoveValidator) is a Python Flask Server which provides RESTFUL-API Calls to check chess boards.
The Chess-Board will be provided with a FEN String and the API returns the following routes and results:

[API_DOCUMENTATION](../API_Documentation/exported/ATC_MoveValidator.md)

# USAGE

The MV is based on a psthon flask server which an be used as stand alone service:
`$ pip3 install -r ./requirements.txt`
`$ python3 ./app.py`

In the whole system this Serive is used in a Dockered environment. 
To build a Dockerimage its.possible to use the 
`$ bash ./build_dockerimage.sh` file which builds a Docker-Image called `atcmovevalidator`

Run the Image with `$ docker run -d -p 3001:3001 atcmovevalidator:latest`
