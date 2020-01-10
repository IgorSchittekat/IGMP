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

void IgmpRouter::scheduleGroupSpecificQuery(Timer* t, void* v) {
    TimerData* tData = (TimerData*) v;
    tData->router->sendGroupSpecificQuery(tData->src, tData->dest, tData->sFlag);
    if (tData->count-- > 0) {
        t->schedule_after_sec(tData->router->getLMQI());
    }
    else {
        delete t;
        delete tData;
    }
}

void IgmpRouter::sendGroupSpecificQuery(IPAddress temp_src, IPAddress dest, int s) {
    int headroom = sizeof(click_ether) + 4;
    
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
	mq->S = s;
    mq->QRV = getRobustnessVariable();
    mq->QQIC = 20;
    mq->NumberOfSources = 0;


    mq->Checksum = click_in_cksum((unsigned char *) mq, sizeof(MembershipQuery));

    q->set_dst_ip_anno(IPAddress(dest));
    q->set_ip_header(iph, sizeof(click_ip));
    q->timestamp_anno().assign_now();
    output(0).push(q);
}

void IgmpRouter::timerCallback(Timer* t, void* v) {
    IgmpRouter::Group* group = (IgmpRouter::Group*)v;
    group->filterMode = INCLUDE;
}

bool IgmpRouter::multicastExists(IPAddress mult_addr) {
    for (auto it = groupsMap.begin(); it != groupsMap.end(); it++) {
        auto groups = it->second;
        for (auto itt = groups->begin(); itt != groups->end(); itt++) {
            if ((*itt)->mult_addr == mult_addr) {
                return true;
            }
        }
    }
    return false;
}

bool IgmpRouter::acceptSource(IPAddress dest, IPAddress client, IPAddress client_mask) {
    for (auto it = groupsMap.begin(); it != groupsMap.end(); it++) {
        if (it->first.matches_prefix(client, client_mask)) {
            Vector<IgmpRouter::Group*>* groups = it->second;
            for (auto itt = groups->begin(); itt != groups->end(); itt++) {
                if ((*itt)->mult_addr == dest) {
                    if ((*itt)->filterMode == EXCLUDE) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void IgmpRouter::isExclude(IPAddress client, IPAddress mult_addr) {
    if (groupsMap.find(client) == groupsMap.end()) {
        Vector<IgmpRouter::Group*>* newGroups = new Vector<IgmpRouter::Group*>();
        groupsMap.set(client, newGroups);
    }
    Vector<IgmpRouter::Group*>* groups = groupsMap.get(client);

    bool exists = false;
    for (auto it = groups->begin(); it != groups->end(); it++) {
        if ((*it)->mult_addr == mult_addr) {
            exists = true;
            (*it)->filterMode = EXCLUDE;
            (*it)->timer->schedule_after_sec(getGroupMembershipInterval());
        }
    }
    if (!exists) {
        Timer* t = new Timer(this);
        IgmpRouter::Group* newGroup = new IgmpRouter::Group(mult_addr, t, EXCLUDE);
        newGroup->timer->initialize(this);
        newGroup->timer->assign(this->timerCallback, newGroup);
        newGroup->timer->schedule_after_sec(getGroupMembershipInterval());
        groups->push_back(newGroup);
    }
}

void IgmpRouter::toExclude(IPAddress client, IPAddress mult_addr) {
    click_chatter("to_EX");
    if (groupsMap.find(client) == groupsMap.end()) {
        Vector<IgmpRouter::Group*>* newGroups = new Vector<IgmpRouter::Group*>();
        groupsMap.set(client, newGroups);
    }
    Vector<IgmpRouter::Group*>* groups = groupsMap.get(client);

    bool exists = false;
    for (auto it = groups->begin(); it != groups->end(); it++) {
        if ((*it)->mult_addr == mult_addr) {
            exists = true;
            (*it)->filterMode = EXCLUDE;
            (*it)->timer->schedule_after_sec(getGroupMembershipInterval());
        }
    }
    if (!exists) {
        Timer* t = new Timer(this);
        IgmpRouter::Group* newGroup = new IgmpRouter::Group(mult_addr, t, EXCLUDE);
        newGroup->timer->initialize(this);
        newGroup->timer->assign(this->timerCallback, newGroup);
        newGroup->timer->schedule_after_sec(getGroupMembershipInterval());
        groups->push_back(newGroup);
    }
    s_map.set(client, 0);
}


void IgmpRouter::toInclude(IPAddress client, IPAddress mult_addr) {
    if (groupsMap.find(client) != groupsMap.end()) {
        Vector<IgmpRouter::Group*>* groups = groupsMap.get(client);

        for (auto it = groups->begin(); it != groups->end(); it++) {
            if ((*it)->mult_addr == mult_addr) {
                (*it)->timer->schedule_after_sec(getLMQT());

                int s;
                if (s_map.find(client) == s_map.end()) {
                    s = 0;
                }
                else {
                    s = s_map.get(client);
                }
                TimerData* tData = new TimerData(client, mult_addr, this, this->getLMQC() - 1, s);
                Timer* t = new Timer(this);
                t->initialize(this);
                t->assign(this->scheduleGroupSpecificQuery, tData);
                t->schedule_now();
                s_map.set(client, 1);
            }
        }
    }
}




CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouter)
