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
    if (Args(conf, this, errh).complete() < 0) return -1;
	return 0;
}


void IgmpClassifier::push(int i, Packet * p) {
    const click_ip* iph = p->ip_header();
    if (iph->ip_p == IP_PROTO_IGMP) {
        output(0).push(p);
    }
    else {
        output(1).push(p);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpClassifier)
