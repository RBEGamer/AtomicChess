#ifndef GUICOMMUNICATOR_H
#define GUICOMMUNICATOR_H

#define GUICOMMUNICATOR_VERSION "1.4.0"



#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>





#include "../magic_enum-master/include/magic_enum.hpp"
#include "../cpp-httplib-master/httplib.h"
#include "../json11-master/json11.hpp"


//SOME DEBUGGING AND LOGGIN
//ON QT USE THE QT STUFF
#ifdef USES_QT
    #include <QString>
    #include<QDebug>
    #include <QThread>
    #include <QMutex>
    #include <QGuiApplication>
    #include <QQmlApplicationEngine>
    #include <QCommandLineParser>
#endif

//IF RUNNING AS CONTROLLER => BIND TO EVERYTHING TO ALLOW COMMUNICATION WITH THE WEBUI
#ifdef USES_QT
#define WEBSERVER_BIND_ADDR "127.0.0.1" //NOTE CHANGE TO LOCALHOST
#else
#define WEBSERVER_BIND_ADDR "0.0.0.0" //NOTE CHANGE TO LOCALHOST
#endif



#ifdef USES_QT
#define WEBSERVER_STAUTS_PORT 8000
#define WEBSERVER_WEBQT_STATUS_PORT 8002
#define EVENT_CLIENT_PORT L"8001" // 1
#define EVENT_URL_COMPLETE "http://127.0.0.1:8001" //1
#else
#define WEBSERVER_STAUTS_PORT 8001 //1
#define EVENT_CLIENT_PORT L"8000"
#define EVENT_CLIENT_PORT_WEBGL L"8002"
#define EVENT_URL_COMPLETE "http://127.0.0.1:8000"
#define EVENT_URL_COMPLETE_WEBGL "http://127.0.0.1:8002"
#endif

#define EVENT_URL_SETEVENT "/status"
#define EVENT_URL_VERSION "/version"
#define WEBUI_EVENT_URL_GET_NEXT_EVENT "/webui_next_event"





class guicommunicator
{
public:

    enum class GUI_ELEMENT{
    BEGIN_BTN = 0,
    QI_START_EVENT = 1,
    INITFIELD_BTN = 2,
    ATC_LOGO_BTN = 3,
    CLOUD_LOGO_BTN = 4,
    OPEN_SETTINGS_BTN = 5,
    LOGOUT_BTN = 6,
    INIT_BTN = 7,
    SCAN_BOARD_BTN = 8,
    SWITCH_MENU = 9,



    INFOSCREEN_HWID_LABEL = 10,
    INFOSCREEN_SESSIONID_LABEL = 11,
    INFOSCREEN_RANK_LABEL = 12,
    INFOSCREEN_VERSION = 13,

    INFOSCREEN_PLAYERINFO_LABEL = 14,

    MAINMENU_START_AI_MATCH_BTN = 15,
    MAINMENU_MATCHMAKING_BTN = 16,
    MAINMENU_PLAYER_LIST_BTN = 17,
    ERROR = 18,
    NETWORK_STATUS = 19,

    MESSAGEBOX_MESSAGE_TEXT = 20,
    MESSAGEBOX_OK_BTN = 21,
    MESSAGEBOX_CANCEL_BTN = 22,



    DEBUG_FUNCTION_A = 23,
    DEBUG_FUNCTION_B = 24,
    DEBUG_FUNCTION_C = 25,
    DEBUG_FUNCTION_D = 26,
    DEBUG_FUNCTION_E = 27,
    DEBUG_FUNCTION_F = 28,
    DEBUG_FUNCTION_G = 28,
    DEBUG_FUNCTION_H = 28,


    GAMESCREEN_ABORT_GAME = 30,
    GAMESCREEN_PLAYER_COLOR = 31,
    GAMESCREEN_POSSIBLE_MOVES = 32,
    GAMESCREEN_PLAYER_TURN_COLOR = 33,

    PLAYER_EMM_LABEL = 34,
    PLAYER_EMM_OK = 35,
    PLAYER_EMM_RESET = 36,
    PLAYER_EMM_INPUT = 37,
    PLAYER_EMM_INPUT_RAW = 38,
    QT_UI_RESET = 39,
};

