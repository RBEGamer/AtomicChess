//
// Created by prodevmo on 12.06.21.
//

#ifndef CONTROLLER_UDPSERVERDISCOVERY_H
#define CONTROLLER_UDPSERVERDISCOVERY_H

#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/udp-discovery-cpp-master/udp_discovery_peer.hpp"


class UDPServerDiscovery {
public:
    UDPServerDiscovery();
    std::string search_for_server( std::string _server_user_data,  int _retry_count = 6,  int _delay = 1000);
private:
    const int kPort = 12022;
    const uint64_t kApplicationId = 7681412;
    const unsigned int kMulticastAddress = (224 << 24) + (0 << 16) + (0 << 8) + 123;  // 224.0.0.123
    udpdiscovery::PeerParameters parameters;
    udpdiscovery::Peer peer;


};


#endif //CONTROLLER_UDPSERVERDISCOVERY_H
