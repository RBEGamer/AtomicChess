var LH = require("../session_handling/lobby_handling");
var GH = require("../session_handling/game_handler");
var CR = require("./chess_ranking");
var CONFIG = require("../config.json");
var CronJob = require('cron').CronJob;
var HELPER_FUNCTIONS = require("../session_handling/helper_functions");


function player_sort_function_swt(a, b) {
    if (a.state_switched_time < b.state_switched_time) return -1;
    if (a.state_switched_time > b.state_switched_time) return 1;
    return 0;
}



var matchmaking_job = new CronJob('*/'+CONFIG.matchmaking_runner_interval+' * * * * *', function() {

   // console.log("matchmaking_job");
    //GET ALL SEARCHING PLAYERS
 LH.get_player_for_matchmaking(function (gpfm_err,gpfm_res) {
    //CHECK ERROR
    if(gpfm_err){
        console.error(gpfm_err);
        return;
    }
    //CHECK IF MORE THEN TWO PLAYERS ARE SEARCHING (HUMAN + AI)
    if(!gpfm_res || gpfm_res.combined_player_searching.length <= 1){
       // console.log("matchmakingjob only 1 player in lobby...");
        return;
    }


        //EASIEST CASE 1 HUMAN AND 1 AI
     if(gpfm_res.player_searching_human.length === 1 && gpfm_res.player_searching_ai.length  >= 1){
        //THEN START ;ATCH BETWEEN THEM
         GH.start_match(gpfm_res.player_searching_human[0].hwid,gpfm_res.player_searching_ai[0].hwid,function (sm_err,sm_res) {
             if(sm_err){
                 console.error(sm_err);
             }
         });



        //MORE THAN 1 HUMAN PLAYER WAITING
     }else if(gpfm_res.player_searching_human.length > 1){
         //THEN MAKE A MATCH BEWEEN THE TWO HUMAN PLAYER
         //SORT PLAYER WITH THE LONGEST WAIT TIME IN THE LOBBY
         gpfm_res.player_searching_human.sort(player_sort_function_swt);
         //SELECT THE MOST WAITING PLAYER
         var p1 = gpfm_res.combined_player_searching[0];

         if(CONFIG.matchmaking_enable_minimum_wait &&  Math.floor((Date.now()-p1.state_switched_time)/1000) < 20){
             return;
         }
         var p2  = gpfm_res.combined_player_searching[HELPER_FUNCTIONS.randomInteger(1,gpfm_res.combined_player_searching.length-1)];
         if(p1.hwid === p2.hwid){
             return;
         }
         //START A MATCH
         GH.start_match(p1.hwid,p2.hwid,function (sm_err,sm_res) {
             if(sm_err){
                 console.error(sm_err);
             }
         });
     }
 },true);
});





function startMatchmakingCycle() {
    //if interval time is less then 0 we disbale the cleanup
    if(CONFIG.matchmaking_runner_interval <=0 ){
        console.info("--- startMatchmakingCycleDisbale ---");
        return;
    }
    matchmaking_job.start();
}

module.exports = {
    startMatchmakingCycle
}
