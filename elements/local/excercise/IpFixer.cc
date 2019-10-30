#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IpFixer.hh"

CLICK_DECLS
IpFixer::IpFixer()
{}

IpFixer::~ IpFixer()
{}

int IpFixer::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_m("IP", ip).read_m("TOFIX", toFix).complete() < 0) return -1;
    //if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

void IpFixer::push(int, Packet* p){
    WritablePacket *q;
    q = p->uniqueify();
    click_ip* iph = q->ip_header();
    if(this->toFix == "DST"){
        iph->ip_dst = this->ip;
        q->set_dst_ip_anno(this->ip);
    } else if(this->toFix == "SRC"){
        iph->ip_src = this->ip;
    }

    int hlen = iph->ip_hl << 2;
    iph->ip_sum = 0;
    iph->ip_sum = click_in_cksum((unsigned char *)iph, hlen);
    click_chatter("Got a packet of size %d",iph->ip_sum);
    output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IpFixer)
