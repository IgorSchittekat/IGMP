#ifndef CLICK_RouterGeneralQuerySender_HH
#define CLICK_RouterGeneralQuerySender_HH
#include <click/element.hh>
CLICK_DECLS

class RouterGeneralQuerySender : public Element { 
	public:
		RouterGeneralQuerySender();
		~RouterGeneralQuerySender();
		
		const char *class_name() const	{ return "RouterGeneralQuerySender"; }
		const char *port_count() const	{ return "0/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void run_timer(Timer*);
	private:
    Packet *make_packet();
    in_addr src_addr;
    in_addr dst_addr;
    int _time = 20000;
};

CLICK_ENDDECLS
#endif
