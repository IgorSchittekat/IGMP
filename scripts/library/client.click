// Output configuration: 
//
// Packets for the network are put on output 0
// Packets for the host are put on output 1

require(library definitions.click)

elementclass Client {
	$address, $gateway |


	
	rt :: StaticIPLookup(
                  					$address:ip/32 0,
                  					$address:ipnet 0,
                  					0.0.0.0/0.0.0.0 $gateway 1)[1]
		-> DropBroadcasts
		-> ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> arpq :: ARPQuerier($address)
		-> output;

	igmpClient::IGMPClientResponder($address)
		->Discard;

	ip :: Strip(14)
		-> CheckIPHeader()
		-> [1]igmpClient;

	igmpClient[1]
		-> EtherEncap(0x0800, $address:ether, multicast_broadcast_router:eth)
		->[0]output;

	igmpClient[2]
		->rt
		-> [1]output;

	ipgw[1] -> ICMPError($address, parameterproblem) -> output;
	ttl[1]  -> ICMPError($address, timeexceeded) -> output;
	frag[1] -> ICMPError($address, unreachable, needfrag) -> output;
	
	// Incoming Packets
	input
		-> HostEtherFilter($address)
		-> in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> arp_res :: ARPResponder($address)
		-> output;

	in_cl[1] -> [1]arpq;
	in_cl[2] -> ip;

	igmp::MembershipReportSender($address)
		-> SetIPChecksum()
		-> SetIGMPChecksum()
		-> igmpClient;
}
