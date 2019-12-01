#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IgmpClassifier.hh"
#include "igmp.h"
#include "click/timer.hh"
#include "clicknet/ether.h"

CLICK_DECLS


IgmpClassifier::IgmpClassifier()
{}

IgmpClassifier::~ IgmpClassifier()
{}

int IgmpClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
    IgmpRouter* r;
    int res = cp_va_kparse(conf, this, errh,
         "ROUTER", 0, cpElementCast, "IgmpRouter", &r,
    cpEnd);
    if(res < 0) return res;

    router = r;
	return 0;
}


void IgmpClassifier::push(int i, Packet * p) {
    const click_ip* iph = p->ip_header();
    if (iph->ip_p == IP_PROTO_IGMP) {
        output(0).push(p);
    }
    else if (router->multicastExists(iph->ip_dst)) {
        output(1).push(p);
    }
    else {
        output(2).push(p);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpClassifier)
