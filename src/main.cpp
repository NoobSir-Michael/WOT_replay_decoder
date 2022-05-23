#include "logger.h"
#include "parser.h"
#include "regex.h"
#include "tank.h"
#include "version.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include <fstream>
#include <float.h>

#define EX_OK 0
#define EX_USAGE 64
#define EX_SOFTWARE 70
#include <unistd.h>
#include <sysexits.h>

#include "packet_analysis.h"

using namespace wotreplay;
using namespace boost::filesystem;
namespace po = boost::program_options;

void show_help(int argc, const char *argv [], po::options_description &desc) {
    std::stringstream help_message;
    help_message << desc << "\n";
    logger.write(help_message.str());
}

int process_replay_file(const std::string input) {

    // 如果没有输入文件地址, 报错
    if (!(input.size() > 0)) {
        logger.write(wotreplay::log_level_t::error, "没有输入文件\n");
        return EX_USAGE;
    }
    // 读取文件
    std::ifstream in(input, std::ios::binary);
    if (!in) {
        logger.writef(log_level_t::error, "Failed to open file: %1%\n", input);
        return EX_SOFTWARE;
    }

    std::string outputBasicPath = input;
    std::replace(outputBasicPath.begin(),outputBasicPath.end(),'/','_');
    outputBasicPath = "output/"+outputBasicPath+"/";
    if (! boost::filesystem::exists(outputBasicPath))
    {
        boost::filesystem::create_directories(outputBasicPath);
    }

    parser_t parser(load_data_mode_t::on_demand); // 实例化一个翻译对象
    game_t game; // 实例化用于承接翻译对象的实例
    
    parser.parse(in, game);

    auto packetVector = game.get_packets();

    std::ofstream of;
    of.open(outputBasicPath+"replay content.csv");
    of<<"time,player,playerName,playerTank,teamId,x,y,z"<<std::endl;
    for (auto i = packetVector.begin(); i != packetVector.end(); i++)
    {
        if (i->type()!=10)continue;
        
        float time = i->clock();
        if (time<=0)continue;

        try{
            uint32_t player=i->player_id();
            int teamId = game.get_team_id(player);
            player_t playerInfo = game.get_player(player);
            std::string playerName = playerInfo.name;
            std::string playerTank = playerInfo.tank;
            float x=std::get<0>(i->position()),y=std::get<1>(i->position()),z=std::get<2>(i->position());
            of<<time<<","<<player<<","<<playerName<<","<<playerTank<<","<<teamId<<","<<x<<","<<y<<","<<z<<std::endl;
        }catch(std::out_of_range variable){
            continue;
        }
    }
    of.close();

    of.open(outputBasicPath+"message.csv");
    of<<"time,message"<<std::endl;
    for (auto i = packetVector.begin(); i != packetVector.end(); i++)
    {
        if (i->type()==35)
        {
            of<<i->clock()<<","<<i->message()<<std::endl;
        }
        
    }
    of.close();
    
    in.close();
    return EX_OK;
}

int main(int argc, const char * argv []) {
    po::options_description desc("Allowed options");

    std::string input;

    desc.add_options()
        ("input", po::value(&input), "input file");
    
    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (...) {
        show_help(argc, argv, desc);
        std::exit(-1);
    }

    int exit_code = EX_USAGE;
    if (! is_directory(input))
    {
        exit_code = process_replay_file(input);
    }
    

    

    if (exit_code == EX_USAGE) {
        show_help(argc, argv, desc);
    }

    return exit_code;
}
