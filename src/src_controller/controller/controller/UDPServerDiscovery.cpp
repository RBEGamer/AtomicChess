//
// Created by prodevmo on 12.06.21.
//

#include "UDPServerDiscovery.h"
UDPServerDiscovery::UDPServerDiscovery(){

}
std::string UDPServerDiscovery::search_for_server( std::string _server_user_data,  int _retry_count,  int _delay){


            if(_server_user_data.empty()) {
                _server_user_data = "ATCSERVER";
            }
            if(_retry_count < 0){
                _retry_count = 3;
            }
            if(_delay){
                _delay = 1000;
            }

            parameters.set_port(kPort);
            parameters.set_application_id(kApplicationId);
            parameters.set_can_discover(true);
            parameters.set_can_be_discovered(false);
            parameters.set_can_use_broadcast(false);
            parameters.set_multicast_group_address(kMulticastAddress);
            parameters.set_can_use_multicast(true);

            std::list<udpdiscovery::DiscoveredPeer> discovered_peers;
            std::map<udpdiscovery::IpPort, std::string> last_seen_user_datas;

            if (!peer.Start(parameters, _server_user_data + "DISCREQ")) {
                LOG_F(ERROR,"UDPServerDiscovery::search_for_server START PEER FAILED");
                return "";
            }
    std::string found_ip = "";

    for(int i = 0; i < _retry_count; i++){
        std::list<udpdiscovery::DiscoveredPeer> new_discovered_peers = peer.ListDiscovered();
        discovered_peers = new_discovered_peers;

        last_seen_user_datas.clear();
        for (std::list<udpdiscovery::DiscoveredPeer>::const_iterator it = discovered_peers.begin(); it != discovered_peers.end(); ++it) {
            last_seen_user_datas.insert(std::make_pair((*it).ip_port(), (*it).user_data()));
        }

        if(discovered_peers.size() > 0){
            std::cout << "Discovered peers: " << discovered_peers.size() << std::endl;
        }
        for (std::list<udpdiscovery::DiscoveredPeer>::const_iterator it = discovered_peers.begin(); it != discovered_peers.end(); ++it) {
            std::cout << " - " << udpdiscovery::IpPortToString((*it).ip_port()) << ", " << (*it).user_data() << std::endl;
            //DEVICE DISCOCERED
            if ((*it).user_data() == _server_user_data)
            {

                found_ip = udpdiscovery::IpToString((*it).ip_port().ip());
                return found_ip;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_delay));
    }


            peer.Stop(true);
return found_ip;


}
