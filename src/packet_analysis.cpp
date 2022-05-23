#include "packet_analysis.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace wotreplay;

PacketAnalysis::PacketAnalysis(const packetVector_t & packetVector){
    for (size_t i = 0; i < packetVector.size(); i++)
    {
        packet_t packet = packetVector.at(i);

        uint32_t subtype = (packet.has_property(property_t::sub_type))?packet.sub_type():0;
        packetType_t type= packetType_t(packet.type(),subtype);
        
        packetSet.insert(mapPair_t(type,packetVector_t()));
    }

    for (size_t i = 0; i < packetVector.size(); i++)
    {
        packet_t packet = packetVector.at(i);
        
        uint32_t subtype = (packet.has_property(property_t::sub_type))?packet.sub_type():0;
        packetType_t type= packetType_t(packet.type(),subtype);
        
        packetSet.find(type)->second.push_back(packet);
    }
    
}

void PacketAnalysis::printContent(){
    for (auto i = packetSet.begin(); i != packetSet.end(); i++)
    {
        double lengthSum = 0;
        for (size_t j = 0; j < i->second.size(); j++)
        {
            lengthSum += i->second.at(j).length();
        }
        double averageLength = lengthSum/i->second.size();

        std::cout<<"packet id: "<<i->first.first<<"_"<<i->first.second<<"\t";
        std::cout<<"packet counter: "<<i->second.size()<<"\t";
        std::cout<<"packet average size: "<<averageLength<<"\t";
        std::cout<<std::endl;
    }
    
}

void PacketAnalysis::rawFileOutput(){
    std::string basicPath = "/home/jze/个人项目/回放解析/wotreplay-parser-modified/packets_output/";
    for (auto i = packetSet.begin(); i != packetSet.end(); i++)
    {
        std::string fileName = "";
        std::stringstream ss;
        ss<<i->first.first<<"."<<i->first.second;
        ss>>fileName;

        std::string packetLengthDistributionFileName = basicPath + fileName +".csv";
        std::ofstream ofile;
        ofile.open(packetLengthDistributionFileName);
        ofile << "packetLength, packetDistance, packetBianary"<<std::endl;
        for (auto j = i->second.begin(); j != i->second.end(); j++)
        {
            ofile<<j->length()<<", "<<j->get_packet_distance()<<", "<<*j<<std::endl;
        }
        ofile.close();

    }
}

void PacketAnalysis::printPacketWithType(uint32_t type, uint32_t subtype){
    auto packetVector = packetSet.find(packetType_t(type, subtype))->second;
    uint counter = 0;
    for (auto i = packetVector.begin(); i != packetVector.end(); i++, counter++)
    {
        std::cout<<"packet "<<type<<"."<<subtype<<" #"<<counter<<std::endl;
        i->packetContentOutput(std::cout)<<std::endl;
    }
    
}

void PacketAnalysis::packetType10ValueOutput(const packetVector_t & packetVector, std::ostream & os){
    os<<"time,playerId,field_1,x,y,z,field_2,field_3,field_4,field_5,field_6,field_7"<<std::endl;
    
    for (auto i = packetVector.begin(); i != packetVector.end(); i++)
    {
        if (i->type() != 10) continue;

        uint16_t offsetList[] = {16, 20, 24, 28, 32, 36, 40, 44, 48, 52};

        os<<i->clock()<<',';
        os<<i->player_id()<<',';
        os<<get_field<uint32_t>(i->data.begin(), i->data.end(), 16)<<','; // field 1

        os<<get_field<float>(i->data.begin(), i->data.end(), 20)<<','; // x
        os<<get_field<float>(i->data.begin(), i->data.end(), 24)<<','; // y
        os<<get_field<float>(i->data.begin(), i->data.end(), 28)<<','; // z

        os<<get_field<uint32_t>(i->data.begin(), i->data.end(), 32)<<','; // field 2
        os<<get_field<uint32_t>(i->data.begin(), i->data.end(), 36)<<','; // field 3
        os<<get_field<uint32_t>(i->data.begin(), i->data.end(), 40)<<','; // field 4

        
        os<<get_field<float>(i->data.begin(), i->data.end(), 44)<<','; // field 5
        os<<get_field<float>(i->data.begin(), i->data.end(), 48)<<','; // field 6
        os<<get_field<float>(i->data.begin(), i->data.end(), 52); // field 7

        os<<std::endl;
    }
}
