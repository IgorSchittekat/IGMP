/*
 * SetIGMPChecksum.{cc,hh} -- element sets IGMP header checksum
 * Wannes Marynen
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 * Copyright (c) 2012 Eddie Kohler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include <click/glue.hh>
#include <clicknet/ip.h>
#include "SetIGMPChecksum.hh"
#include "IGMP_Packets.h"

CLICK_DECLS

SetIGMPChecksum::SetIGMPChecksum() {}

SetIGMPChecksum::~SetIGMPChecksum() {
}

Packet *
SetIGMPChecksum::simple_action(Packet *p_in) {
    if (WritablePacket * p = p_in->uniqueify()) {
        int numberOfSourceAdresses = 0;
        auto iph = (click_ip *) p->data();
        auto ra = (RouterAlert *) (iph + 1);
        auto mr = (MembershipReport *) (ra + 1);
        int numberOfGroupRecords = ntohs(mr->M);
        mr->Checksum = click_in_cksum((unsigned char *) mr, sizeof(MembershipReport) +
                                                            sizeof(GroupRecord) * numberOfGroupRecords +
                                                            numberOfGroupRecords * numberOfSourceAdresses *
                                                            sizeof(in_addr));

        return p;
    }
    return 0;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(SetIGMPChecksum)

ELEMENT_MT_SAFE(SetIGMPChecksum)
