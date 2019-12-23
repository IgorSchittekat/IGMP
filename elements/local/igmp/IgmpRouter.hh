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
        Group(IPAddress a, Timer* t, filter_mode m) {
            mult_addr = a;
            timer = t;
            filterMode = m;
        }
        
        IPAddress mult_addr;
        Timer* timer;
        filter_mode filterMode;
    };

    struct TimerData {
        TimerData(IPAddress src, IPAddress dest, IgmpRouter* router, int count, int sFlag) {
            this->src = src;
            this->dest = dest;
            this->router = router;
            this->count = count;
            this->sFlag = sFlag;
        }

        IPAddress src;
        IPAddress dest;
        IgmpRouter* router;
        int count;
        int sFlag;
    };

public:

    IgmpRouter();

    ~IgmpRouter();

    const char *class_name() const { return "IgmpRouter"; }

    const char *port_count() const { return "0/0-1"; }

    const char *processing() const { return AGNOSTIC; }

    int configure(Vector <String> &, ErrorHandler *);

    static void timerCallback(Timer* t, void* v);

    bool acceptSource(IPAddress dest, IPAddress client, IPAddress client_mask);

    void isExclude(IPAddress src, IPAddress mult_addr);

    void toExclude(IPAddress src, IPAddress mult_addr);

    void toInclude(IPAddress src, IPAddress mult_addr);

    bool multicastExists(IPAddress mult_addr);

    static void scheduleGroupSpecificQuery(Timer* t, void* v);

    void sendGroupSpecificQuery(IPAddress client, IPAddress dest, int s);

    int getRobustnessVariable() const { return robust_var; }

    int getQueryInterva() const { return query_int; }

    int getQueryResponceInterval() const { return query_resp_int; }

    int getGroupMembershipInterval() const { return robust_var * query_int + query_resp_int; }

    int getStartupQueryInterval() const { return query_int / 4; }

    int getStartupQueryCount() const { return robust_var; }

    int getLMQI() const { return LMQI; }

    int getLMQC() const { return robust_var; }

    int getLMQT() const { return getLMQI() * getLMQC(); }

private:
    HashTable<IPAddress, Vector<IgmpRouter::Group*>*> groupsMap;
    HashTable<IPAddress, int> s_map;

    // Default values
    int robust_var = 2;
    int query_int = 125;
    int query_resp_int = 10;
    int LMQI = 1;
    

};

CLICK_ENDDECLS


#endif //CLICK_IgmpRouter_HH
