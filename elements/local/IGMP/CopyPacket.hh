#ifndef CLICK_CopyPacket_HH
#define CLICK_CopyPacket_HH
#include <click/element.hh>
CLICK_DECLS

class CopyPacket : public Element { 
	public:
		CopyPacket();
		~CopyPacket();
		
		const char *class_name() const	{ return "CopyPacket"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

    	void push(int, Packet *);
	private:
};

CLICK_ENDDECLS
#endif
