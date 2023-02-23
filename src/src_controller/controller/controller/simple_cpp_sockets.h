// Willy Nolan 2017 https://github.com/computersarecool/cpp_sockets MIT LICENCE
// MODIFIED VERSION by Marcel Ochsendorf 23.02.2023
// ADDED RETURN OF REC BYTES IN SEND REC FUNCTIONS TO IMPLEMENT REQUIRED WAIT FOR BYTES FUNCTION
/*
#ifndef _SIMPLE_CPP_SOCKETS_SOCKET_H_
#define _SIMPLE_CPP_SOCKETS_SOCKET_H_

#include <string>
#include <iostream>


#include <sys/socket.h>
#include <arpa/inet.h> // This contains inet_addr
#include <unistd.h> // This contains close
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
typedef int SOCKET;



// These could also be enums

const int connection_err = 5;
const int message_send_err = 6;
const int receive_err = 7;

class Socket {
public:
    enum class SocketType {
        TYPE_STREAM = SOCK_STREAM,
        TYPE_DGRAM = SOCK_DGRAM
    };

protected:
    explicit Socket();
    ~Socket();
    SOCKET m_socket;
    sockaddr_in m_addr;
    void set_port(u_short port);
    int set_address(const std::string& ip_address);

private:

};



Socket::Socket() : m_socket(), m_addr()
{

    // Create the socket handle
    m_socket = socket(AF_INET, static_cast<int>(SOCK_STREAM), 0);
    if (m_socket == INVALID_SOCKET)
    {
        throw std::runtime_error("Could not create socket");
    }

    m_addr.sin_family = AF_INET;


}

void Socket::set_port(u_short port)
{
    m_addr.sin_port = htons(port);
}

int Socket::set_address(const std::string& ip_address)
{
    return inet_pton(AF_INET, ip_address.c_str(), &m_addr.sin_addr);
}

Socket::~Socket() = default;



class TCPClient : public Socket
{
public:
    TCPClient(u_short port = 8000, const std::string& ip_address = "127.0.0.1");
    int make_connection();
    int send_message(const std::string& message);
};



TCPClient::TCPClient(u_short port, const std::string& ip_address) : Socket(SocketType::SOCK_STREAM)
{
    set_address(ip_address);
    set_port(port);
    std::cout << "TCP client created." << std::endl;
}

int TCPClient::make_connection()
{
    std::cout << "Connecting" << std::endl;
    if (connect(m_socket, reinterpret_cast<sockaddr*>(&m_addr), sizeof(m_addr)) < 0)
    {
        std::cout << "Connection error" << std::endl;
        return connection_err;
    }
    std::cout << "connected" << std::endl;

    return 0;
}

int TCPClient::send_message(const std::string& message)
{
    char server_reply[2000];
    size_t length = message.length();
    if (send(m_socket, message.c_str(), length, 0) < 0)
    {
        std::cout << "Send failed" << std::endl;
        return message_send_err;
    }
    else
    {
        std::cout << "Data sent" << std::endl;
    }
    if (recv(m_socket, server_reply, 2000, 0) == SOCKET_ERROR)
    {
        std::cout << "Receive Failed" << std::endl;
        return receive_err;
    }
    else
    {
        std::cout << server_reply << std::endl;
    }
    return 0;
}
*/