var dotenv = null;
var STATIC_CONFIG = require('./config.json'); //LOAD OUR STATIC CONFIG


var config = {};

function init_config(){
    console.log("init_config");
    console.log(process.env);
    //LOAD A STATIC CONFIG FROM THE config.json
    config = STATIC_CONFIG;
    //IF WE ARE NOT IN PRODUCTION USE THE .env FILE
    if (!process.env.PRODUCTION) {
        console.log("NO PRODUCTION ENVIRONMENT - LOADING FROM .ENV");
        dotenv = require('dotenv').config();
    }else{
        console.log("PRODUCTION ENVIRONMENT - USING ENV VARIABLES GIVEN");
    }
    //REPLACE STARTIC CONFIG WITH .ENV CONFIG
    for (const key_static in STATIC_CONFIG) {
      if(process.env.hasOwnProperty(key_static)){
        config[key_static] = STATIC_CONFIG[key];
        console.log("overriding static with conf.env conf ");
        console.log(key_static);
      }
    }

    console.log(config);

}

function set_key(_key, _value){
    config[String(_key)] = _value;
}
function get_key(_key){
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
