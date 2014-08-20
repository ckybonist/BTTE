#include <iostream>

#include "InitEnv.h"

using namespace std;


int main(int argc, const char *argv[])
{
	int *peers;

	//sleep(10);  // for testing

	(InitEnv::getInstance()).createPeers();

	//InitEnv init;
	//init.createPeers(peers, N);

	return 0;
}
