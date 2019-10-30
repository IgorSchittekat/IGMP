#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "NullPull.hh"

CLICK_DECLS
NullPull::NullPull()
{}

NullPull::~ NullPull()
{}

int NullPull::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).complete() < 0) return -1;
    return 0;
}

Packet* NullPull::pull(int){
    Packet * p = input(0).pull();
    if(p == 0){
        return 0;
    }
    click_chatter("Got a packet of size %d",p->length());
    return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NullPull)
