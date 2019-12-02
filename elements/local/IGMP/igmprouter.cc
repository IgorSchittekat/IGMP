#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "IgmpRouter.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
IgmpRouter::IgmpRouter() {}

IgmpRouter::~ IgmpRouter() {}

int IgmpRouter::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}


bool IgmpRouter::multicastExists(IPAddress mult_addr) {
    for (auto it = statesMap.begin(); it != statesMap.end(); it++) {
        auto states = it->second;
        for (auto itt = states->begin(); itt != states->end(); itt++) {
            if (itt->mult_addr == mult_addr) {
                return true;
            }
        }
    }
    return false;
}

bool IgmpRouter::acceptSource(IPAddress dest, IPAddress client, IPAddress client_mask) {
    for (auto it = statesMap.begin(); it != statesMap.end(); it++) {
        if (it->first.matches_prefix(client, client_mask)) {
            Vector<IgmpRouter::State>* states = it->second;
            for (auto itt = states->begin(); itt != states->end(); itt++) {
                click_chatter("mult: %d", itt->mult_addr);
                if (itt->mult_addr == dest) {
                    return true;
                }
            }
        }
    }
    return false;
}


void IgmpRouter::add(IPAddress client, IPAddress mult_addr) {
    if (statesMap.find(client) == statesMap.end()) {
        Vector<IgmpRouter::State>* newStates = new Vector<IgmpRouter::State>();
        statesMap.set(client, newStates);
    }
    Vector<IgmpRouter::State>* states = statesMap.get(client);

    bool exists = false;
    for (auto it = states->begin(); it != states->end(); it++) {
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
