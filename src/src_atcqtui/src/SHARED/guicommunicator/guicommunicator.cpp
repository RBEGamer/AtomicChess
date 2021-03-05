#include "guicommunicator.h"

#ifdef USES_QT
bool guicommunicator::IS_PLATTFORM_WEB = false;
#endif
guicommunicator::guicommunicator()
{
    #ifdef USES_QT

    #endif
	//    rpc::server srv(RPC_PORT);
	//    srv.bind(RPC_FKT_NAME, &guicommunicator::rpc_callback);
	//    ptrsrv = &srv;
	//    srv.async_run();
	
		debug_output("guicommunicator started");
}

guicommunicator::~guicommunicator() {
	//STOP REC THREAD IF RUNNING
	stop_recieve_thread();
}

void guicommunicator::debug_output(std::string _msg) {
#ifdef USES_QT
	qInfo() << _msg.c_str();
#else
	std::cout << _msg.c_str() << std::endl;
#endif
}

void guicommunicator::debug_event(GUI_EVENT _event, bool _rec) {
	if (_rec)
	{
		debug_output("---RECEIEVED EVENT ---");	
	}
	else
	{
		debug_output("---SEND EVENT ---");
	}
    

	debug_output("RAW: " + std::to_string((int)_event.event) + " " + std::to_string((int)_event.type) + " " + _event.value);
	//https://stackoverflow.com/questions/47525238/why-is-there-no-implicit-conversion-from-stdstring-view-to-stdstring
	std::string e = "";
	e += magic_enum::enum_name(_event.event);
	debug_output("EVENT: " + e);
	e = "";
	e += magic_enum::enum_name(_event.type);
	debug_output("TYPE: " + e);
	e = "";
	debug_output("VALUE: " + _event.value);

	if (_event.is_event_valid) {
		debug_output(" EVENT IS VALID");
	}
	else {
		debug_output("!! EVENT IS NOT VALID !!");
	}
}



void guicommunicator::createEvent(GUI_ELEMENT _event, GUI_VALUE_TYPE _type, std::string _value) {

	std::string tmp; //EVENT STRING
	//CONVERT EVENT TO JSON
	GUI_EVENT tmp_event;
	
	tmp_event.event = _event;
	tmp_event.type = _type;
	tmp_event.value = _value;
	
	
	tmp_event.ispageswitchevent = false;
	//HARDCODES STUFF TODO REMOVE
	//HAHA IS A WOKRAROUND FOR THE SHITTY ENUM/STRUCT CONSTRUCTION
	 std::string_view cfg_name = magic_enum::enum_name(_type);
	if (std::string(cfg_name).rfind("_SCREEN", 0) == 0) {tmp_event.ispageswitchevent = true; }
	else if (std::string(cfg_name) == "ERROR_MESSAGE") {tmp_event.ispageswitchevent = true; }
	else if (std::string(cfg_name).rfind("MESSAGEBOX_", 0) == 0) {tmp_event.ispageswitchevent = true; }
	
	
	
#ifdef USES_QT
	tmp_event.ack = 0;
#else
	tmp_event.ack = 1;
#endif // USES_QT

	tmp = guievent2Json(tmp_event);
	
	
#ifdef DEBUG
	debug_event(json2Guievent(tmp), false);			  
#endif // DEBUG
	
	
	
	if (en_qt_communication)
	{
		//MAKE REQUEST
		httplib::Client cli(EVENT_URL_COMPLETE);
		cli.Post(EVENT_URL_SETEVENT, tmp, "application/json");		
	}
    #ifndef USES_QT
    //MAKE REQUEST TO THE WEBAPPLICATION
    httplib::Client cli(EVENT_URL_COMPLETE_WEBGL);
    cli.Post(EVENT_URL_SETEVENT, tmp, "application/json");



#endif
}



std::string guicommunicator::guievent2Json(GUI_EVENT _ev)
{
	json11::Json::object root;
	
	root["event"] = magic_enum::enum_integer(_ev.event);
	root["type"] = magic_enum::enum_integer(_ev.type);
	root["value"] = _ev.value;
	root["ack"] = _ev.ack;
	root["ispageswitchevent"] = _ev.ispageswitchevent;
	root["is_event_valid"] = _ev.is_event_valid;
	
	
	std::string json_str = json11::Json(root).dump();
	return json_str;
}
	
	
guicommunicator::GUI_EVENT guicommunicator::json2Guievent(std::string _jsonstring)
{
	

	if (_jsonstring.empty()) {	
		GUI_EVENT tmp;
		tmp.is_event_valid = false;
		return tmp;
	}
	//PARSE JSON STRING
	std::string parse_err = "";
	json11::Json json_root = json11::Json::parse(_jsonstring.c_str(), parse_err);
	json11::Json::object root_obj = json_root.object_items();
	//CHECK PARSE RESULT
	if(!parse_err.empty() || json_root.is_null()) {
			GUI_EVENT tmp;
			tmp.is_event_valid = false;
			return tmp;
	}
	
	
	return json2Guievent(root_obj);
}

