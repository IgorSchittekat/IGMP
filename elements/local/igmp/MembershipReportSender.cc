#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include <click/ipaddress.hh>
#include "MembershipReportSender.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
MembershipReportSender::MembershipReportSender() {}

MembershipReportSender::~ MembershipReportSender() {}

int MembershipReportSender::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_mp("IP_ADDR", ip_addr).complete() < 0) return -1;
    return 0;
}

Packet *MembershipReportSender::make_join_leave_packet(IGMPSTATUS status, in_addr join_addr) {
    int headroom = sizeof(click_ether) + 4;
    int numberOfGroupRecords = 1;
    int numberOfSourceAdresses = 1;

    bool found = status == JOIN;

    for (auto i = list.begin(); i < list.end(); i++) {
        if (*i == join_addr) {
            if (status == JOIN) {
                click_chatter("already in list");
                return 0;
            } else if (status == LEAVE) {
                list.erase(i);
                found = true;
            }
        }
    }

    if (!found) {
        click_chatter("Not in list");
        return 0;
    }

    switch (status) {
        case JOIN:
            list.push_back(join_addr);
            break;
        case LEAVE:
            break;
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
    iph->ip_hl = (sizeof(click_ip) + sizeof(RouterAlert)) >> 2;
    iph->ip_len = htons(q->length());
    iph->ip_id = htons(1);
    iph->ip_ttl = 1;
    iph->ip_p = IP_PROTO_IGMP;
    iph->ip_src = ip_addr;
    iph->ip_dst = IPAddress("224.0.0.22");
    iph->ip_tos = 0xc0;

    auto ra = (RouterAlert *) (iph + 1);
    ra->Type = 148;
    ra->Length = 4;

    iph->ip_sum = click_in_cksum((unsigned char *) iph, sizeof(click_ip) + sizeof(RouterAlert));

    auto *mr = (MembershipReport *) (ra + 1);
    mr->Type = 34;
    mr->M = htons(1);

    auto addr = (GroupRecord *) (mr + 1);

    if (status == JOIN) {
        addr->Record_Type = IGMP_CHANGE_TO_EXCLUDE_MODE;
    } else if (status == LEAVE) {
        addr->Record_Type = IGMP_CHANGE_TO_INCLUDE_MODE;
    } else {
        click_chatter("ERROR");
    }
    addr->N = htons(0);
    addr->auxDataLen = 0;
    addr->MulticastAddress = join_addr;

    /*mr->Checksum = click_in_cksum((unsigned char *) mr, sizeof(MembershipReport) +
                                                        sizeof(GroupRecord) * numberOfGroupRecords +
                                                        numberOfGroupRecords * numberOfSourceAdresses *
                                                        sizeof(in_addr));
    */
    q->set_dst_ip_anno(IPAddress("224.0.0.22"));
    q->set_ip_header(iph, sizeof(click_ip));
    q->timestamp_anno().assign_now();

    return q;
}

void MembershipReportSender::sendPacket(Packet* p){
    output(0).push(p);
}

int MembershipReportSender::join(const String &conf, Element *e, void *thunk, ErrorHandler *errh) {
    MembershipReportSender *me = (MembershipReportSender *) e;
    IPAddress addr;
    Vector <String> vec;
    cp_argvec(conf, vec);
    if (Args(vec, e, errh).read_mp("IP ADDR", addr).complete() < 0)
        return -1;
    auto *send = reinterpret_cast<MembershipReportSender *>(e);
    if (Packet * q = send->make_join_leave_packet(JOIN, addr)) {
        click_chatter("\033[1;34mJoin [group %s]\033[0m", addr.unparse().c_str());
        me->output(0).push(q);
    }
    return 0;
}

int MembershipReportSender::leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh) {
    MembershipReportSender *me = (MembershipReportSender *) e;
    IPAddress addr;
    Vector <String> vec;
    cp_argvec(conf, vec);
    if (Args(vec, e, errh).read_mp("IP ADDR", addr).complete() < 0)
        return -1;
    auto *send = reinterpret_cast<MembershipReportSender *>(e);
    if (Packet * q = send->make_join_leave_packet(LEAVE, addr)) {
        click_chatter("\033[1;34mLeave [group %s]\033[0m", addr.unparse().c_str());
        me->output(0).push(q);


    }
    return 0;
}

void MembershipReportSender::add_handlers() {
    add_write_handler("join", join, (void *) 0);
    add_write_handler("leave", leave, (void *) 0);
}

#include <click/vector.cc>

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<in_addr>;
#endif

CLICK_ENDDECLS
EXPORT_ELEMENT(MembershipReportSender)
