var CronJob = require('cron').CronJob;
//var RDB = require('../session_handling/redis_db_connection');
var CFG = require('../config/config'); //LOAD GLOABAL CONFIG FILE
var LH = require("./lobby_handling");
var GH = require("./game_handler");
var SH = require("./session_handler");



var cleanup_job = new CronJob('*/'+CFG.getConfig().session_cleanup_loop_interval+' * * * * *', function() {
    if(CFG.session_lifetime_in_seconds <=0 ){return;}
    try {

    //FIRST GET ALL PLAYER IN THE LOBBY
    LH.get_player_in_lobby(function (gpil_err,gpil_res){
        if(gpil_err){
            return;
        }
        if(gpil_res.length > 0){
            console.log("--- SCAN LOBBY ENTRIES ",gpil_res.length, "-----------");
        }
        for (let i = 0; i < gpil_res.length; i++) {
            const entry = gpil_res[i];
            SH.check_expired_session(entry.hwid,function (ces_err,ces_res){
                //IF ERROR
               // if(ces_err || !ces_res ){
                //    return;
                //}
                //IF RESULT IS TRUE THE SESSION IS EXIPRED
                if(ces_res != null && ces_res ===true){
                    console.log("---- REMOVING SESSION FOR ", entry.hwid,"------");
                    //FIRST DELETE SESSION
                    SH.delete_session(entry.hwid,function (ds_err,ds_res){
                        //CANCEL GAME
                        GH.cancel_match_for_player(entry.hwid,function (cmfp_err,cmfp_res){
                            //SET SESSION FLAG TO FALSE
                            LH.set_valid_session_flag(entry.hwid,false,function (svsf_err,svsf_res){
                                //SET PLAYER TO IDLE
                                LH.set_player_lobby_state(entry.hwid,LH.player_state.idle,function (spls_err,spls_res){

                                });
                            });
                        });
                    });

                    //IF RESULT IS NULL => SEESION ENTRY NOT EXISTS
                }else if( ces_res == null){
                    //CANCEL GAME
                    GH.cancel_match_for_player(entry.hwid,function (cmfp_err,cmfp_res){
                        //REMOVE PLAYER LOBBY ENTRY COMPLETELY
                        LH.remove_lobby_entry(entry.hwid,function (spls_err,spls_res){

                        });
                    });
                }
            });
        }
    });

    } catch (e) {
        console.log(e);
    }

}, null, true, 'America/Los_Angeles');



module.exports = {
    startSessionCleanupCycle: function () {
        //if interval time is less then 0 we disbale the cleanup
        if(CFG.getConfig().session_lifetime_in_seconds <=0 ){
            console.info("--- SessionCleanupCycleDisabled ---");
            return;
        }
        cleanup_job.start();
    }
}
