var CronJob = require('cron').CronJob;
var redisDbConnection = require('../session_handling/redis_db_connection');
var CONFIG = require('../config'); //LOAD GLOABAL CONFIG FILE



function check_valid_session(_hwid,_sid,_callback){
    redisDbConnection.getRedisConnection().get("session:"+_hwid, function(err, reply) {
        //CHECK FOR KEY EXISTS
        if(err || !reply){
            _callback(err,false);
            return;
        }
        //PARSE KEY JSON-DATA
        var data = null;
        try {
            data = JSON.parse(reply);
            if(!data){
                throw "err_json_parse_data_failed_or_data_are_null";
            }
        } catch (e) {
            _callback(e,false);
            return;
        }
        //CHECK SESSION_ID
        if(!data.session_id || data.session_id !== _sid){
            _callback("err__sid_match_failed_with_db_session",false);
            return;
        }
        _callback(null,true);
    });
}



//TODO UPDATE SESSION TIMESTAMP AND REDO

module.exports = {
    check_valid_session

}