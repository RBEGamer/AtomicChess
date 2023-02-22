//
// Created by prodevmo on 28.03.21.
//

#include "UserBoardController.h"

UserBoardController::UserBoardController(std::string _serialport_file, int _baud)
{
    LOG_SCOPE_F(INFO, "UserBoardController::UserBoardController");
    if (_serialport_file.empty())
    {
        LOG_F(ERROR, "serial port _serialport_file is empty %s", _serialport_file.c_str());
        return;
    }
    // OPEN SERIAL PORT
    if (serialport)
    {
        delete serialport;
    }
    serialport = new SerialInterface(_serialport_file, _baud);
    //OPEN PORT
    if(!serialport->init()){
        LOG_F(ERROR, "cant create init_serial_port SerialInterface");
        return;
    }
    // READ MAY EXISTING BUFFER => IGNORING STATUS DATA link FW version,....
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // READ BUFFER EMPTY
    if (serialport->is_open())
    {
        serialport->dummy_read();
    }
}

UserBoardController::~UserBoardController()
{
    if (serialport)
    {
        delete serialport;
    }
}
// TODO USE A BETTER WAY => tokenize
std::string UserBoardController::get_value(const std::string data, const char separator, const int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.at(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substr(strIndex[0], strIndex[1]) : "";
}

void UserBoardController::tokenize(std::string const &str, const char *delim, std::vector<std::string> &out)
{
    char *token = strtok(const_cast<char *>(str.c_str()), delim);
    while (token != nullptr)
    {
        out.push_back(std::string(token));
        token = strtok(nullptr, delim);
    }
}

// https://stackoverflow.com/questions/10058606/splitting-a-string-by-a-character/10058756
std::vector<std::string> UserBoardController::split(const std::string& _input, const char _char)
{
    std::stringstream test;
    test.str(_input); // LOAD STRING
    std::string segment;
    std::vector<std::string> seglist;
    while (std::getline(test, segment, _char))
    {
        seglist.push_back(segment);
    }
    return seglist;
}

std::string UserBoardController::send_command(std::string _cmd, const bool _blocking)
{
    if (serialport == nullptr)
    {
        LOG_F(ERROR, "serialpor is null");
        return "";
    }

    if (_cmd.empty())
    {
        return "";
    }
    // APPEND _ CHARAKTER IF NEEDED ACCORDING THE PROTOCOL
    //  version => _version_
    //  readnfc => _readnfc_
    char t = _cmd.at(0);
    if (_cmd.at(0) != '_')
    {
        _cmd = "_" + _cmd;
    }
    if (_cmd.at(_cmd.size() - 1) != '_')
    {
        _cmd += "_";
    }

    // WRITE COMMAND TO SERIAL LINE
    // APPEND NEW LINE CHARAKTER IF NEEDED
    if (_cmd.rfind('\n') == std::string::npos)
    {
        serialport->write_string(_cmd + '\n');
    }
    else
    {
        serialport->write_string(_cmd);
    }
    // RETURN ALWAYS A GOOD REPSONSE IF WE DISABLE WAIT FOR SUCCESS
    if (!_blocking)
    {
        return "_state_res_ok_";
    }

    // NOW READ RECEIVE BUFFER
    int wait_counter = 0;
    while (true)
    {
        const std::string resp = serialport->read_line();
        // CHECK FOR RESPONSE
        if (resp.rfind(_cmd + "res_") != std::string::npos)
        {
            return resp;
        }
        else
        {
            wait_counter++;
            if (wait_counter > 3)
            {
                break;
            }
        }
    }
    LOG_F(ERROR, "invalid response");
    return "_state_res_err_";
}
// HARDWARE_UBC_NFC_READ_RETRY_COUNT

void UserBoardController::set_led(const int _color)
{
    // const std::string readres = send_command("_"+ UBC_COMMAND_LED + "_" + std::to_string(_color) + "_", false);

    for (int i = 0; i < GENERAL_UBC_COMMAND_RESPONSE_RETRY; i++)
    {
        const std::string readres = send_command("_" + UBC_COMMAND_LED + "_" + std::to_string(_color) + "_", false);
        if (readres.empty())
        {
            continue;
        }
        // SPLIT REPSONSE
        const std::vector<std::string> re = split(readres, UBC_CMD_SEPERATOR);
        // CHECK SPLIT LENGTH
        if (re.size() != 4)
        {
            break;
        }
        // READ RESULT
        const std::string &errorcode = re.at(3);
        // READ STATUS CODE IF READOUT IS VALID / NO TAG PRESENT OR READ FAILED
        if (errorcode == "ok")
        {
            break;
        }
        else
        {
            LOG_F(WARNING, "UBC_COMMAND_LED ERRORCODE %s", errorcode.c_str());
        }
    }
}

void UserBoardController::set_servo(const int _pos)
{
    for (int i = 0; i < GENERAL_UBC_COMMAND_RESPONSE_RETRY; i++)
    {
        const std::string readres = send_command("_" + UBC_COMMAND_SERVO + "_" + std::to_string(_pos) + "_", false);
        if (readres.empty())
        {
            continue;
        }
        // SPLIT REPSONSE
        const std::vector<std::string> re = split(readres, UBC_CMD_SEPERATOR);
        // CHECK SPLIT LENGTH
        if (re.size() != 4)
        {
            continue;
        }
        // READ RESULT
        const std::string &errorcode = re.at(3);
        // READ STATUS CODE IF READOUT IS VALID / NO TAG PRESENT OR READ FAILED
        if (errorcode == "ok")
        {
            break;
        }
        else
        {
            LOG_F(WARNING, "UBC_COMMAND_LED ERRORCODE %s", errorcode.c_str());
        }
    }
}

ChessPiece::FIGURE UserBoardController::read_chess_piece_nfc()
{

    ChessPiece::FIGURE fig;
    fig.color = ChessPiece::COLOR::COLOR_UNKNOWN;
    fig.type = ChessPiece::TYPE::TYPE_INVALID;
    fig.figure_number = -1;
    fig.unique_id = -1;
    fig.is_empty = true;
    fig.figure_read_failed = true;
    // NOW TRY TO READ THE
    const int retry_count = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HARDWARE_UBC_NFC_READ_RETRY_COUNT) + 1;
    for (int i = 0; i < retry_count; i++)
    {
        std::string readres = send_command(UBC_COMMAND_READNFC, true);
        if (readres.empty())
        {
            continue;
        }
        // SPLIT REPSONSE
        const std::vector<std::string> re = split(readres, UBC_CMD_SEPERATOR);
        // CHECK SPLIT LENGTH
        if (re.size() != 6)
        {
            continue;
        }
        // READ RESULT
        const std::string &figure = re.at(3);
        const std::string &errorcode = re.at(4);
        // READ STATUS CODE IF READOUT IS VALID / NO TAG PRESENT OR READ FAILED
        if (errorcode == "ok")
        {
            if (figure.empty())
            {
                break;
            }
            const char figure_charakter = figure.at(0);
            // const char figure_id = figure_at(1);
            fig = ChessPiece::getFigureByCharakter(figure_charakter);
            break;
        }
        else if (errorcode == "notagpresent")
        {
            break;
        }
        else
        {
            LOG_F(WARNING, "NFC READ RESPONSE ERRORCODE %s", errorcode.c_str());
        }
    }
    return fig;
}