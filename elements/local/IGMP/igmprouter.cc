#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "igmprouter.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
IgmpRouter::IgmpRouter() {}

IgmpRouter::~ IgmpRouter() {}

int IgmpRouter::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

void IgmpRouter::push(int i, Packet * p) {
    
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouter)