guicommunicator::GUI_EVENT guicommunicator::json2Guievent(json11::Json::object _jsobj)
{
	GUI_EVENT tmp;
	tmp.is_event_valid = true;
	//EVENT
	if(_jsobj.find("event") != _jsobj.end() && !_jsobj["event"].is_null()) {
		auto enres =  magic_enum::enum_cast<guicommunicator::GUI_ELEMENT>(_jsobj["event"].int_value());
		if (enres.has_value())
		{
			tmp.event = enres.value();
		}
		else
		{
			tmp.is_event_valid = false;
		}
			
	}else
	{
		tmp.is_event_valid = false;
	}
	//TYPE
	if(_jsobj.find("type") != _jsobj.end() && !_jsobj["type"].is_null()) {
		auto enres =  magic_enum::enum_cast<guicommunicator::GUI_VALUE_TYPE>(_jsobj["type"].int_value());
		if (enres.has_value())
		{
			tmp.type = enres.value();
		}
		else
		{
			tmp.is_event_valid = false;
		}
			
	}
	else
	{
		tmp.is_event_valid = false;
	}
	//VALUE
	if(_jsobj.find("value") != _jsobj.end() && !_jsobj["value"].is_null()) {
		tmp.value = _jsobj["value"].string_value();
	}
	else
	{
		tmp.is_event_valid = false;
	}
	
	
	//ACK
	if(_jsobj.find("ack") != _jsobj.end() && !_jsobj["ack"].is_null()) {
		tmp.ack = _jsobj["ack"].int_value();
	}
	else
	{
		tmp.is_event_valid = false;
	}
	
	//ispageswitchevent
	if(_jsobj.find("ispageswitchevent") != _jsobj.end() && !_jsobj["ispageswitchevent"].is_null()) {
		tmp.ispageswitchevent = _jsobj["ispageswitchevent"].bool_value();
	}
	else
	{
		tmp.is_event_valid = false;
	}
	
	return tmp;
}

void guicommunicator::enable_qt_communication(bool _en)
{
	en_qt_communication = _en;
}


bool guicommunicator::check_guicommunicator_version() {
	httplib::Client cli(EVENT_URL_COMPLETE);

	httplib::Result res =  cli.Get(EVENT_URL_VERSION);
	if (GUICOMMUNICATOR_VERSION == res->body) {
		debug_output("CHECK VERSION OK");
		debug_output("GOT:" + res->body + " REQUIRED:" + GUICOMMUNICATOR_VERSION);
		return true;
	}
	else {
		debug_output("GOT:" + res->body + " REQUIRED:" + GUICOMMUNICATOR_VERSION);
		return false;
	}
}

bool guicommunicator::check_guicommunicator_reachable() {
	if (!en_qt_communication)
	{
		return true;
	}
		
		
	httplib::Client cli(EVENT_URL_COMPLETE);
	httplib::Result res =  cli.Get(EVENT_URL_VERSION);
	if (res->status >= 200 && res->status < 300) {
		debug_output("CHECK check_guicommunicator_reachable OK");
		return true;
	}
		return false;
}


void guicommunicator::start_recieve_thread() {
  
	thread_running = true;
#ifdef USES_QT
	update_thread = QThread::create([this]{ this->recieve_thread_function(this); });
	update_thread->start();

	if (update_thread->isRunning()) {
		debug_output("GUI_COM THREAD isRunning confirmed");
	}
#else
	std::thread t1(recieve_thread_function, this);
	t1.detach();

	update_thread = &t1;     
#endif
	debug_output("GUI_COM THREAD STARTED");
}

void guicommunicator::stop_recieve_thread() {
	thread_running = false;
#ifdef USES_QT
	update_thread->terminate();
#else
	if (update_thread)
	{
		update_thread->join();
	}
#endif
	debug_output("GUI_COM THREAD STOPPED");
}


guicommunicator::GUI_EVENT guicommunicator::get_event() {
	return guicommunicator::get_gui_update_event();
}
guicommunicator::GUI_EVENT guicommunicator::get_gui_update_event() {
	GUI_EVENT tmp;
	tmp.is_event_valid = false;
	
	update_thread_mutex.lock();
	//IF A NEW GUI EVENT EXITS GET THEM WITH A THREAD LOCK
	if(!gui_update_event_queue.empty()) {
		tmp = gui_update_event_queue.front();
		gui_update_event_queue.pop();
	}
	
	update_thread_mutex.unlock();
	
	if (tmp.is_event_valid) {
		debug_output("GOT AN EVENT FROM RECIEVE THREAD");
		debug_event(tmp, true);
	}
	
	return tmp;
}

void guicommunicator::clearPreviousEvents()
{
	update_thread_mutex.lock();
	//SIMPLEST WAY TO CLEAN UP THE QUEUE
	gui_update_event_queue = std::queue<GUI_EVENT>();
	update_thread_mutex.unlock();
}



void guicommunicator::enqueue_event(GUI_EVENT _ev) {
	update_thread_mutex.lock();
	gui_update_event_queue.push(_ev);
	update_thread_mutex.unlock();
}

