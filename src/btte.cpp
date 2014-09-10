#include <iostream>
#include <cstdlib>

#include "random.h"
#include "env_manager.h"

int main(int argc, const char *argv[]) {
    //uniformdist::srand((unsigned) time(0));  // slow speed
    uniformdist::srand(1);  // for testing

    // Singleton init
	(EnvManager::GetInstance()).Init("../btte.conf");

    // Do something

	(EnvManager::GetInstance()).Destroy();

    /*
    // Normal init
	EnvManager em;
	em.createPeers();
    */
	return 0;
}
