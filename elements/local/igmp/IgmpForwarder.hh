#ifndef CLICK_IgmpForwarder_HH
#define CLICK_IgmpForwarder_HH
#include <click/element.hh>
#include <click/ipaddress.hh>
#include "IgmpRouter.hh"
CLICK_DECLS

/**
 * Forwards packet if network asked for it
 * Input[0]: Multicast traffic
 * Input[1]: IGMP Group specific queries
 * Output[0]: Multicast traffic
 * Output[1]: IGMP Group specific queries
 */


class IgmpForwarder : public Element { 
public:
	IgmpForwarder();
	~IgmpForwarder();
	
	const char *class_name() const	{ return "IgmpForwarder"; }
	const char *port_count() const	{ return "0-2/0-2"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet *);
	
private:
	IgmpRouter* router;
	IPAddress net_addr;
	IPAddress net_mask;
	
};

CLICK_ENDDECLS
#endif
