package main

import (
	"encoding/json"
	"errors"
	"fmt"
	//"uci"
	"io/ioutil"
	"log"
	"math/rand"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"
)

type PlayerState struct {
	Err              interface{}      `json:"err"`
	Status           string           `json:"status"`
	MatchmakingState MatchmakingState `json:"matchmaking_state"`
	GameState        GameState        `json:"game_state"`
}
type MatchmakingState struct {
	Detailed       interface{} `json:"detailed"`
	Err            interface{} `json:"err"`
	WaitingForGame bool        `json:"waiting_for_game"`
}
type CurrentBoard struct {
	Fen          string      `json:"fen"`
	ExtendetFen  string
	PlayerTurn   int         `json:"player_turn"`
	Move         interface{} `json:"move"`
	InitialBoard bool        `json:"initial_board"`
	IsBoardValid bool        `json:"is_board_valid"`
	LegalMoves   []string    `json:"legal_moves"`
	IsGameOver   bool        `json:"is_game_over"`
}
type TurnHistory struct {
	Fen          string      `json:"fen"`
	PlayerTurn   int         `json:"player_turn"`
	Move         interface{} `json:"move"`
	InitialBoard bool        `json:"initial_board"`
	IsBoardValid bool        `json:"is_board_valid"`
	LegalMoves   []string    `json:"legal_moves"`
	IsGameOver   bool        `json:"is_game_over"`
}
type Detailed struct {
	_id                  string        `json:"_id"`
	id                   string        `json:"id"`
	DOCTYPE              string        `json:"DOCTYPE"`
	PlayerWhiteHwid      string        `json:"player_white_hwid"`
	PlayerBlackHwid      string        `json:"player_black_hwid"`
	PlayerWhiteTurnState int           `json:"player_white_turn_state"`
	PlayerBlackTurnState int           `json:"player_black_turn_state"`
	GameInitTimestamp    int64         `json:"game_init_timestamp"`
	LastGameInteraction  int64         `json:"last_game_interaction"`
	GameState            int           `json:"game_state"`
	ObserverPlayer       []interface{} `json:"observer_player"`
	StartPlayer          string        `json:"start_player"`
	StartBoardString     string        `json:"start_board_string"`
	CurrentBoard         CurrentBoard  `json:"current_board"`
	CurrentActivePlayer  string        `json:"current_active_player"`
	TurnHistory          []TurnHistory `json:"turn_history"`
}
type Simplified struct {
	CurrentBoard   CurrentBoard `json:"current_board"`
	GameState      int          `json:"game_state"`
	ImWhitePlayer  bool         `json:"im_white_player"`
	IsMyTurn       bool         `json:"is_my_turn"`
	IsSyncingPhase bool         `json:"is_syncing_phase"`
	PlayerState int 			`json:"player_state"`
}
type GameState struct {
	Detailed    Detailed    `json:"detailed"`
	Simplified  Simplified  `json:"simplified"`
	Err         interface{} `json:"err"`
	GameRunning bool        `json:"game_running"`
}



type Profile struct {
	hwid string
	DOCTYPE string
	account_created int
	rank int
	friendly_name string
	virtual_player_id string
	player_type int
	_id string
}

type LoginMessage struct {
	err string
	status string
	sid string
	profile Profile
}

type LoginResult struct {
	err string
	sid string
	profile Profile
}

type SetPlayerStateResult struct{
	err string
	status string
}

const charset = "abcdefghijklmnopqrstuvwxyz" +
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

var seededRand *rand.Rand = rand.New(
	rand.NewSource(time.Now().UnixNano()))

func StringWithCharset(length int, charset string) string {
	b := make([]byte, length)
	for i := range b {
		b[i] = charset[seededRand.Intn(len(charset))]
	}
	return string(b)
}

func RandomString(length int) string {
	return StringWithCharset(length, charset)
}

