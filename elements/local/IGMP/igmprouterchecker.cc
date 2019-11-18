#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "igmprouterchecker.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
IgmpRouterChecker::IgmpRouterChecker() {}

IgmpRouterChecker::~ IgmpRouterChecker() {}

int IgmpRouterChecker::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

void IgmpRouterChecker::push(int, Packet * q) {
    click_ip *iph = (click_ip *) q->data();
    if (iph->ip_p == IP_PROTO_IGMP) {
        
    }
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouterChecker)
