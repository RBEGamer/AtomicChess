var dotenv = null;
var STATIC_CONFIG = require('./config.json'); //LOAD OUR STATIC CONFIG FROM THE LOCAL FILESYSTEM


var config = {};

//INIT CONFIG OBJECT = STATIC_CONFIG + ENV VARIABLES
function init_config() {
    console.log("init_config");
    console.log(process.env);
    //LOAD A STATIC CONFIG FROM THE config.json
    config = STATIC_CONFIG;
    //IF WE ARE NOT IN PRODUCTION USE THE .env FILE
    if (!process.env.PRODUCTION) {
        console.log("NO PRODUCTION ENVIRONMENT - LOADING FROM .ENV");
        dotenv = require('dotenv').config();
    } else {
        console.log("PRODUCTION ENVIRONMENT - USING ENV VARIABLES GIVEN");
    }
    //NOW LOAD THE REMAINING CONNECTION URLS FOR THE DATABASES ONTOP OF THE CONFIG
    //THESE ARE DEFINED IN THE DOCKERCOMPOSE FILE

    //LOADING IP FOR REDIS_IP
    if (process.env.REDIS_IP) {
        console.log("found redis ip in process.env.REDIS_IP");
        config.redis_connection_url = "redis://" + process.env.REDIS_IP
        console.log(config.redis_connection_url)
    } else {
        console.error("REDIS_IP does not exists in environment variables");
        //process.exit(-1);
    }

    //LOADING IP FOR MONGODB
    if (process.env.MONGO_IP) {
        console.log("found mongodb ip in process.env.REDIS_IP");
        config.mongodb_connection_url = "mongodb://" + process.env.MONGO_IP
        console.log(config.mongodb_connection_url)
    } else {
        console.error("MONGO_IP does not exists in environment variables");
        //process.exit(-1);
    }

    //LOADING IP FOR MOVE VALIDATOR IP
    if (process.env.MVVALIDATOR_IP) {
        console.log("found move validator ip in process.env.MVVALIDATOR_IP");
        config.chess_move_validator_api_url = "http://" + process.env.MVVALIDATOR_IP
        console.log(config.chess_move_validator_api_url)
    } else {
        console.error("MVVALIDATOR_IP does not exists in environment variables");
        process.exit(-1);
    }
    console.log(config);

}

function set_key(_key, _value) {
    config[String(_key)] = _value;
}

function get_key(_key) {
    return config[String(_key)];
}

module.exports = {
    getConfig: function () {
        return config;
    },
    init_config,
    get_key,
    set_key

}