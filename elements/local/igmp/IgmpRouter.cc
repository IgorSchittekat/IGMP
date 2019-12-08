#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/packet_anno.hh>
#include "IgmpRouter.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
IgmpRouter::IgmpRouter() {}

IgmpRouter::~ IgmpRouter() {}

int IgmpRouter::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

void IgmpRouter::createGroupSpecificQuery(IPAddress temp_src, IPAddress dest) {
    int headroom = sizeof(click_ether);
    
    int size = sizeof(click_ip) + sizeof(MembershipQuery) + sizeof(RouterAlert);

    WritablePacket *q = Packet::make(headroom, 0, size, 0);
    if (q == 0) {
        return;
    }
    memset(q->data(), '\0', size);

    click_ip *iph = (click_ip *) q->data();
    iph->ip_v = 4;
    iph->ip_hl = (sizeof(click_ip) + sizeof(RouterAlert)) >> 2;
    iph->ip_len = htons(q->length());
    iph->ip_id = htons(0);
    iph->ip_ttl = 1;
    iph->ip_p = IP_PROTO_IGMP;
    iph->ip_src = temp_src;
    iph->ip_dst = dest;
    iph->ip_tos = 0xc0;
    SET_FIX_IP_SRC_ANNO(q, 1);

    auto ra = (RouterAlert * ) (iph + 1);
    ra->Type = 148;
    ra->Length = 4;

    iph->ip_sum = click_in_cksum((unsigned char *) iph, sizeof(click_ip) + sizeof(ra));


    auto *mq = (MembershipQuery *) (ra + 1);
    mq->Type = IGMP_QUERY_TYPE;
    mq->Max_respond_code = 0x32;
	mq->Checksum = 0;
    mq->group_addr = dest;
	mq->Resv = 0;
	mq->S = 0;
    mq->QRV = 2;
    mq->QQIC = 20;
    mq->NumberOfSources = 0;


    mq->Checksum = click_in_cksum((unsigned char *) mq, sizeof(MembershipQuery));

    q->set_dst_ip_anno(IPAddress(dest));
    q->set_ip_header(iph, sizeof(click_ip));
    q->timestamp_anno().assign_now();
    output(0).push(q);
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
    click_chatter("%d, %d", client, mult_addr); //192.268.2.1, 224.4.4.4
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
    createGroupSpecificQuery(client, mult_addr);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouter)
