#ifndef CLICK_NULLPULL_HH
#define CLICK_NULLPULL_HH

#include <click/element.hh>

CLICK_DECLS

class NullPull : public Element {
public:
    NullPull();

    ~NullPull();

    const char *class_name() const { return "NullPull"; }

    const char *port_count() const { return "1/1"; }

    const char *processing() const { return PULL; }

    int configure(Vector <String> &, ErrorHandler *);

    Packet *pull(int);
};

CLICK_ENDDECLS


#endif //CLICK_NULLPULL_HH
