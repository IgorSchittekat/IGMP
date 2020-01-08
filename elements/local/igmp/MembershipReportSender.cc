#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "MembershipReportSender.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
MembershipReportSender::MembershipReportSender() {}

MembershipReportSender::~ MembershipReportSender() {}

int MembershipReportSender::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("IP_ADDR", ip_addr).read_mp("DST_ADDR", dst_addr).complete() < 0) return -1;

    Timer *timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(_time);
    return 0;
}

Packet *MembershipReportSender::make_packet() {
    int headroom = sizeof(click_ether);
    int numberOfGroupRecords = 2;
    int numberOfSourceAdresses = 3;
    Vector <in_addr> ip_list;

    for (int i = 0; i < numberOfSourceAdresses; i++) {
        ip_list.push_back(dst_addr);
    }

    int size = sizeof(click_ip) + sizeof(MembershipReport) + sizeof(GroupRecord) * numberOfGroupRecords +
               numberOfGroupRecords * numberOfSourceAdresses * sizeof(in_addr) + sizeof(RouterAlert);

    WritablePacket *q = Packet::make(headroom, 0, size, 0);
    if (q == 0) {
        return 0;
    }
    memset(q->data(), '\0', size);

    click_ip *iph = (click_ip *) q->data();
    iph->ip_v = 4;
    iph->ip_hl = sizeof(click_ip) >> 2;
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

    iph->ip_sum = click_in_cksum((unsigned char *) iph, sizeof(click_ip) + sizeof(RouterAlert));

    auto *mr = (MembershipReport *) (ra + 1);
    mr->Type = 34;
    mr->M = htons(numberOfGroupRecords);

    auto addr = (GroupRecord *) (mr + 1);

    for (int i = 0; i < numberOfGroupRecords; i++) {
        addr->Record_Type = IGMP_MODE_IS_EXCLUDE;
        addr->N = htons(numberOfSourceAdresses);
        addr->auxDataLen = 0;
        addr->MulticastAddress = ip_addr;
        auto ip_addr = (in_addr * )(addr + 1);
        for (auto j : ip_list) {
            *ip_addr = j;
            ip_addr = (in_addr * )(ip_addr + 1);
        }
        addr = (GroupRecord *) (ip_addr);
    }

    mr->Checksum = click_in_cksum((unsigned char *) mr, sizeof(MembershipReport) +
                                                        sizeof(GroupRecord) * numberOfGroupRecords +
                                                        numberOfGroupRecords * numberOfSourceAdresses *
                                                        sizeof(in_addr));

    q->set_dst_ip_anno(IPAddress(dst_addr));
    q->set_ip_header(iph, sizeof(click_ip));
    q->timestamp_anno().assign_now();

    return q;
}

void MembershipReportSender::run_timer(Timer *timer) {
    if (Packet * q = make_packet()) {
        click_chatter("Got a packet");
        output(0).push(q);
        timer->reschedule_after_msec(_time);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MembershipReportSender)
