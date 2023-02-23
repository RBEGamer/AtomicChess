//
// Created by marcel on 23.02.23.
//

#ifndef CONTROLLER_SERIALINTERFACEBASE_H
#define CONTROLLER_SERIALINTERFACEBASE_H


class SerialInterfaceBase {
public:



    enum class SERIAL_PORT_STATE{
        CLOSED,
        OPEN
    };

    static SerialInterfaceBase* get_interface_instance(const std::string& _serial_port_file,int _baud_rate);


    virtual bool init() = 0;
    virtual bool close() = 0; //CLOSES THE SERIAL OR TCP PORT
    virtual SERIAL_PORT_STATE get_state() = 0;
    virtual void dummy_read() = 0;
    virtual std::string read_line() = 0;
    virtual std::string read_until(char _termination, unsigned int _max_wait) = 0;
    virtual bool write_string(std::string _data) = 0;
    virtual bool is_open() = 0;
protected:

    static void tokenize(std::string const &str, const char *delim, std::vector<std::string> &out);

};


#endif //CONTROLLER_SERIALINTERFACEBASE_H
