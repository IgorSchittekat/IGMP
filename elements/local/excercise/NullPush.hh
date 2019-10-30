#ifndef CLICK_NULLPUSH_H
#define CLICK_NULLPUSH_H


#include <click/element.hh>

CLICK_DECLS

class NullPush : public Element {
public:
    NullPush();

    ~NullPush();

    const char *class_name() const { return "NullPush"; }

    const char *port_count() const { return "1/1"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet*);
};

CLICK_ENDDECLS

#endif //CLICK_NULLPUSH_H
