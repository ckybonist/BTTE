#include <iostream>
#include <ctime>

#include "args.h"
#include "debug.h"
#include "event_handler.h"
#include "utils/record.h"
//#include "peer_manager.h"
#include "env_manager.h"


using namespace btte_uniformrand;


EnvManager& EnvManager::GetInstance()
{
    static EnvManager instance;
    return instance;
}

void EnvManager::Init(const std::string filename)
{
    /* Init Simulation Args */
    g_btte_args.InitArgs(filename);
    PrintSimuArgsInfo();

    /* Rand Init */
    //const int rand_seed = 1;
    //InitRandSeeds(rand_seed);
    InitRandSeeds((unsigned) std::time(0));
    //std::cout << "Initial Random Seeds: " << init_seed << "\n";
    //PrintRandSeeds();

}

void EnvManager::Simulate(const std::string record_type)
{
    /* Create Initial Swarm*/
    PeerManager pm = PeerManager();
    pm.CreateSwarm();

    // EventHandler(peer_manager-obj, arrival-rate, departure-rate)
    EventHandler evh(&pm, 0.2, 0.5);
    evh.StartRoutine();

    WriteRecord(record_type);
    //PrintDbgInfo();
    //std::cout << "\n\nFinal Random Seeds:\n";
    //PrintRandSeedsInfo();
}
