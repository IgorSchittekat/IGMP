#ifndef CLICK_IgmpClassifier_HH
#define CLICK_IgmpClassifier_HH
#include <click/element.hh>
#include <click/ipaddress.hh>
CLICK_DECLS

/**
 * Input[0]: Packets (Incl Ethernet header)
 * Output[0]: IGMP packet (Incl Ethernet header)
 * Output[1]: Other packets (Incl Ethernet header)
 */


class IgmpClassifier : public Element { 
public:
	IgmpClassifier();
	~IgmpClassifier();
	
	const char *class_name() const	{ return "IgmpClassifier"; }
	const char *port_count() const	{ return "1/2"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet *);
	
private:
	
};

CLICK_ENDDECLS
#endif
