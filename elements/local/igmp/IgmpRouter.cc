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
    for (auto it = groupsMap.begin(); it != groupsMap.end(); it++) {
        auto groups = it->second;
        for (auto itt = groups->begin(); itt != groups->end(); itt++) {
            if (itt->mult_addr == mult_addr) {
                return true;
            }
        }
    }
    return false;
}

bool IgmpRouter::acceptSource(IPAddress dest, IPAddress client, IPAddress client_mask) {
    for (auto it = groupsMap.begin(); it != groupsMap.end(); it++) {
        if (it->first.matches_prefix(client, client_mask)) {
            Vector<IgmpRouter::Group>* groups = it->second;
            for (auto itt = groups->begin(); itt != groups->end(); itt++) {
                if (itt->mult_addr == dest) {
                    if (itt->filterMode == EXCLUDE) {
                            return true;
                    }
                    else if (itt->filterMode == INCLUDE) {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}


void IgmpRouter::toExclude(IPAddress client, IPAddress mult_addr) {
    if (groupsMap.find(client) == groupsMap.end()) {
        Vector<IgmpRouter::Group>* newGroups = new Vector<IgmpRouter::Group>();
        groupsMap.set(client, newGroups);
    }
    Vector<IgmpRouter::Group>* groups = groupsMap.get(client);

    bool exists = false;
    for (auto it = groups->begin(); it != groups->end(); it++) {
        if (it->mult_addr == mult_addr) {
            exists = true;
            it->filterMode = EXCLUDE;
        }
    }
    if (!exists) {
        IgmpRouter::Group newGroup = {mult_addr, Timer(), EXCLUDE};
        groups->push_back(newGroup);
    }
}


void IgmpRouter::toInclude(IPAddress client, IPAddress mult_addr) {
    if (groupsMap.find(client) == groupsMap.end()) {
        Vector<IgmpRouter::Group>* newGroups = new Vector<IgmpRouter::Group>();
        groupsMap.set(client, newGroups);
    }
    Vector<IgmpRouter::Group>* groups = groupsMap.get(client);

    bool exists = false;
    for (auto it = groups->begin(); it != groups->end(); it++) {
        if (it->mult_addr == mult_addr) {
            exists = true;
            it->filterMode = INCLUDE;
        }
    }
    if (!exists) {
        IgmpRouter::Group newGroup = {mult_addr, Timer(), INCLUDE};
        groups->push_back(newGroup);
    }
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouter)
