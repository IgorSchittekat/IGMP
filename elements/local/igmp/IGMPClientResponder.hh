
#ifndef IGMP_IGMPCLIENTRESPONDER_HH
#define IGMP_IGMPCLIENTRESPONDER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>

//#include "IGMP_Packets.h"

struct  GroupRecord;

CLICK_DECLS

class IGMPClientResponder: public Element {

public:
    IGMPClientResponder();

    ~IGMPClientResponder();

    const char *class_name() const { return "IGMPClientResponder"; }

    const char *port_count() const { return "2/3"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet*);

    void run_timer(Timer*);

    static void timerCallback(Timer* t, void* v);
private:
    Packet* make_packet();
    HashTable<IPAddress, Timer*> timers;
    HashTable<Timer*, Pair<Packet*,int>> ressends;
    Vector<GroupRecord*> records;

    IPAddress ip_addr = IPAddress("0.0.0.0");
    IPAddress dst_addr = IPAddress("224.0.0.22");


    int QRV = 2;

};

CLICK_ENDDECLS

#endif IGMP_IGMPCLIENTRESPONDER_HH
