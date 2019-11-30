#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "CopyPacket.hh"
#include "IGMP_Packets.h"

CLICK_DECLS
CopyPacket::CopyPacket()
{}

CopyPacket::~ CopyPacket()
{}

int CopyPacket::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh).complete() < 0) return -1;
	return 0;
}


void CopyPacket::push(int, Packet * p) {
    output(0).push(p);
    output(1).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CopyPacket)
