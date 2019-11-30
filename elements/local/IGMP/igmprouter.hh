#ifndef CLICK_IgmpRouter_HH
#define CLICK_IgmpRouter_HH

#include <click/element.hh>

CLICK_DECLS


enum filter_mode {INCLUDE, EXCLUDE};

class IgmpRouter:  public Element {
    public:
    struct SourceRecord {
        IPAddress src_addr;
        Timer src_timer;
    };
    struct State {
        IPAddress mult_addr;
        Timer timer;
        filter_mode filterMode;
        Vector<SourceRecord> records;
    };

public:
    IgmpRouter();

    ~IgmpRouter();

    const char *class_name() const { return "IgmpRouter"; }

    const char *port_count() const { return "0/0"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet *);


    Vector<State> states;

private:

};

CLICK_ENDDECLS


#endif //CLICK_IgmpRouter_HH
