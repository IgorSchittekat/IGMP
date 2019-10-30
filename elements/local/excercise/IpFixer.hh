#ifndef CLICK_IPFIXER_HH
#define CLICK_IPFIXER_HH

#include <click/element.hh>

CLICK_DECLS

class IpFixer : public Element {
public:
    IpFixer();

    ~IpFixer();

    const char *class_name() const { return "IpFixer"; }

    const char *port_count() const { return "1/1"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet*);
private:
    String toFix = "Dst";
    in_addr ip;
};

CLICK_ENDDECLS


#endif //CLICK_IPFIXER_HH
