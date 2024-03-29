// Router with three interfaces
// The input/output configuration is as follows:
//
// Input:
//	[0]: packets received on the 192.168.1.0/24 network
//	[1]: packets received on the 192.168.2.0/24 network
//	[2]: packets received on the 192.168.3.0/24 network
//
// Output:
//	[0]: packets sent to the 192.168.1.0/24 network
//	[1]: packets sent to the 192.168.2.0/24 network
//	[2]: packets sent to the 192.168.3.0/24 network
//  [3]: packets destined for the router itself

require(library definitions.click)

elementclass Router {
	$server_address, $client1_address, $client2_address |

	igmpCopyQuery::Tee(2);
	router::IgmpRouter() -> igmpCopyQuery;

	client1_forwarder::IgmpForwarder(ROUTER router, NET $client1_address:ipnet);
	client2_forwarder::IgmpForwarder(ROUTER router, NET $client2_address:ipnet);

	igmpCopyQuery[0]
		-> [1]client1_forwarder[1]
		-> FixIPSrc(router_client_network1_address)
		-> EtherEncap(0x0800, $client1_address:ether, multicast_client_address:eth)
		-> [1]output;

	igmpCopyQuery[1]
		-> [1]client2_forwarder[1]
		-> FixIPSrc(router_client_network2_address)
		-> EtherEncap(0x0800, $client2_address:ether, multicast_client_address:eth)
		-> [2]output;


	genQuery1 :: RouterGeneralQuerySender(router_server_network_address, multicast_broadcast_address)
	genQuery1 
		-> EtherEncap(0x0800, router_server_network_address:eth, multicast_broadcast_address:eth)
		-> [0]output;

	genQuery2 :: RouterGeneralQuerySender(router_client_network1_address, multicast_broadcast_address)
	genQuery2 
		-> EtherEncap(0x0800, router_client_network1_address:eth, multicast_broadcast_address:eth)
		->[1]output;

	genQuery3 :: RouterGeneralQuerySender(router_client_network2_address, multicast_broadcast_address)
	genQuery3 
		-> EtherEncap(0x0800, router_client_network2_address:eth, multicast_broadcast_address:eth)
		->[2]output;

	igmpCopy::Tee(2);
	igmpCopy[0]
		-> [0]client1_forwarder[0]
		-> EtherEncap(0x0800, $client1_address:ether, multicast_client_address:eth)
		-> [1]output;

	igmpCopy[1]
		-> [0]client2_forwarder[0]
		-> EtherEncap(0x0800, $client2_address:ether, multicast_client_address:eth)
		-> [2]output;

	// Shared IP input path and routing table
	ip :: Strip(14)
		-> CheckIPHeader
		-> igmp_class::IgmpClassifier()
		-> IgmpRouterChecker(ROUTER router);

	igmp_class[1] 
		-> multi_class::IPClassifier(224/4, -)
		-> igmpCopy;

	multi_class[1]
		-> rt :: StaticIPLookup(
					$server_address:ip/32 0,
					$client1_address:ip/32 0,
					$client2_address:ip/32 0,
					$server_address:ipnet 1,
					$client1_address:ipnet 2,
					$client2_address:ipnet 3);
	
	// ARP responses are copied to each ARPQuerier and the host.
	arpt :: Tee (3);

	// Input and output paths for interface 0
	input[0]
		-> HostEtherFilter($server_address)
		-> server_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
		-> ARPResponder($server_address)
		-> [0]output;

	server_arpq :: ARPQuerier($server_address) -> output;
	server_class[1] -> arpt[0] -> [1]server_arpq;
	server_class[2] -> Paint(1) -> ip;

	// Input and output paths for interface 1
	input[1]
		-> HostEtherFilter($client1_address)
		-> client1_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
		-> ARPResponder($client1_address)
		-> [1]output;

	client1_arpq :: ARPQuerier($client1_address) -> [1]output;
	client1_class[1] -> arpt[1] -> [1]client1_arpq;
	client1_class[2] -> Paint(2) -> ip;

	// Input and output paths for interface 2
	input[2]
		-> HostEtherFilter($client2_address)
		-> client2_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, -)
		-> ARPResponder($client2_address)
		-> [2]output;

	client2_arpq :: ARPQuerier($client2_address) -> [2]output;
	client2_class[1] -> arpt[2] -> [1]client2_arpq;
	client2_class[2] -> Paint(3) -> ip;


	// Local delivery
	rt[0] -> [3]output
	
	// Forwarding paths per interface
	rt[1]
		-> DropBroadcasts
		-> server_paint :: PaintTee(1)
		-> server_ipgw :: IPGWOptions($server_address)
		-> FixIPSrc($server_address)
		-> server_ttl :: DecIPTTL
		-> server_frag :: IPFragmenter(1500)
		-> server_arpq;
	
	server_paint[1] -> ICMPError($server_address, redirect, host) -> rt;
	server_ipgw[1]  -> ICMPError($server_address, parameterproblem) -> rt;
	server_ttl[1]   -> ICMPError($server_address, timeexceeded) -> rt;
	server_frag[1]  -> ICMPError($server_address, unreachable, needfrag) -> rt;


	rt[2]
		-> DropBroadcasts
		-> client1_paint :: PaintTee(2)
		-> client1_ipgw :: IPGWOptions($client1_address)
		-> FixIPSrc($client1_address)
		-> client1_ttl :: DecIPTTL
		-> client1_frag :: IPFragmenter(1500)
		-> client1_arpq;
	
	client1_paint[1] -> ICMPError($client1_address, redirect, host) -> rt;
	client1_ipgw[1]  -> ICMPError($client1_address, parameterproblem) -> rt;
	client1_ttl[1]   -> ICMPError($client1_address, timeexceeded) -> rt;
	client1_frag[1]  -> ICMPError($client1_address, unreachable, needfrag) -> rt;

	
	rt[3]
		-> DropBroadcasts
		-> client2_paint :: PaintTee(2)
		-> client2_ipgw :: IPGWOptions($client2_address)
		-> FixIPSrc($client2_address)
		-> client2_ttl :: DecIPTTL
		-> client2_frag :: IPFragmenter(1500)
		-> client2_arpq;
	
	client2_paint[1] -> ICMPError($client2_address, redirect, host) -> rt;
	client2_ipgw[1]  -> ICMPError($client2_address, parameterproblem) -> rt;
	client2_ttl[1]   -> ICMPError($client2_address, timeexceeded) -> rt;
	client2_frag[1]  -> ICMPError($client2_address, unreachable, needfrag) -> rt;
}
