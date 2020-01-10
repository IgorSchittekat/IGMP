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
    generalTimer = new Timer(this);
}

IGMPClientResponder::~IGMPClientResponder() {}

int IGMPClientResponder::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("ip", ip_addr).complete() < 0) return -1;
    return 0;
}

void IGMPClientResponder::push(int port, Packet *p) {
    //click_chatter("Got a packet of size %d , on port %d", p->length(), port);
    if (port == 0) {
        //click_chatter("state change");
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
                    //click_chatter("timer : %d", num);
                    timer->schedule_after_msec(num);
                    timers.find_insert(rec->MulticastAddress, timer);
                    Pair<Packet*, int> pair;
                    pair.first = p->clone()->uniqueify();
                    pair.second = QRV-1;
                    ressends.find_insert(timer, pair);
                }
            }else if(rec->Record_Type == IGMP_CHANGE_TO_INCLUDE_MODE && timers.count(rec->MulticastAddress) == 1) {
                timers.erase(rec->MulticastAddress);
                Timer *timer = new Timer(this);
                timer->initialize(this);
                auto num = click_random(1, 1 * 1000);
                //click_chatter("timer : %d", num);
                timer->schedule_after_msec(num);
                timers.find_insert(rec->MulticastAddress, timer);
                Pair<Packet*, int> pair;
                pair.first = p->clone()->uniqueify();
                pair.second = QRV-1;
                ressends.find_insert(timer, pair);
            }
            records.push_back(newrec);
            rec = (GroupRecord*)rec + 1;
        }
        output(1).push(p);
    } else {
        //click_chatter("recieved packet");
        auto iph = (click_ip *) p->data();
        if(iph->ip_p == IP_PROTO_IGMP){
            iph->ip_id++;
            //click_chatter("responding");
            auto ra = (RouterAlert *) (iph + 1);
            auto mr = (MembershipQuery *) (ra + 1);
            auto num = click_random(1, mr->Max_respond_code * 100);
            //click_chatter("test");
            if(!generalTimer->initialized()){
                generalTimer->initialize(this);
            }
            //click_chatter("generalTimer : %d", num);
            generalTimer->schedule_after_msec(num);
        }
        else{
            auto iph = (click_ip *) p->data();
            IPAddress t = iph->ip_dst;
            click_chatter(t.unparse().c_str());
            if(timers.count(iph->ip_dst) != 0){
                click_chatter("accept");
                output(2).push(p);
            }
            output(0).push(p);
        }
    }
}


void IGMPClientResponder::run_timer(Timer *timer) {
//     if(records.size() > 0 && timer == generalTimer){
//         //click_chatter("Timer");
//         output(1).push(make_packet());
//         //records.clear();
//     } else if(timer != generalTimer){
//         auto item = ressends.find(timer);
//         output(1).push(item->second.first->clone()->uniqueify());
//         if(item->second.second - 1 == 0){
//             ressends.erase(item);
//         }else{
//             auto num = click_random(1, 1 * 1000);
//             timer->reschedule_after_msec(num);
//             item->second.second--;
//         }
//     }
//     //timer->reschedule_after_msec(_time);
}

Packet *IGMPClientResponder::make_packet() {
    int headroom = sizeof(click_ether);
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