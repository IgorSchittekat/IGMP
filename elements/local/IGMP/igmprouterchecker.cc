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
    click_chatter("packet in");
    WritablePacket* q = p->uniqueify();
    click_ether* eth = (click_ether *) q->data();
    click_ip* iph = (click_ip *) (eth + 1);
    if (iph->ip_p == IP_PROTO_IGMP) {
        click_chatter("IGMP packet in");
        RouterAlert* ra = (RouterAlert*)(iph + 1);
        MembershipQuery* mq = (MembershipQuery*)(ra + 1);
        // Reception of query
        click_chatter("%d", mq->Type);
        if (mq->Type == IGMP_QUERY_TYPE) {
            uint16_t checksum = mq->Checksum;
            mq->Checksum = 0;
            // check is checksum is correct
            if (checksum == click_in_cksum((unsigned char *) mq, q->length() - sizeof(click_ether) - sizeof(click_ip) - sizeof(RouterAlert) )) {
                
            }
        }
        // Reception of Report
        MembershipReport* mr = (MembershipReport*)(ra + 1);
        click_chatter("%d, %d", mr->Type, IGMP_REPORT_TYPE);
        if (mr->Type == IGMP_REPORT_TYPE) {
            uint16_t checksum = mr->Checksum;
            mr->Checksum = 0;
            // check is checksum is correct
            if (checksum == click_in_cksum((unsigned char *) mr, q->length() - sizeof(click_ether) - sizeof(click_ip) - sizeof(RouterAlert) )) {
                uint16_t M = htons(mr->M);
                click_chatter("%d", M);
                for (int i = 1; i <= M; i++) {
                    GroupRecord* rec = (GroupRecord*)(mr + i);
                    if (rec->Record_Type == IGMP_MODE_IS_INCLUDE) {
                        click_chatter("IS_INC");
                    }
                    else if (rec->Record_Type == IGMP_MODE_IS_EXCLUDE) {
                        click_chatter("IS_EXC");
                        
                    }
                    else if (rec->Record_Type == IGMP_CHANGE_TO_INCLUDE_MODE) {
                        click_chatter("TO_INC");
                        
                    }
                    else if (rec->Record_Type == IGMP_CHANGE_TO_EXCLUDE_MODE) {
                        click_chatter("TO_EXC");
                        
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
