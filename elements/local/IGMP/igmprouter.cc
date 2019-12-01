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



void IgmpRouter::add(IPAddress client, IPAddress mult_addr) {
    if (statesMap.find(client) == statesMap.end()) {
        Vector<IgmpRouter::State>* newStates = new Vector<IgmpRouter::State>();
        statesMap.set(client, newStates);
    }
    Vector<IgmpRouter::State>* states = statesMap.get(client);

    bool exists = false;
    Vector<IgmpRouter::State>::iterator it;
    for (it = states->begin(); it != states->end(); it++) {
        if (it->mult_addr == mult_addr) {
            exists = true;
        }
    }
    if (!exists) {
        IgmpRouter::State newState = {mult_addr, Timer(), INCLUDE, Vector<SourceRecord>()};
        states->push_back(newState);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouter)
