#include <iostream>
#include <cstdlib>

#include "Random.h"
#include "EnvManager.h"

int main(int argc, const char *argv[]) {
    //uniformdist::srand((unsigned) time(0));
    uniformdist::srand(1);

    // Singleton init
	(EnvManager::getInstance()).init("../btte.conf");

    // Do something

	(EnvManager::getInstance()).destroy();

    /*
    // Normal init
	InitEnv init;
	init.createPeers(peers, N);
    */
	return 0;
}
