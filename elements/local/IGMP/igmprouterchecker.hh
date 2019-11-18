#ifndef CLICK_IGMPROUTERCHECKER_HH
#define CLICK_IGMPROUTERCHECKER_HH

#include <click/element.hh>

CLICK_DECLS

class IgmpRouterChecker:  public Element {

public:
    IgmpRouterChecker();

    ~IgmpRouterChecker();

    const char *class_name() const { return "IgmpRouterChecker"; }

    const char *port_count() const { return "1/1"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet *);

private:
};

CLICK_ENDDECLS


#endif //CLICK_IGMPROUTERCHECKER_HH
