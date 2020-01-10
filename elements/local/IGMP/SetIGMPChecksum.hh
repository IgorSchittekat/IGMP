//
// Created by wannes on 03.12.19.
//

#ifndef IGMP_SETIGMPCHECKSUM_HH
#define IGMP_SETIGMPCHECKSUM_HH
#include <click/element.hh>
#include <click/glue.hh>
CLICK_DECLS


class SetIGMPChecksum : public Element{
public:
    SetIGMPChecksum() CLICK_COLD;
    ~SetIGMPChecksum() CLICK_COLD;

    const char *class_name() const		{ return "SetIGMPChecksum"; }
    const char *port_count() const		{ return PORTS_1_1; }

    Packet *simple_action(Packet *p);

};

CLICK_ENDDECLS
#endif //IGMP_SETIGMPCHECKSUM_HH

