#include <iostream>

#include "Random.h"
#include "EnvManager.h"

using namespace std;


int main(int argc, const char *argv[])
{
    uniformdist::srand((unsigned)time(0));

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