void guicommunicator::recieve_thread_function(guicommunicator* _this) {
	using namespace httplib;
	auto ret = _this->svr.set_mount_point("/public", WEBSERVER_STATIC_FILE_DIR);
	if (!ret) {
		_this->debug_output(WEBSERVER_STATIC_FILE_DIR);
	}
	_this->svr.set_file_extension_and_mimetype_mapping("qml", "application/qml");
	//REGISTER WEBSERVER EVENTS
	_this->svr.Get("/",
		[](const Request& req, Response& res) {
			res.set_redirect("/public/index.html", 302);
		});

	_this->svr.Get(EVENT_URL_SETEVENT,
		[_this](const Request& req, Response& res) {
			res.set_content(_this->guievent2Json(_this->last_event_from_webserver), "application/json");
		});

#ifndef USES_QT
	_this->svr.Get(WEBUI_EVENT_URL_GET_NEXT_EVENT,
		[_this](const Request& req, Response& res) {
			_this->webview_thread_mutex.lock();
			if (!_this->webview_update_event_queue.empty())
			{
				res.set_content(_this->guievent2Json(_this->webview_update_event_queue.front()), "application/json");	
				_this->webview_update_event_queue.pop();
			}
			else
			{
				res.set_content("{}", "application/json");	
			}
				
			_this->webview_thread_mutex.unlock();
			
		});
#endif
	
	_this->svr.Get(EVENT_URL_VERSION,
		[_this](const Request& req, Response& res) {
			res.set_content(GUICOMMUNICATOR_VERSION, "text/plain");
		});



	_this->svr.Post(EVENT_URL_SETEVENT,
		[_this](const Request& req, Response& res) {

			if (req.body.empty()) {
				res.set_content("{\"status\"\"body_is_empty\", \"err\":true}", "application/json");
				return;
			}

			GUI_EVENT ev = _this->json2Guievent(req.body);
			_this->last_event_from_webserver = ev;
			//        _this->debug_event(ev,false);
			         if(!ev.is_event_valid) {
				         res.set_content(req.body, "application/json");
				return;
			}

			_this->enqueue_event(ev);
			res.set_content("{\"status\"\"success\",\"err\":false}", "application/json");
			return;
		});

	//START WEBSERVER
#ifdef USES_QT


    if(guicommunicator::IS_PLATTFORM_WEB){
        _this->svr.listen(WEBSERVER_BIND_ADDR, WEBSERVER_WEBQT_STATUS_PORT); //START AS WEBAPP
    }else{
        _this->svr.listen(WEBSERVER_BIND_ADDR, WEBSERVER_STAUTS_PORT); //REGUALAR DISPLAY PORT

    }


#else
    _this->svr.listen(WEBSERVER_BIND_ADDR, WEBSERVER_STAUTS_PORT);
#endif


	while (_this->thread_running) {
	
	}
	_this->svr.stop();
}


	







//ADDING SOME VARIATIONS OF createEvent
void guicommunicator::createEvent(GUI_ELEMENT _event, GUI_VALUE_TYPE _type) {
	guicommunicator::createEvent(_event, _type, (std::string)"");
}
#ifdef USES_QT 
void guicommunicator::createEvent(GUI_ELEMENT _event, GUI_VALUE_TYPE _type, QString _value) {
	guicommunicator::createEvent(_event, _type, _value.toStdString());
}
#endif

#ifdef USES_QT
#else	
void guicommunicator::show_error_message_on_gui(std::string _err)
{
	createEvent(guicommunicator::GUI_ELEMENT::ERROR, guicommunicator::GUI_VALUE_TYPE::ERROR_MESSAGE, _err);
}


guicommunicator::GUI_MESSAGE_BOX_RESULT guicommunicator::show_message_box(GUI_MESSAGE_BOX_TYPE _type, std::string _message, int _wait_time_ms)
{
	//SET MESSAGEBOX TEXT
	createEvent(guicommunicator::GUI_ELEMENT::MESSAGEBOX_MESSAGE_TEXT, guicommunicator::GUI_VALUE_TYPE::USER_INPUT_STRING, _message);
	//SET TYPE (WITH OK BUTON AND CANCEL BUTTON,...)
	if(_type == guicommunicator::GUI_MESSAGE_BOX_TYPE::MSGBOX_B_OK)
	{
		createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MESSAGEBOX_TYPE_B);
	}
	else
	{
		createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::MESSAGEBOX_TYPE_A);
	}
	
	guicommunicator::GUI_MESSAGE_BOX_RESULT res = guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_NONE;	
	while (true)
	{
		guicommunicator::GUI_EVENT tmp_ev = get_gui_update_event();
		if (!tmp_ev.is_event_valid)
		{
			continue;
		}
		if (tmp_ev.event == guicommunicator::GUI_ELEMENT::MESSAGEBOX_OK_BTN && tmp_ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED)
		{
			res = guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_OK;
			break;
		}
		else if (tmp_ev.event == guicommunicator::GUI_ELEMENT::MESSAGEBOX_CANCEL_BTN && tmp_ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED)
		{
			res = guicommunicator::GUI_MESSAGE_BOX_RESULT::MSGBOX_RES_CANCEL;
			break;
		}
	}

}
#endif
