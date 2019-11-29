#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IGMPTest.hh"
#include "igmp.h"
#include "click/timer.hh"
#include "clicknet/ether.h"

CLICK_DECLS


IGMPTest::IGMPTest()
{}

IGMPTest::~ IGMPTest()
{}

struct options {
    uint8_t type;
    uint8_t length;
    uint16_t data;
};

int IGMPTest::configure(Vector<String> &conf, ErrorHandler *errh) {
        
    if (Args(conf, this, errh)
        .read_mp("SRC", _srcIP)
        .read_mp("DST", _dstIP)
        .read_mp("LISTEN", _listenIP)
        .complete() < 0)
        return -1;
    Timer* timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);
	return 0;
}

Packet* IGMPTest::make_packet(){
    int headroom = sizeof(click_ether);
    WritablePacket* p = Packet::make(headroom, 0, sizeof(click_ip) + sizeof(membershipReport) + sizeof(struct groupRecord), 0);
    if (p == 0) {
        click_chatter("cannot make packet");
        return nullptr;
    }

    memset(p->data(), 0, sizeof(click_ip) + sizeof(membershipReport));

    click_ip* ip = (click_ip *)p->data();

    ip->ip_v = 4;
    ip->ip_hl = sizeof(click_ip) >> 2;
    ip->ip_len = htons(p->length());
    uint16_t ip_id = ((_sequence++) % 0xFFFF) + 1;
    ip->ip_id = htons(ip_id);
    ip->ip_p = IP_PROTO_IGMP; /* igmp */
    ip->ip_ttl = 1;
    ip->ip_src = _srcIP;
    ip->ip_dst = _dstIP;
    ip->ip_sum = click_in_cksum((unsigned char *)ip, sizeof(click_ip));

    membershipReport* report = (membershipReport*)(ip + 1);

    report->igmp_type = IGMP_REPORT_TYPE;
    report->resv = 0;
    report->igmp_cksum = 0;
    report->resv2 = 0;
    report->record_num = htons(1);

    groupRecord record;
    record.record_type = IGMP_CHANGE_TO_EXCLUDE_MODE;
    record.aux_data_len = 0;
    record.source_num = 0;
    record.multicast_address = _listenIP;

    report->records[0] = record;


    p->set_dst_ip_anno(_dstIP);
    
    report->igmp_cksum = click_in_cksum((unsigned char *)report, p->length()-sizeof(click_ip));

    return p;

}

void IGMPTest::run_timer(Timer* timer){
    if (Packet* q= make_packet()) {
        output(0).push(q);
        timer->reschedule_after_msec(1000);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPTest)
