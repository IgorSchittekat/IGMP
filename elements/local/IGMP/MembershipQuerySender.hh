#ifndef CLICK_MEMBERSHIPQUERYSENDER_HH
#define CLICK_MEMBERSHIPQUERYSENDER_HH

#include <click/element.hh>

CLICK_DECLS

class MembershipQuerySender:  public Element {

public:
    MembershipQuerySender();

    ~MembershipQuerySender();

    const char *class_name() const { return "MembershipQuerySender"; }

    const char *port_count() const { return "0/1"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void run_timer(Timer *);

private:
    Packet *make_packet();
    in_addr ip_addr;
    in_addr dst_addr;
    int _time = 100;
};

CLICK_ENDDECLS


#endif //CLICK_MEMBERSHIPQUERYSENDER_HH
