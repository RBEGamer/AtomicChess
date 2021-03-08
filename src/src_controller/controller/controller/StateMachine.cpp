#include "StateMachine.h"


StateMachine::StateMachine()
{
	
}




StateMachine::SM_STATE StateMachine::determ_state(BackendConnector::PLAYER_STATUS _ps)
{
	if (!_ps.game_state.game_running && !_ps.matchmaking_state.waiting_for_game)
	{
		return StateMachine::SM_STATE::SMS_IDLE_NO_MATCHMAKING;
	}
	else if (!_ps.game_state.game_running && _ps.matchmaking_state.waiting_for_game)
	{
		return StateMachine::SM_STATE::SMS_IDLE_MATCHMAKING;
	}
	else if (_ps.game_state.game_running && _ps.game_state.is_syncing_phase)
	{
		return StateMachine::SM_STATE::SMS_GAME_RUNNING_WAITING_FOR_INITILIZEING;
	}
//	else if (_ps.game_state.game_running && !_ps.game_state.is_syncing_phase){
//			return StateMachine::SM_STATE::SMS_GAME_RUNNING_INITILIZED;
//	}
else if (_ps.game_state.game_running && !_ps.game_state.is_syncing_phase && _ps.game_state.is_my_turn && !_ps.game_state.is_game_over){
		return StateMachine::SM_STATE::SMS_GANE_RUNNIGN_WAITING_FOR_OWN_TURN;
	}
	else if (_ps.game_state.game_running && !_ps.game_state.is_syncing_phase && !_ps.game_state.is_my_turn && !_ps.game_state.is_game_over){
		return StateMachine::SM_STATE::SMS_GAME_RUNNING_WAITING_FOR_OTHER_TURN;
	}else if (_ps.game_state.game_running && _ps.game_state.is_game_over){
		return StateMachine::SM_STATE::SMS_GAME_ABORTED;
	}
		
	return StateMachine::SM_STATE::SMS_UNKNOWN;
}

StateMachine::SM_STATE StateMachine::switch_to_next_state(BackendConnector::PLAYER_STATUS _ps)
{
	StateMachine::SM_STATE tmp = determ_state(_ps);
	prev_state = tmp;
	return tmp;
}

StateMachine::SM_STATE StateMachine::get_prev_state()
{
	return prev_state;
}
