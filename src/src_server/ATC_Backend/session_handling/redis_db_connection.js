var CFG = require('../config/config'); //include the cofnig file
var REDIS = require("redis");

var redisClient = null;
var redisReady = false;
function connect_db(){
redisClient = REDIS.createClient(CFG.getConfig().redis_connection_url);
    //REGISTER EVENTS FOR ERROR AND REDIS FINISHED
    redisClient.on("error", function(error) {
        console.error(error);
    });

    redisClient.on('ready', function () {
        console.log("Redis Ready");
        redisReady = true;
    });

}



/**
 * @class
 * Terminates Redis Database Connection on app shutdown
 */
process.on("exit", function(){
    if(redisClient == null){return;}
    redisClient.quit();
});


module.exports = {
    getRedisConnection: function () {
        if(redisClient == null){
            console.error("getRedisConnection - redis is not connected - please use conntec function first");
            connect_db();
            return null;
        }
        return redisClient;
    },connect: function(){
        return connect_db();
    },
}