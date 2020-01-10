#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/glue.hh>
#include "IGMP_Packets.h"
#include "IGMPClientResponder.hh"


IGMPClientResponder::IGMPClientResponder() {
}

IGMPClientResponder::~IGMPClientResponder() {}

int IGMPClientResponder::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("ip", ip_addr).complete() < 0) return -1;
    return 0;
}

void IGMPClientResponder::push(int port, Packet *p) {
    if (port == 0) {
        auto iph = (click_ip *) p->data();
        auto ra = (RouterAlert *) (iph + 1);
        auto mr = (MembershipReport *) (ra + 1);
        int numberOfGroupRecords = ntohs(mr->M);
        auto rec = (GroupRecord*) (mr+1);
        for (int i = 0; i < numberOfGroupRecords; i++) {
            auto newrec = new GroupRecord;
            newrec->N = rec->N;
            newrec->auxDataLen = rec->auxDataLen;
            newrec->Record_Type = rec->Record_Type;
            newrec->MulticastAddress = rec->MulticastAddress;
            if(rec->Record_Type == IGMP_CHANGE_TO_EXCLUDE_MODE && timers.count(rec->MulticastAddress) == 0){

                if(QRV-1 > 0){
                    Timer *timer = new Timer(this);
                    timer->initialize(this);
                    auto num = click_random(1, 1 * 1000);
                    timer->schedule_after_msec(num);
                    timers.find_insert(rec->MulticastAddress, timer);
                    Pair<Packet*, int> pair;
                    pair.first = p->clone()->uniqueify();
                    pair.second = QRV-1;
                    ressends.find_insert(timer, pair);
                }
                records.push_back(newrec);
            }else if(rec->Record_Type == IGMP_CHANGE_TO_INCLUDE_MODE && timers.count(rec->MulticastAddress) == 1) {
                timers.erase(rec->MulticastAddress);
                Timer *timer = new Timer(this);
                timer->initialize(this);
                auto num = click_random(1, 1 * 1000);
                timer->schedule_after_msec(num);
                timers.find_insert(rec->MulticastAddress, timer);
                Pair<Packet*, int> pair;
                pair.first = p->clone()->uniqueify();
                pair.second = QRV-1;
                ressends.find_insert(timer, pair);
                for (auto it = records.begin(); it != records.end(); ) {
                    GroupRecord* deleterec= *it;
                    if (deleterec->MulticastAddress == rec->MulticastAddress) {
                        it = records.erase(it);
                    }
                    else {
                        it++;
                    }
                }
            }
            rec = (GroupRecord*)rec + 1;
        }
        output(1).push(p);
    } else {
        auto iph = (click_ip *) p->data();
        if(iph->ip_p == IP_PROTO_IGMP){
            iph->ip_id++;
            auto ra = (RouterAlert *) (iph + 1);
            auto mq = (MembershipQuery *) (ra + 1);
            if (mq->Type == IGMP_QUERY_TYPE) {
                uint16_t checksum = mq->Checksum;
                mq->Checksum = 0;
                if (checksum == click_in_cksum((unsigned char *) mq, p->length() - sizeof(click_ip) - sizeof(RouterAlert) )) {
                    auto num = click_random(1, mq->Max_respond_code * 100);
                    Timer* generalTimer = new Timer(this);
                    generalTimer->initialize(this);
                    generalTimer->assign(this->timerCallback, this);
                    generalTimer->schedule_after_msec(num);
                }
            }
        }
        else{
            auto iph = (click_ip *) p->data();
            IPAddress t = iph->ip_dst;
            if(timers.count(iph->ip_dst) != 0){
                output(2).push(p);
            }
            output(0).push(p);
        }
    }
}

void IGMPClientResponder::timerCallback(Timer* t, void* v) {
    IGMPClientResponder* resp = (IGMPClientResponder*)v;
    if(resp->records.size() > 0) {
        resp->output(1).push(resp->make_packet());
    }
    delete t;
}


void IGMPClientResponder::run_timer(Timer *timer) {

    auto item = ressends.find(timer);
    output(1).push(item->second.first->clone()->uniqueify());
    if(item->second.second - 1 == 0){
        ressends.erase(item);
    }else{
        auto num = click_random(1, 1 * 1000);
        timer->reschedule_after_msec(num);
        item->second.second--;
    }
}

Packet *IGMPClientResponder::make_packet() {
    int headroom = sizeof(click_ether) + 4;
    int numberOfGroupRecords = records.size();

    int size = sizeof(click_ip) + sizeof(MembershipReport) + sizeof(RouterAlert) +
               sizeof(GroupRecord) * numberOfGroupRecords;

    WritablePacket *q = Packet::make(headroom, 0, size, 0);
    if (q == 0) {
        return 0;
    }
    memset(q->data(), '\0', size);

    click_ip *iph = (click_ip *) q->data();
    iph->ip_v = 4;
    iph->ip_hl = (sizeof(click_ip) + sizeof(RouterAlert)) >> 2;
    iph->ip_len = htons(q->length());
    iph->ip_id = htons(1);
    iph->ip_ttl = 1;
    iph->ip_p = IP_PROTO_IGMP;
    iph->ip_src = ip_addr;
    iph->ip_dst = dst_addr;
    iph->ip_tos = 0xc0;

    auto ra = (RouterAlert *) (iph + 1);
    ra->Type = 148;
    ra->Length = 4;

    auto *mr = (MembershipReport *) (ra + 1);
    mr->Type = 34;
    mr->M = htons(numberOfGroupRecords);

    auto addr = (GroupRecord *) (mr + 1);

    for (auto i = records.begin(); i < records.end(); i++) {
        auto t = *i;
        addr->MulticastAddress = t->MulticastAddress;
        addr->Record_Type = IGMP_MODE_IS_EXCLUDE;
        addr->auxDataLen = t->auxDataLen;
        addr->N = t->N;
        addr = (GroupRecord *) addr + 1;
    }

    iph->ip_sum = click_in_cksum((unsigned char *) iph, sizeof(click_ip) + sizeof(RouterAlert));

    mr->Checksum = click_in_cksum((unsigned char *) mr, sizeof(MembershipReport) +
                                                        sizeof(GroupRecord) * numberOfGroupRecords);

    q->set_dst_ip_anno(IPAddress(dst_addr));
    q->set_ip_header(iph, sizeof(click_ip));
    q->timestamp_anno().assign_now();

    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientResponder)