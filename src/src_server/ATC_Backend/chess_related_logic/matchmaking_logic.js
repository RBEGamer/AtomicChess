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

var ENABLE_AI_VS_AI = false;
//TODO MATCHMAKING HUMAN VS PLAYER
//SET VIA CONFIG

var matchmaking_job = new CronJob('*/'+CONFIG.matchmaking_runner_interval+' * * * * *', function() {

   // console.log("matchmaking_job");
    //GET ALL SEARCHING PLAYERS
 LH.get_player_for_matchmaking(function (gpfm_err,gpfm_res) {
 //   console.log(gpfm_res);
    if(gpfm_err){
        console.error(gpfm_err);
        return;
    }
    //TODO CREATE TWO LISTS WITH HUMAN AND AI PLAYER
     // IF
     //for each human where the is in state is over 10seconds combine with an
    //CHECK IF MORE THEN TWO PLAYERS ARE SEARCHING
    if(!gpfm_res || gpfm_res.combined_player_searching.length <= 1){
       // console.log("matchmakingjob only 1 player in lobby...");
        return;
    }
    //SORT PLAYERS BY WAITING TIME IN THE LOBBY
    gpfm_res.combined_player_searching.sort(player_sort_function_swt);
    //SELECT THE MOST WAITING PLAYER
    var p1 = gpfm_res.combined_player_searching[0];
    //CHOSE AN OTHER RANDOM ONE PLAYER
     //TODO ADD RANK ??
    var p2  = gpfm_res.combined_player_searching[HELPER_FUNCTIONS.randomInteger(1,gpfm_res.combined_player_searching.length-1)];
    //START A MATCH
    GH.start_match(p1.hwid,p2.hwid,function (sm_err,sm_res) {
        if(sm_err){
            console.error(sm_err);
        }
    });

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
