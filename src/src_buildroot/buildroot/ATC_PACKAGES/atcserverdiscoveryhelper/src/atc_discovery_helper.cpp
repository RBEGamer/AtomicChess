#include <string.h>
#include <map>
#include <iostream>
#include "udp_discovery_peer.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

const int kPort = 12022;
const uint64_t kApplicationId = 7681412;
const unsigned int kMulticastAddress = (224 << 24) + (0 << 16) + (0 << 8) + 123; // 224.0.0.123


int main(int argc, char* argv[]) {
  udpdiscovery::PeerParameters parameters;

  parameters.set_can_use_broadcast(true);
  parameters.set_multicast_group_address(kMulticastAddress);
  parameters.set_can_use_multicast(true);
  parameters.set_can_be_discovered(true);

  std::string user_data = "ATCSERVER";
  
  parameters.set_port(kPort);
  parameters.set_application_id(kApplicationId);

  udpdiscovery::Peer peer;

  if (!peer.Start(parameters, user_data)) {
    return 1;
  }

 

  while (true) {

#if defined(_WIN32)
      Sleep(500);
#else
      usleep(500000);
#endif

  }

  peer.Stop(true);

  return 0;
}
