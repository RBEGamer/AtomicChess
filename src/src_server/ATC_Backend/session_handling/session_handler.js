var CronJob = require('cron').CronJob;
var redisDbConnection = require('../session_handling/redis_db_connection');
var CONFIG = require('../config'); //LOAD GLOABAL CONFIG FILE
var UUID = require('uuid');


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

function check_expired_session(_hwid,_callack){
    if(_hwid === undefined || _hwid == null){
        _callack("_hwid is undefined", null);
        return;
    }
    get_session(_hwid,function (gs_err,gs_res){
        if(gs_err || !gs_res){
            _callack(gs_err,null);
            return;
        }
        //CEHCK FOR TIMESTAMP ENTRY
        if(gs_res.timestamp != null && gs_res.timestamp) {
            var dt = new Date(gs_res.timestamp); //PARSE TIMESTAMP TO A DATE OBJECT
            //IF TIME DIFFERENCE IS TOO HIGH -> REMOVE THE SESSION
            var diff = (Date.now() - dt) / 1000; //A TIMESTAMP IS IN MS SO WE HAVE DIVIDE
            //console.log(diff);
            if (diff > CONFIG.getConfig().session_lifetime_in_seconds) {
                _callack(null, true);
            }else{
                _callack(null, false);
            }
        }else{
            _callack("session json parse failed", null);
        }
    });
}

function get_session_keys(_callback){
    var cursor = '0';
    redisDbConnection.getRedisConnection().scan(cursor, 'MATCH', 'session:*', function(err, reply) {
        if (err) {
            _callback(err,null);
            return;
        }
        //CHECK IF SEACH SUCCESSFUL
        if (reply[0]) {
            //RETURN KEYS
            _callback(null,reply[1]);
        }
    });
}

function insert_session(_hwid, _callback){
    //GENERATE SESSION STUFF
    var session_id = UUID.v1();
    var ts = Date.now();
    redisDbConnection.getRedisConnection().set("session:"+_hwid, JSON.stringify({session_id:session_id,timestamp:ts, hwid:_hwid}),function (s_err,s_res){
        _callback(s_err,session_id);
    });
}

function get_session(_hwid, _callback){
    if(_hwid === undefined || _hwid == null || _hwid === ""){
        _callback("_key is undefined", null);
        return;
    }
    redisDbConnection.getRedisConnection().get("session:"+_hwid, function(err, reply) {
        if (err) {
            console.error("db_fetch_failed");
            _callback(err,null);
            return;
        }
        if(reply == null || reply === ""){
            _callback("session_content_empty",null);
        }
        //THE RESULT IS A STRING SO WE NEED TO PARSE IT;
        try {
            var val2json = JSON.parse(reply);
        } catch (e) {
            console.log("json_parse_failed_of_redis_content");
            console.log(e);
            _callback(e,null);
            return;
        }
        //RETURN JSON OBJECT
        _callback(null,val2json);
    });
}

function update_session_timestamp(_hwid,_sid,_callback){
    get_session(_hwid,function (gs_err,gs_res) {
        //IF NO SESSION EXISTS => CREATE ONE
        if(gs_err || !gs_res){
            _callback("error_session_not_found", null);
            return;
        }
        if(!_sid || gs_res.session_id !== _sid){
            _callback("error_session_id_invalid", null);
            return;
        }
        //UPDATE SESSION TIMESTAMP
        var ts = Date.now();
        redisDbConnection.getRedisConnection().set("session:"+_hwid, JSON.stringify({session_id:_sid,timestamp:ts, hwid:_hwid}),function (s_err,s_res){
            _callback(s_err,_sid);
        });
    });
}

function delete_session(_hwid,_callback){
    redisDbConnection.getRedisConnection().del("session:"+_hwid,function (err, reply) {
        if(err || !reply){
            _callback(err,null);
            return;
        }
        _callback(err,reply);
    });
}



module.exports = {
    check_valid_session,
    insert_session,
    check_expired_session,
    get_session_keys,
    update_session_timestamp,
    get_session_keys,
    delete_session

}