	enum class GUI_MESSAGE_BOX_TYPE
	{
		MSGBOX_B_OK = 0,
		MSGBOX_A_OK_CANCEL = 1
	};
	enum class GUI_MESSAGE_BOX_RESULT
	{
		MSGBOX_RES_NONE = 0,
		MSGBOX_RES_OK = 1,
		MSGBOX_RES_CANCEL =2
		
	};
	enum class GUI_VALUE_TYPE{
	CLICKED = 0,
	SELECTED = 1,
	USER_INPUT_STRING = 2,
	USER_INPUT_NUMBER = 3,
	ENABLED = 4,
	DISBALED = 5,


	//SPECIAL TYPES
	AVARIABLED_PLAYERS_ARRAY = 6,   //DATA FOR AVARIABLE PLAYER ARRAY VIEW
	CHESS_MOVE = 7,   //IS A C
	SWITCH_SCREEN = 8,
	PLAYER_ID = 9,   // THE PLAYER ID USED ON BEGIN_MATCH_WITH_PLAYER ELEMENT

	RES1 = 10,
	RE2 = 11,
	RES3 = 12,
	//MENUS
	LOGIN_SCREEN = 13,
	MAIN_MENU_SCREEN = 14,
	SETTINGS_SCREEN = 15,
	PLAYER_SEARCH_SCREEN = 16,
	GAME_SCREEN = 17,
	CREDIT_SCREEN = 18,
	INFO_SCREEN = 19,
    ERROR_MESSAGE = 20,
    ONLINE = 21,
    OFFLINE = 22,
    MESSAGEBOX_TYPE_A = 23,
    MESSAGEBOX_TYPE_B = 24,
    PROCESSING_SCREEN = 25,
    DEBUG_SCREEN = 26,
    CHESS_COLOR_WHITE = 27,
    CHESS_COLOR_BLACK = 28,
    PLAYER_ENTER_MANUAL_MOVE_SCREEN = 29
};

    //KEEP IN SYNC WITH THE PROTOCOL_MSG.proto
    struct GUI_EVENT{
        GUI_ELEMENT event;
        GUI_VALUE_TYPE type;
        std::string value;
        int ack;
        int ispageswitchevent;

        bool is_event_valid;
    };


    guicommunicator();
    ~guicommunicator();


	
	
	
	void enable_qt_communication(bool _en);

    void createEvent(GUI_ELEMENT _event, GUI_VALUE_TYPE _type, std::string _value); //sends a event though ZeroMQ using protocol buffer
    //DERIVATIONS FRom createEvent
    void createEvent(GUI_ELEMENT _event, GUI_VALUE_TYPE _type);
	void clearPreviousEvents();
    #ifdef USES_QT
    void createEvent(GUI_ELEMENT _event, GUI_VALUE_TYPE _type, QString _value);
    #endif

  
    void debug_event(GUI_EVENT _event, bool _rec);

    void start_recieve_thread();
    void stop_recieve_thread();


     GUI_EVENT get_gui_update_event();
	 GUI_EVENT get_event();
     bool check_guicommunicator_version();
	 bool check_guicommunicator_reachable();
#ifdef USES_QT
     static bool IS_PLATTFORM_WEB;
#endif
	//SOME FUNTIONS ARE ONLY FOR THE GUI SIDE
    #ifndef USES_QT
        void show_error_message_on_gui(std::string _err);	//DISPLAYS A ERROR MESSAGE WITH OK BUTTONS
        GUI_MESSAGE_BOX_RESULT show_message_box(GUI_MESSAGE_BOX_TYPE _type, std::string _message, int _wait_time_ms); //DISPLAY A MESSAGEBOX WITH OK/CANCEL BUTTON AND WAITS FOR THE USER INPUT
    #endif
private:

	
#ifdef USES_QT
      QThread* update_thread = nullptr;
      QMutex update_thread_mutex;
#else
    std::thread* update_thread = nullptr;
    std::mutex update_thread_mutex;
	std::mutex webview_thread_mutex;
	
#endif

    std::queue<GUI_EVENT> gui_update_event_queue;
	std::queue<GUI_EVENT> webview_update_event_queue;
	GUI_EVENT webview_last_screen_switch_event;
	
	bool thread_running = false;
	bool en_qt_communication = true;
    httplib::Server svr;

    GUI_EVENT last_event_from_webserver;


    void enqueue_event(GUI_EVENT _ev);
	void debug_output(std::string _msg);
	std::string guievent2Json(GUI_EVENT _ev);
	GUI_EVENT json2Guievent(std::string _jsonstring);
	GUI_EVENT json2Guievent(json11::Json::object _jsobj);
    static void recieve_thread_function(guicommunicator* _this);


};

#endif // GUICOMMUNICATOR_H
