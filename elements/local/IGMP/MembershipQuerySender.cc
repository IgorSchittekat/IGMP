#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "MembershipQuerySender.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
MembershipQuerySender::MembershipQuerySender() {}

MembershipQuerySender::~ MembershipQuerySender() {}

int MembershipQuerySender::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("IP_ADDR", ip_addr).read_mp("DST_ADDR", dst_addr).complete() < 0) return -1;

    Timer *timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(_time);
    return 0;
}

Packet *MembershipQuerySender::make_packet() {
    int headroom = sizeof(click_ether);
    int numberOfSourceAdresses = 0;
    Vector <in_addr> ip_list;

    for (int i = 0; i < numberOfSourceAdresses; i++) {
        ip_list.push_back(dst_addr);
    }

    int size = sizeof(click_ip) + sizeof(MembershipQuery) +
               sizeof(in_addr) * numberOfSourceAdresses + sizeof(RouterAlert);

    WritablePacket *q = Packet::make(headroom, 0, size, 0);
    if (q == 0) {
        return 0;
    }
    memset(q->data(), '\0', size);

    click_ip *iph = (click_ip *) q->data();
    iph->ip_v = 4;
    iph->ip_hl = (sizeof(click_ip) + sizeof(RouterAlert)) >> 2;
    iph->ip_len = htons(q->length());
    iph->ip_id = htons(0);
    iph->ip_ttl = 1;
    iph->ip_p = IP_PROTO_IGMP;
    iph->ip_src = ip_addr;
    iph->ip_dst = dst_addr;
    iph->ip_tos = 0xc0;

    auto ra = (RouterAlert * )(iph + 1);
    ra->Type = 148;
    ra->Length = 4;

    iph->ip_sum = click_in_cksum((unsigned char *) iph, sizeof(click_ip) + sizeof(ra));


    auto *mq = (MembershipQuery *) (ra + 1);
    mq->Type = 17;
    //mq->group_addr = ip_addr;
    mq->Max_respond_code = 120;
    mq->NumberOfSources = htons(numberOfSourceAdresses);
    mq->QQIC = 20;
    mq->QRV = 2;

    auto addr = (in_addr * )(mq + 1);

    for (auto i : ip_list) {
        *addr = i;
        addr = (in_addr * )(addr + 1);
    }


    mq->Checksum = click_in_cksum((unsigned char *) mq,
                                  sizeof(MembershipQuery) + sizeof(in_addr) * numberOfSourceAdresses);

    q->set_dst_ip_anno(IPAddress(dst_addr));
    q->set_ip_header(iph, sizeof(click_ip));
    q->timestamp_anno().assign_now();

    return q;
}

void MembershipQuerySender::run_timer(Timer *timer) {
    if (Packet * q = make_packet()) {
        click_chatter("Got a packet");
        output(0).push(q);
        timer->reschedule_after_msec(_time);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MembershipQuerySender)
