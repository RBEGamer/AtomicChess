#pragma once
#ifndef __StateMachine__
#define __StateMachine__

#include "BackendConnector.h"

class StateMachine
{
	
	
public:
	
	//THIS "SYSTEM_STATES" ARE USED LOCALLAY
	//WHICH ARE GENERATED OUT OF THE /get_player_state REQUEST FROM THE SERVER
	enum class SM_STATE
	{
		SMS_UNKNOWN = 0,
		SMS_IDLE_NO_MATCHMAKING = 1,
		SMS_IDLE_MATCHMAKING = 2,
		SMS_GAME_RUNNING_WAITING_FOR_INITILIZEING=3,
		SMS_GAME_RUNNING_INITILIZED=4,
		SMS_GAME_RUNNING_WAITING_FOR_OTHER_TURN=5,
		SMS_GANE_RUNNIGN_WAITING_FOR_OWN_TURN=6,
		SMS_GAME_ABORTED=7,
		SMS_GAME_ABORTED_BOARD_RESET=8
			
	}; 
	
	enum class SM_STATE_LOCAL
	{
		SMSL_UNKNOWN = 0,
		SMSL_BOARD_UNSYNCED = 1,
		SMSL_BOARD_SYNCED = 2,
			
	};
	
	
	StateMachine();
	
	
	
	SM_STATE_LOCAL determ_state(SM_STATE_LOCAL _psl);
	SM_STATE determ_state(BackendConnector::PLAYER_STATUS _ps);
	SM_STATE switch_to_next_state(BackendConnector::PLAYER_STATUS _ps);
	
	
	SM_STATE get_prev_state();
	
	
	
private:
	SM_STATE prev_state = SM_STATE::SMS_UNKNOWN; //REPRESENTS THE SERVER STATE
	SM_STATE_LOCAL prev_state_local = SM_STATE_LOCAL::SMSL_UNKNOWN;  //REPRESENTS THE LOCAL (HARDWARE)
};
#endif

