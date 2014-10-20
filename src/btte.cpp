#include <iostream>

#include "debug.h"
#include "error.h"
#include "random.h"
#include "args.h"
#include "peer.h"
#include "peer_manager.h"
#include "event_handler.h"
//#include "env_manager.h"


int main(int argc, const char *argv[])
{
    /////////////////////////////
    // 1. init random seeds group
    //
    uniformrand::InitRandSeeds();

    std::cout << "Group of Initial Rand-Seeds:\n";
    for(int i = 0; i < g_kNumRSeeds; i++)
    {
        if(i == 10) { std::cout << "\nUnused rand seeds: \n"; }
        std::cout << i << " : " << g_rand_grp[i] << "\n";
    }
    std::cout << "\n\n";


    ////////////////////
    // 2. read arguments
    //
    //if(argc == 1)
    //{
    //    ExitError("First argument: path of config file");
    //}
    //else if (argc > 2)
    //{
    //    ExitError("Too much arguments.");
    //}

    //Args args(argv[1]);
    Args args("../btte.conf.dbg");


    ///////////////////////
    // 3. start simulating
    //
    PeerManager pm(&args);
    pm.CreatePeers();

    EventHandler evh(args, &pm, 0.2, 0.5);
    evh.StartRoutine();

    ShowDbgInfo(args);


    /////////////////////////////////
    // 4. check seeds was being used
    //
    std::cout << "\n\nGroup of Final Rand-Seeds:\n";
    for(int i = 0; i < g_kNumRSeeds; i++)
    {
        if(i == 10)
        {
            std::cout << "\nUnused rand seeds: \n";
        }
        std::cout << i << " : " << g_rand_grp[i] << "\n";
    }

	return 0;
}