func RestLogin(_hwid string) (error,LoginResult){
	var lr LoginResult

	//LOGIN AS e PLAYER
	resp, err := http.Get("http://127.0.0.1:3000/rest/login?hwid="+_hwid+"&playertype=1")
	check(err)
	body, err := ioutil.ReadAll(resp.Body)
	check(err)
	//fmt.Println(body)
	fmt.Print(string(body))


	//var msg LoginMessage
	var result map[string]string
	json.Unmarshal(body,&result)
	//if err_parse == nil {
		if result["status"] == "ok"{
		//TODO USE MAP INTERFACE
			//	profile := result["profile"]
		//	fmt.Print(profile)
			//a := result["sid"].(map[string]interface{})
			lr := LoginResult{
				err:     result["err"],
				sid:     result["sid"],
				profile: Profile{
					hwid: _hwid,
				},
			}//  := fmt.Print(result["sid"])

			return nil,lr
		}else{
			return errors.New("login result unknown or login already ok"),lr
		}

	//}else{
	 //return errors.New("result parsing failed"),lr
	//}
}

func RestSetPlayerState(_hwid string, _sid string, _ps  string)(error,SetPlayerStateResult){
	var lr SetPlayerStateResult

	//LOGIN AS e PLAYER
	resp, err := http.Get("http://127.0.0.1:3000/rest/set_player_state?hwid="+_hwid+"&sid="+_sid+"&ps=" + _ps)
	check(err)
	body, err := ioutil.ReadAll(resp.Body)
	check(err)
	//fmt.Println(body)
	fmt.Print(string(body))


	//var msg LoginMessage
	var result map[string]string
	err_parse := json.Unmarshal(body,&result)

	if err_parse != nil {
		return errors.New("result parsing failed"),lr
	}
		if result["status"] == "ok"{

			lr := SetPlayerStateResult{
				err:     result["err"],
				status:     result["status"],

			}
			return nil,lr
		}else{
			return errors.New("set_status_failed"),lr
		}


}

func RestHeartbeat(_hwid string, _sid string, _interval time.Duration){
	for range time.Tick(time.Second*_interval){
		resp, err := http.Get("http://127.0.0.1:3000/rest/heartbeat?hwid="+_hwid+"&sid="+_sid)
		check(err)
		body, err := ioutil.ReadAll(resp.Body)
		check(err)
		//fmt.Println(body)
		if !strings.Contains(string(body),"ok"){
			fmt.Println(string(body))
		}

	}
}

func RestGetPlayerState(_hwid string, _sid string)(error,PlayerState){
	resp, err := http.Get("http://127.0.0.1:3000/rest/get_player_state?hwid="+_hwid+"&sid="+_sid+"&simplified=1")
	check(err)
	body, err := ioutil.ReadAll(resp.Body)
	check(err)
	//fmt.Println(body)
	//fmt.Println(string(body))
	b := string(body)
	fmt.Print(b)
	var app PlayerState
	eerr := json.Unmarshal(body, &app)
	if eerr != nil{
		fmt.Println(b)
	fmt.Println(eerr)
	return errors.New("error occured"), PlayerState{
		Err:              nil,
		Status:           "",
		MatchmakingState: MatchmakingState{},
		GameState:        GameState{},
	}
	}
	return nil,app

}

func RestPlayerSetupConfirmation(_hwid string, _sid string)(error){

	resp, err := http.Get("http://127.0.0.1:3000/rest/player_setup_confirmation?hwid="+_hwid+"&sid="+_sid+"")
	check(err)
	body, err := ioutil.ReadAll(resp.Body)
	check(err)
	b := string(body)
	fmt.Print(b)
	return err
}

func RestExecuteMove(_hwid string, _sid string, _move string)(error){

	resp, err := http.Get("http://127.0.0.1:3000/rest/make_move?hwid="+_hwid+"&sid="+_sid+"&move=" +_move)
	check(err)
	_, err = ioutil.ReadAll(resp.Body)
	check(err)
//	b := string(body)
//	fmt.Print(b)
	return err
}

