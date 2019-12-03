#ifndef CLICK_IgmpRouter_HH
#define CLICK_IgmpRouter_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>

CLICK_DECLS

enum filter_mode {INCLUDE, EXCLUDE};

class Timer;
class IgmpRouter:  public Element {
private:
    struct Group {
        IPAddress mult_addr;
        Timer timer;
        filter_mode filterMode;
    };

public:
    IgmpRouter();

    ~IgmpRouter();

    const char *class_name() const { return "IgmpRouter"; }

    const char *port_count() const { return "0/0"; }

    const char *processing() const { return AGNOSTIC; }

    int configure(Vector <String> &, ErrorHandler *);

    bool acceptSource(IPAddress dest, IPAddress client, IPAddress client_mask);

    void toExclude(IPAddress src, IPAddress mult_addr);

    void toInclude(IPAddress src, IPAddress mult_addr);

    bool multicastExists(IPAddress mult_addr);

private:
    HashTable<IPAddress, Vector<IgmpRouter::Group>*> groupsMap;

};

CLICK_ENDDECLS


#endif //CLICK_IgmpRouter_HH
