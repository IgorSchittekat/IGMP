#ifndef CLICK_IGMPTEST_HH
#define CLICK_IGMPTEST_HH
#include <click/element.hh>
#include <click/ipaddress.hh>
CLICK_DECLS

class IGMPTest : public Element { 
public:
	IGMPTest();
	~IGMPTest();
	
	const char *class_name() const	{ return "IGMPTest"; }
	const char *port_count() const	{ return "0/1"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void run_timer(Timer*);
	
private:
	Packet* make_packet();
	IPAddress _srcIP;
	IPAddress _dstIP;
	uint32_t _sequence;
	IPAddress _listenIP;
	
};

CLICK_ENDDECLS
#endif
