#include <iostream>
//#include <cstdlib> // if use normal srand()

#include "debug.h"
#include "error.h"
#include "random.h"
#include "args.h"
#include "peer.h"
#include "peer_manager.h"
//#include "env_manager.h"


int main(int argc, const char *argv[])
{
    if(argc == 1) {
        ExitError("First argument: path of config file");
    } else if (argc > 2) {
        ExitError("Too much arguments.");
    }

    uniformrand::InitRandSeeds();  // init group of random seeds

    std::cout << "Group of Initial Rand-Seeds:\n";
    for(int i = 0; i < k_num_rseeds; i++) {
        if(i == 10) { std::cout << "\nUnused rand seeds: \n"; }
        std::cout << i << " : " << g_rand_grp[i] << "\n";
    }
    std::cout << "\n\n";


    /* read arguments */
    Args args(argv[1]);

    PeerManager pm(args);

    /* start simulating */
    pm.CreatePeers();

    ShowDbgInfo(args);

    pm.DestroyPeers();
    /* stop simulating */


    std::cout << "\n\nGroup of Final Rand-Seeds:\n";
    for(int i = 0; i < k_num_rseeds; i++) {
        if(i == 10) { std::cout << "\nUnused rand seeds: \n"; }
        std::cout << i << " : " << g_rand_grp[i] << "\n";
    }

	return 0;
}
