#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "NullPush.hh"

CLICK_DECLS
NullPush::NullPush()
{}

NullPush::~ NullPush()
{}

int NullPush::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

void NullPush::push(int, Packet* p){
    click_chatter("Got a packet of size %d",p->length());
    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NullPush)
