#include <iostream>

#include "InitEnv.h"

using namespace std;


int main(int argc, const char *argv[])
{
    // Singleton init
	(InitEnv::getInstance()).createPeers();

    // Normal init
	//InitEnv init;
	//init.createPeers(peers, N);

	return 0;
}
