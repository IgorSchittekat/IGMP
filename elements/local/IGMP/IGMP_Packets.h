#pragma pack(push, 1)
#ifndef CLICK_IGMP_PACKETS_H
#define CLICK_IGMP_PACKETS_H

struct MembershipQuery {
    uint8_t Type;
    uint8_t Max_respond_code;
    uint16_t Checksum;
    uint32_t group_addr;

#if CLICK_BYTE_ORDER == CLICK_BIG_ENDIAN
    unsigned Resv:4;
    unsigned S:1;
    unsigned QRV:3;
#elif CLICK_BYTE_ORDER == CLICK_LITTLE_ENDIAN
    unsigned QRV:3;
    unsigned S:1;
    unsigned Resv:4;
#else
#   error "unknown byte order"
#endif

    uint8_t QQIC;
    uint16_t NumberOfSources;

    //in_addr source_addres[];

};

struct  MembershipReport {
    uint8_t Type;
    uint8_t Reserved1;
    uint16_t Checksum;
    uint16_t Reserved2;
    uint16_t M;

    //GroupRecord records[];
};

#define IGMP_MODE_IS_INCLUDE            1
#define IGMP_MODE_IS_EXCLUDE            2
#define IGMP_CHANGE_TO_INCLUDE_MODE     3
#define IGMP_CHANGE_TO_EXCLUDE_MODE     4

#define IGMP_REPORT_TYPE                0x22
#define IGMP_QUERY_TYPE                 0x11


struct GroupRecord {
    uint8_t Record_Type;
    uint8_t auxDataLen;
    uint16_t N;
    in_addr MulticastAddress;

    //in_addr source_addres[];
};

struct RouterAlert {
    uint8_t Type;
    uint8_t Length;
    uint16_t Value;
};




#endif //CLICK_IGMP_PACKETS_H
