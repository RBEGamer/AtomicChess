//
// Created by prodevmo on 12.06.21.
//

#include "UDPDiscovery.h"



UDPDiscovery::UDPDiscovery(std::string _user_data){
    if(_user_data.empty()){
        user_data = user_data_default;
    }
    user_data = _user_data;
}
bool UDPDiscovery::enable_service(bool _en){

if(_en){

    parameters.set_port(kPort);
    parameters.set_application_id(kApplicationId);
    parameters.set_can_discover(false);
    parameters.set_can_be_discovered(true);
    parameters.set_can_use_broadcast(true);
    parameters.set_multicast_group_address(kMulticastAddress);
    parameters.set_can_use_multicast(true);


    if (!peer.Start(parameters, user_data)) {
       return false;
  }
    return true;

}else{
       peer.Stop(true);
       return true;
}


}
