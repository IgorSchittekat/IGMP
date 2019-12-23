#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IgmpForwarder.hh"
#include "igmp.h"
#include "click/timer.hh"
#include "clicknet/ether.h"

CLICK_DECLS


IgmpForwarder::IgmpForwarder()
{}

IgmpForwarder::~ IgmpForwarder()
{}

int IgmpForwarder::configure(Vector<String> &conf, ErrorHandler *errh) {
	IPAddress* addr = new IPAddress("0.0.0.0");
	IPAddress* mask = new IPAddress("0.0.0.0");
    IgmpRouter* r;
    int res = cp_va_kparse(conf, this, errh, 
         "NET", 0, cpIPPrefix, addr, mask,
         "ROUTER", 0, cpElementCast, "IgmpRouter", &r,
    cpEnd);
    if(res < 0) return res;


	this->net_addr = IPAddress(*addr);
	this->net_mask = IPAddress(*mask);
    router = r;
	return 0;
}


void IgmpForwarder::push(int i, Packet * p) {
    const click_ip* iph = (click_ip *) p->ip_header();
    if (i == 0) {
        IPAddress destination = iph->ip_dst;    
        if (router->acceptSource(destination, net_addr, net_mask)) {
            output(0).push(p);
        }
    }
    else if (i == 1) {
        IPAddress source = iph->ip_src;
        if (source.matches_prefix(net_addr, net_mask)) {
            output(1).push(p);
        }
    }
    p->kill();

}

CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpForwarder)
