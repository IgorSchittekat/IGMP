#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "igmprouterchecker.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
IgmpRouterChecker::IgmpRouterChecker() {}

IgmpRouterChecker::~ IgmpRouterChecker() {}

int IgmpRouterChecker::configure(Vector <String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

void IgmpRouterChecker::push(int, Packet * p) {
    WritablePacket* q = p->uniqueify();
    click_ip* iph = (click_ip *) q->data();
    if (iph->ip_p == IP_PROTO_IGMP) {
        // (MembershipQuery*)(iph + 1) is Router Alert
        MembershipQuery* mq = (MembershipQuery*)(iph + 2);
        // Reception of query
        if (mq->Type == IGMP_QUERY_TYPE) {
            uint16_t checksum = mr->Checksum;
            mr->Checksum = 0;
            // check is checksum is correct
            if (checksum == click_in_cksum((unsigned char *) mr, sizeof(mr))) {

            }
        }
        // Reception of Report
        else if (mq->Type == IGMP_REPORT_TYPE) {
            MembershipReport* mr = (MembershipReport*)(mq);
            uint16_t checksum = mr->Checksum;
            mr->Checksum = 0;
            // check is checksum is correct
            if (checksum == click_in_cksum((unsigned char *) mr, sizeof(mr))) {
                uint16_t M = mr->M;
                for (int i = 1; i <= M; i++) {
                    GroupRecord* rec = (GroupRecord*)(mr + i);
                    if (rec->Record_Type == IGMP_MODE_IS_INCLUDE) {

                    }
                    else if (rec->Record_Type == IGMP_MODE_IS_EXCLUDE) {
                        
                    }
                    else if (rec->Record_Type == IGMP_CHANGE_TO_INCLUDE_MODE) {
                        
                    }
                    else if (rec->Record_Type == IGMP_CHANGE_TO_EXCLUDE_MODE) {
                        
                    }
                }
            }
        }
    }
    else {
        output(0).push(p);
    }
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IgmpRouterChecker)
