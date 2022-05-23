#ifndef PACKET_ANALYSIS_H
#define PACKET_ANALYSIS_H

#include"packet.h"

#include<vector>
#include<map>

namespace wotreplay{
    typedef std::pair<uint32_t, uint32_t> packetType_t;
    typedef std::vector<wotreplay::packet_t> packetVector_t;
    typedef std::pair<packetType_t , packetVector_t > mapPair_t;

    class PacketAnalysis{
    private:
        std::map<packetType_t , packetVector_t > packetSet;
    public:
        PacketAnalysis(const packetVector_t & packetVector);
        void printContent();
        void rawFileOutput();
        void printPacketWithType(uint32_t type, uint32_t subtype);
        void packetType10ValueOutput(const packetVector_t & packetVector, std::ostream & ostream);

    };

}

#endif