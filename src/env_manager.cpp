#include <iostream>

#include "debug.h"
#include "args.h"
#include "event_handler.h"
//#include "peer_manager.h"
#include "env_manager.h"


EnvManager& EnvManager::GetInstance()
{
    static EnvManager instance;
    return instance;
}

void EnvManager::Init(const std::string filename)
{
    /* Rand Init */
    const int init_seed = 1;
    uniformrand::InitRandSeeds(init_seed);

    std::cout << "Initial Random Seeds: " << init_seed << "\n";
    PrintRandSeeds();

    /* Simulation Args Init */
    g_btte_args.InitArgs(filename);
}

void EnvManager::Simulate()
{
    /* Create Initial Swarm*/
    PeerManager pm = PeerManager();
    pm.CreateSwarm();

    EventHandler evh(&pm, 0.2, 0.5);
    evh.StartRoutine();

    ShowDbgInfo();
    //std::cout << "\n\nFinal Random Seeds:\n";
    //PrintRandSeeds();
}
