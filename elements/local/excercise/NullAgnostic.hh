#ifndef CLICK_NULLAGNOSTIC_HH
#define CLICK_NULLAGNOSTIC_HH
#include <click/element.hh>

CLICK_DECLS

class NullAgnostic : public Element {
public:
    NullAgnostic();

    ~NullAgnostic();

    const char *class_name() const { return "NullAgnostic"; }

    const char *port_count() const { return "1/1"; }

    const char *processing() const { return AGNOSTIC; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet*);

    Packet * pull(int);
};

CLICK_ENDDECLS




#endif //CLICK_NULLAGNOSTIC_HH
