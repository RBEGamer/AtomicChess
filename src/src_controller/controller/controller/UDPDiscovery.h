//
// Created by prodevmo on 12.06.21.
//

#ifndef CONTROLLER_UDPDISCOVERY_H
#define CONTROLLER_UDPDISCOVERY_H

#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>

#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/udp-discovery-cpp-master/udp_discovery_peer.hpp"


class UDPDiscovery {

public:
    UDPDiscovery(std::string _user_data);
    bool enable_service(bool _en);
private:
    const int kPort = 12021;
    const uint64_t kApplicationId = 7681412;
    const unsigned int kMulticastAddress = (224 << 24) + (0 << 16) + (0 << 8) + 123;  // 224.0.0.123
    const std::string user_data_default = "ATCTABLE";
    std::string user_data = "ATCTABLE";
   udpdiscovery::PeerParameters parameters;
   udpdiscovery::Peer peer;



};


#endif //CONTROLLER_UDPDISCOVERY_H