func main() {
	//GET WORKSPACE
	workspace_path, err := os.Getwd()
	if err != nil {
		log.Println(err)
	}
	fmt.Println(workspace_path)  // for example /home/user
	//CHECK FOR STOCKFISH ENGINE
	eng, err := NewEngine(filepath.Join(workspace_path,"./stockfish-11-linux/src", "stockfish"))
	if err != nil {
		log.Fatal(err)
	}


	time.Sleep(time.Millisecond*5000)



	HWID := RandomString(10)
	fmt.Println("--- OUR USED HARDWARE ID ---")
	fmt.Println(HWID)
	var SID string

	//FIRST LOGIN AT THE SERVER AS CPU PLAYER
	err,LoginResultDetails := RestLogin(HWID)
	if err != nil{
		fmt.Println(LoginResultDetails)
		fmt.Println(err)
		//		return
	}

	SID = LoginResultDetails.sid
	fmt.Println("--- WE GOT A SESSION ID ---")
	fmt.Println(SID)

	//ENABLE HEARTBEAT
	//SEND A HEARBEAT REQUEST EACH SECOND TO AVOID THAT THE CLIENT WILL BE REMOVED FROM THE GAME
	//go RestHeartbeat(HWID,SID,5)


	//SET OWN STATUS TO MATCHMAKING
	err, SetPlayerStateResultDetails := RestSetPlayerState(HWID,SID,"4")
	if err != nil{
		fmt.Println(SetPlayerStateResultDetails)
		fmt.Println(err)
		return
	}

	//MAIN GAME LOOP
	//TODO GAME START ABBORT
	//GAME STATE FINISHED
	//-> EXIT
	for true{

		_, PlayerStateDetails := RestGetPlayerState(HWID,SID)
	//	fmt.Println(PlayerStateDetails)
		if PlayerStateDetails.Err != nil{
			//fmt.Println(PlayerStateDetails.Err)
			break
		}
		//NOW CHECK PLAYER STATE UNTIL WE GOT A MATCH
		if PlayerStateDetails.MatchmakingState.WaitingForGame{
			fmt.Println("-- MATCHMAKING RUNNING RUNNING --")
		}else if PlayerStateDetails.GameState.GameRunning{
			fmt.Println("-- GAME RUNNING --")

			//PLAYER_STATE = 0 -> SETUP BOARD / SETUP UCI ENGINE
			if PlayerStateDetails.GameState.Simplified.PlayerState == 0{
				//LOAD BOARD FEN
				eng.SetFEN(PlayerStateDetails.GameState.Simplified.CurrentBoard.ExtendetFen)
				//SETUP ENGINE
				eng.SetOptions(Options{
							Hash:128,
							Ponder:false,
							OwnBook:true,
							MultiPV:4,
						})
				//REPORT READY STATE TO BACKEND
				_ = RestPlayerSetupConfirmation(HWID,SID)

			}else if PlayerStateDetails.GameState.Simplified.IsMyTurn{
				//MAKE A TURN
				eng.SetFEN(PlayerStateDetails.GameState.Simplified.CurrentBoard.ExtendetFen)
				resultOpts := HighestDepthOnly | IncludeUpperbounds | IncludeLowerbounds
				uci_res, _ := eng.GoDepth(10, resultOpts)
				//fmt.Println(uci_res)
				//SEND TURN AND CHECK RESULT IF TURN WAS SUCCESS ELSE TRY NEXT IN QUEUE oder der state sich ändert
				//TODO CHECK IF MOVE != ""
				//TODO SYNC TABLES THEN MAKE MOVE
				err_em := RestExecuteMove(HWID,SID,uci_res.BestMove)
				if err_em != nil{
					fmt.Println(err_em)
				}
			}else{
				fmt.Print(PlayerStateDetails.GameState.GameRunning)
			}
			//GAME IS RUNNING CHECK GAME STATE NOW
		}
		//WAIT A BIT
		time.Sleep(time.Millisecond*1000)
	}





	//TODO LOGOUT -> WITH REMOVE
	time.Sleep(time.Second * 50)
}
func check(err error) {
	if err != nil {
		fmt.Println(err)

	}
}