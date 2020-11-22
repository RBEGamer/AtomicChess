var CronJob = require('cron').CronJob;
var redisDbConnection = require('../session_handling/redis_db_connection');
var CONFIG = require('../config'); //LOAD GLOABAL CONFIG FILE
var LH = require("./lobby_handling");
var GH = require("./game_handler");




var cleanup_job = new CronJob('*/'+CONFIG.getConfig().session_cleanup_loop_interval+' * * * * *', function() {
    if(CONFIG.session_lifetime_in_seconds <=0 ){return;}
    var cursor = '0';
    //GET ALL KEYS WITH MATCHED KEY PATTERN
    redisDbConnection.getRedisConnection().scan(cursor, 'MATCH', 'session:*', function(err, reply){
        if(err){
            throw err;
        }
        cursor = reply[0];
        //CHECK IF SEACH SUCCESSFUL
        if(cursor === '0'){
            try {
                //GET THE FOUND KEYS
                var found_keys = reply[1];
                //console.log('Scan Complete ' + typeof found_keys);
                //CHECK IF KEYS FOUND
                if(found_keys.length > 0){
                    //console.log('has_data');
                    //console.log(found_keys);
                    //ITERATE THOUGH EACH KEY TO GET THE DATA
                    for(var i = 0; i < found_keys.length;i++){
                        var k = found_keys[i];
                        //NOW CHECK THE CONTENT OF EACH KEY TO CHECK THE TIMESTAMP
                        redisDbConnection.getRedisConnection().get(k, function(err, reply) {
                            if(err || !reply){
                                console.error("db_fetch_failed");
                                return;
                            }
                            console.log(JSON.parse(reply).timestamp);
                            try {
                                var val2json = JSON.parse(reply);
                                //CEHCK FOR TIMESTAMP ENTRY
                                if(val2json && val2json.timestamp){
                                    var dt = new Date(val2json.timestamp); //PARSE TIMESTAMP TO A DATE OBJECT
                                    //IF TIME DIFFERENCE IS TOO HIGH -> REMOVE THE SESSION
                                    var diff=(Date.now()-dt) / 1000; //A TIMESTAMP IS IN MS SO WE HAVE DIVIDE
                                    //console.log(diff);
                                    if(diff > CONFIG.session_lifetime_in_seconds){
                                        console.log("entry to remove ts: " + diff);
                                        //REMOVE PLAYER FROM LOBBY
                                        LH.remove_player_from_lobby(val2json.hwid,function (lhrm_err,lhrm_res) {

                                        },false); //FALSE -> SET PLAYER ONLY OFFLINE
                                        GH.cancel_match_for_player(val2json.hwid,function (cmp_err,cmp_res){

                                        });
                                        //REMOVE KEY FROM DB
                                        redisDbConnection.getRedisConnection().del(k,function (err, reply) {
                                            if(err || !reply){
                                                console.error("db_del_failed");
                                                return;
                                            }
                                            //console.log("Redis Del", reply);
                                        });
                                    }
                                }else{
                                    throw "value doesn not contain a timestamp attribute";
                                }
                            }catch (e) {
                                console.log("json_parse_failed_of_redis_content");
                                console.log(e);
                            }
                        });
                    }
                }
            }catch (e) {
            console.error(e);
            }
        }else{
        }
    });


}, null, true, 'America/Los_Angeles');



module.exports = {
    startSessionCleanupCycle: function () {
        //if interval time is less then 0 we disbale the cleanup
        if(CONFIG.getConfig().session_lifetime_in_seconds <=0 ){
            console.info("--- SessionCleanupCycleDisabled ---");
            return;
        }
        cleanup_job.start();
    }
}
