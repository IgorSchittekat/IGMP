#ifndef CLICKNET_IGMP_H
#define CLICKNET_IGMP_H
#include <clicknet/ip.h>
#include <click/vector.hh>

struct menbershipQuery {
    uint8_t igmp_type;          // 0        IGMP type
    uint8_t max_resp_code;      // 1        Max time allowed before sending resp
    uint16_t igmp_cksum;        // 2-3      checksum
    uint32_t group_address;     // 4-7      IP Multicast address, or 0
    uint8_t resv_s_qrv;         // 8        first 4 bits are reserved, next bit is S-flag, last 3 bits are QRV
    uint8_t qqic;               // 9        QQIC
    uint16_t source_num;        // 10-11    number of sources
    Vector<uint32_t> sources;   // 12-...   sources
};

struct groupRecord {
    uint8_t record_type;        // 0        Group Record type
    uint8_t aux_data_len;       // 1        should be zero for IGMPv3
    uint16_t source_num;        // 2-3      number of sources
    uint32_t multicast_address;     // 4-7      IP multicast address
    uint32_t sources[0];   // 8-...    sources
    uint32_t aux_data[0];  // ...      should be empty
};

struct membershipReport {
    uint8_t igmp_type;          // 0        IGMP type
    uint8_t resv;               // 1        Reserved, set to zero on transmit
    uint16_t igmp_cksum;        // 2-3      checksum
    uint16_t resv2;             // 4-5      Reserved, set to zero on transmit
    uint16_t record_num;        // 6-7      Number of Group Records
    groupRecord records[1];   // 8-...    Group Records
};




#define IGMP_QUERY_TYPE     0x11        // Membership Query
#define IGMP_REPORT_TYPE    0x22        // v3 Membership Report

#define IGMP_MODE_IS_INCLUDE        1   // filter mode is INCLUDE
#define IGMP_MODE_IS_EXCLUDE        2   // filter mode is EXCLUDE
#define IGMP_CHANGE_TO_INCLUDE_MODE 3   // set filter mode to INCLUDE
#define IGMP_CHANGE_TO_EXCLUDE_MODE 4   // set filter mode to EXCLUDE

#endif