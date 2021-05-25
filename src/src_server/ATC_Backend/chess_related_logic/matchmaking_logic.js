var LH = require("../session_handling/lobby_handling");
var GH = require("../session_handling/game_handler");
var CR = require("./chess_ranking");
var CFG = require('../config/config'); //include the cofnig file
var CronJob = require('cron').CronJob;
var HELPER_FUNCTIONS = require("../session_handling/helper_functions");


function player_sort_function_swt(a, b) {
    if (a.state_switched_time < b.state_switched_time) return -1;
    if (a.state_switched_time > b.state_switched_time) return 1;
    return 0;
}

//SETUP MATCHMAKING CRONJOB
var matchmaking_job = new CronJob('*/' + CFG.getConfig().matchmaking_runner_interval + ' * * * * *', function () {
    //GET ALL PLAYERS WITH SEARCHING FOR A NEW GAME IS ENABLED
    //gpfm_res CONTAINS THESE PLAYERS DIVIED INTO HUMAN, AI AND BOTH COMBINED, IN SEPERATE LISTS
    LH.get_player_for_matchmaking(function (gpfm_err, gpfm_res) {
        //CHECK ERROR
        if (gpfm_err) {
            console.error(gpfm_err);
            return;
        }
        //CHECK IF MORE THEN TWO PLAYERS ARE SEARCHING (HUMAN + AI)
        if (!gpfm_res || gpfm_res.combined_player_searching.length <= 1) {
            // console.log("matchmakingjob only 1 player in lobby...");
            return;
        }
        //EASIEST CASE 1 HUMAN AND 1 AI
        if (CFG.getConfig().matchmaking_ai_enable === true && gpfm_res.player_searching_human.length === 1 && gpfm_res.player_searching_ai.length >= 1) {
            //THEN START ;ATCH BETWEEN THEM
            const player_wait_time = Math.floor((Date.now() - gpfm_res.player_searching_human[0].state_switched_time) / 1000);
            if (CFG.getConfig().matchmaking_enable_minimum_wait && player_wait_time < CFG.getConfig().matchmaking_enable_minimum_wait_time) {
                return;
            }
            //START A MATCH FOR THESES TWO PLAYERS => REMOVE LOBBY ENTRY FROM DB AND CREATE A NEW GAME IN THE GAME DATABASE
            GH.start_match(gpfm_res.player_searching_human[0].hwid, gpfm_res.player_searching_ai[0].hwid, function (sm_err, sm_res) {
                if (sm_err) {
                    //ON ANY ERROR THE CLIENT WILL RESET THE FAULTY STATE ITSELF AND RELOGIN TO THE SYSTEM
                    console.error(sm_err);
                }
            });
        //MORE THAN 1 HUMAN PLAYER WAITING
        } else if (gpfm_res.player_searching_human.length > 1) {
            //THEN MAKE A MATCH BEWEEN THE TWO HUMAN PLAYER
            //SORT PLAYER WITH THE LONGEST WAIT TIME IN THE LOBBY
            gpfm_res.player_searching_human.sort(player_sort_function_swt);
            //SELECT THE MOST WAITING PLAYER
            var p1 = gpfm_res.player_searching_human[0];
            //SELECT A RANDOM OTHER PLAYER
            var p2 = gpfm_res.player_searching_human[HELPER_FUNCTIONS.randomInteger(1, gpfm_res.player_searching_human.length - 1)];
            if (p1.hwid === p2.hwid) {
                return;
            }
            //START A MATCH
            GH.start_match(p1.hwid, p2.hwid, function (sm_err, sm_res) {
                if (sm_err) {
                    //ON ANY ERROR THE CLIENT WILL RESET THE FAULTY STATE ITSELF AND RELOGIN TO THE SYSTEM
                    console.error(sm_err);
                }
            });
        }
    }, true);
});


function startMatchmakingCycle() {
    //CHECK IF MATCHMAKING IS ENABLED
    if (CFG.getConfig().matchmaking_runner_interval <= 0) {
        console.info("--- startMatchmakingCycleDisbale ---");
        return;
    }
    //START THE MATCHMAKING CYCLE CRONJOB
    matchmaking_job.start();
}

module.exports = {
    startMatchmakingCycle
}
