#ifndef CLICK_MEMBERSHIPREPORTSENDER_HH
#define CLICK_MEMBERSHIPREPORTSENDER_HH

#include <click/element.hh>

CLICK_DECLS

enum IGMPSTATUS {JOIN, LEAVE};

class MembershipReportSender:  public Element {

public:
    MembershipReportSender();

    ~MembershipReportSender();

    const char *class_name() const { return "MembershipReportSender"; }

    const char *port_count() const { return "0/1"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void run_timer(Timer *);

    static int join(const String &conf, Element* e, void * thunk, ErrorHandler * errh);

    static int leave(const String &conf, Element* e, void * thunk, ErrorHandler * errh);

    void add_handlers();

private:
    Packet *make_packet();
    Packet *make_join_leave_packet(IGMPSTATUS, in_addr);
    in_addr ip_addr;
    in_addr dst_addr;
    int _time = 1000;

    int currentmode = -1;

    Vector<in_addr> list;
};


#endif //CLICK_MEMBERSHIPREPORTSENDER_HH
