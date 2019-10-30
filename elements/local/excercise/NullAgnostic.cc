#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "NullAgnostic.hh"

CLICK_DECLS
NullAgnostic::NullAgnostic()
{}

NullAgnostic::~ NullAgnostic()
{}

int NullAgnostic::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

Packet* NullAgnostic::pull(int port = 0){
    Packet * p = input(port).pull();
    click_chatter("Got a packet of size %d",p->length());
    return p;
}

void NullAgnostic::push(int, Packet* p){
    click_chatter("Got a packet of size %d",p->length());
    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NullAgnostic)
