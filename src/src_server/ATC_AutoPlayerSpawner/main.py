import requests
import docker
import logging
import time
import sys
import signal
import os
import json
import yaml
import uuid


# GLOBAL VARIABLED
IS_PROD_ENVIRONMENT = False
BACKEND_IP = "http://127.0.0.1:3000/" #DEBUG BACKEND_IP
DOCKER_CONTAINER_SPAWN_CONFIG = None
CONFIG = {
"number_of_targeted_idle_ai_players":2, # HOW MANY AI PLAYER SHOULD BE AVARIABLE
"docker_compose_autoplayer_entry_name": "AtomicChessAutoPlayer" # NAME OF THE CONTAINER ENTRY IN THE DOCKER_COMPOSE_FILE
}

# LOAD CONFIG FILE .config.json INTO THE CONFIG DICT
def load_config():
    global CONFIG
    try:
        with open('./config.json') as config_file:
            data = json.load(config_file)
            CONFIG = data
    except Exception as e:
        logging.error("-- ./config.json cant be parsed --")
        logging.error(e)
# LOADS THE docker-compose.yml into the DOCKER_CONTAINER_SPAWN_CONFIG variable
def load_compose_file_attributes():
    global DOCKER_CONTAINER_SPAWN_CONFIG
    with open("./docker-compose.yml", 'r') as stream:
        try:
            compose_yml = yaml.safe_load(stream)
            logging.info(compose_yml)
            DOCKER_CONTAINER_SPAWN_CONFIG = compose_yml[CONFIG['docker_compose_autoplayer_entry_name']]
            return True
        except Exception as e:
            logging.error(e)
            return False

# SPAWN S A NEW AUTOPLAYER_CONTAINTER
def spawn_autoplayercontainter(_num_of_player_to_spawn):
    logging.info("spawn_autoplayercontainter")
    if DOCKER_CONTAINER_SPAWN_CONFIG is None:
        logging.error("DOCKER_CONTAINER_SPAWN_CONFIG is None run load_compose_file_attributes first")
        return
    #FINALLY START THE NEW CONTAINER WITH THE ATTRIBUTES FROM THE DOCKER-COMPOSE.yml
    # NOTE - not using from env beacuse docker is not installed in the container
    client = docker.DockerClient(base_url='unix://var/run/docker.sock')
    ret = client.containers.run(DOCKER_CONTAINER_SPAWN_CONFIG['image'], auto_remove=True,detach=True,links=DOCKER_CONTAINER_SPAWN_CONFIG['links'],environment=DOCKER_CONTAINER_SPAWN_CONFIG['environment'],remove=True,restart_policy={"Name": "None", "MaximumRetryCount": 5})
    logging.info(ret)
    return True

# CHECK THE CURRENT
def check_current_autoplayer_count():
    logging.info("check_current_autoplayer_count")
    print("ccac")
    logging.info("ping_backend()")
    try:
        # MAKE A REQUEST TO THE STATUS API OF THE BACKEND
        # A 200 IS VALID ERROR ON TIMEOUT
        r = requests.get(BACKEND_IP + "rest/get_avariable_ai_players", timeout=10)
        if r.status_code < 200 and r.status_code > 300:
            raise Exception('status_code invalid,must be in range of 200')
            return None
        retjson = r.json()
        if retjson['err'] is not None:
            logging.error(retjson['err'])
            raise Exception('err attribute in repsonse is not null')
        logging.info('current ai player count')
        logging.info(retjson['count'])
        return retjson['count']
    except Exception as e:
        logging.error(e)
        return None

def ping_backend():
    print("pb")
    logging.info("ping_backend()")
    try:
        # MAKE A REQUEST TO THE STATUS API OF THE BACKEND
        # A 200 IS VALID ERROR ON TIMEOUT
        r = requests.get(BACKEND_IP + "rest/status", timeout=2)
        if r.status_code >= 200 and r.status_code < 300:
            return True

    except Exception as e:
        logging.error(e)
        return False



def get_number(_num):
    try:
        return int(_num)
    except ValueError:
        return float(_num)

def signal_handler(_signum, _frame):
    logging.error("trapped signal")
    logging.error("terminating")
    sys.exit(1)


if __name__ == '__main__':
    # SETUP LOGGING
    logging.basicConfig(filename='./ATC_AutoPlayerSpawner.log', level=logging.DEBUG)
    logging.getLogger().addHandler(logging.StreamHandler()) # LOG TO STDOUT TOO
    logging.info('ATC_AutoPlayerSpawner - started')
    # SETUP SIGNAL HANDLING
    signal.signal(signal.SIGINT, signal_handler)
    # LOAD CONFIG FILE
    load_config()
    # LOAD/PARSE docker-compose FILE TO GET INFORMATION ABOUT THE TO SPAWN CONTAINER
    load_compose_file_attributes()
    spawn_autoplayercontainter(0)
    # CHECK FOR PRODUCTION ENVIRONMENT VARIABLE
    if os.environ.get('PRODUCTION') is not None:
        logging.info('-- ATC_AutoPlayerSpawner PRODUCTION VARIABLE IS SET')
        IS_PROD_ENVIRONMENT = True

    # LOAD .env FILE IF PROGRAM IS NOT IN PRODUCTION
    # IMPORT dotenv HERE IS A FIX DUE PYTHON 3.8
    if not IS_PROD_ENVIRONMENT:
        from dotenv import load_dotenv
        load_dotenv()

    # GET THE BACKEND_IP FORM .ENV
    if  os.environ.get('BACKEND_IP') is None:
        logging.error(".env variable BACKEND_IP not set")
        logging.error("terminating")
        sys.exit(2)
    BACKEND_IP = "http://" + os.environ.get('BACKEND_IP') + "/"
    logging.info("-- BACKEND_IP --")
    logging.info(BACKEND_IP)


    # CHECK IF THE BACKEND IS ALREADY REACHABLE - IF NOT TERMINATE AFTER X RETRIES
    backend_reachable = False
    for i in range(5):
        logging.info("* ping_backend")
        if ping_backend():
            backend_reachable = True
            break
        time.sleep(5)

    if not backend_reachable:
        logging.error("backend not reachable")
        logging.error("terminating")
        sys.exit(1)

    # NOW THE BACKEND IS REACHABLE WE CAN GOTO THE MAIN LOOP
    check_loop_running = True
    while check_loop_running:
        try:
            # GET AI PLAYER COUNTER
            running_atc_player = check_current_autoplayer_count()
            if running_atc_player is not None:
                # IF AI PLAYER COUNT IS TOO SMALL
                delta = get_number(running_atc_player)-get_number(CONFIG['number_of_targeted_idle_ai_players'])
                # IF MORE AI PLAYERS NEEDED SPAWN ONE AT EVERY LOOP
                logging.info("delta")
                logging.info(delta)
                if delta < 0:
                    logging.info("spawn a new player")
                    logging.info(running_atc_player)
                    # SPAWN A NEW PLAYER
                    spawn_autoplayercontainter(delta)
                    # WAIT A BIT FOR NEW PLAYER CONTAINER STARTUP
                    time.sleep(5)

        except Exception as e:
            logging.error(e)


        time.sleep(2)