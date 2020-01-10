///===========================================================================///
/// Definitions of the different hosts and related address information.

// The idea is that instead of sending the data to a unicast address, it should be sent to 
// a multicast address and all clients subscribed to this multicast group receive the UDP 
// packet and print this out.

AddressInfo(multicast_client_address 224.0.0.1 01:00:5e:00:00:01)
AddressInfo(multicast_client_ether 01:00:5e:04:04:04)
