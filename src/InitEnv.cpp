#include <iostream>

#include <ctime>

#include "Dice.h"

#include "Error.h"

#include "ConfigFile.h"

#include "InitEnv.h"


/*
 * Check amount of each class' peers
 * is reach the target of distributed-rate
 *
 * args:
 * @amount : current amount of n-class' peers
 * @volume : total amount of peers
 *
 * */
static bool isEnough(int p_no, int amount, int volume)
{
	int limit = (float)DISTRIBUTED[p_no] / (float)100 * volume;
	return (amount == limit) ? (true) : (false);
}


/* -------------------------------- */
/* Class definition */
InitEnv& InitEnv::getInstance()
{
	static InitEnv instance;
	return instance;
}

/* Initialize static member of class
 *
 *
 * Note : TT -> Transmission Time
 *
 * */
const float InitEnv::TT[3] =
{
	(float)PIECE_SIZE / (float)DSPEED_C1,
	(float)PIECE_SIZE / (float)DSPEED_C2,
	(float)PIECE_SIZE / (float)DSPEED_C3,
};

/*
 * Create peers for class#1 ~ class#3
 *
 * Args:
 *     @peers : array of peers
 *     @num_peer : total size of peers
 *
 * */
//void InitEnv::createPeers(int *peers, int volume) {
void InitEnv::createPeers()
{
    ConfigFile cfg("../config.txt");  // read config in root of project
    int num_peer = cfg.getValueOfKey<int>("NUM_PEER", 5000);  // if num_peer is not found in config file, then set default num_peer to 5000

    int *peers = new int[num_peer];
    if(!peers)
        exitWithError("Allocate memory of peers is fault!\n");

	int exSet[3] = {0};
	int counts[3] = {0};
	int size = sizeof(exSet) / sizeof(*exSet);  // exclusive set size

    srand(time(0));
	Dice dice;

	for(int i = 0; i < num_peer; i++)
    {

		int cls = dice.excludeSet(exSet, size, 1, 3);  // create class for each peers

		peers[i] = cls;

		++counts[cls-1];  // count the amount of class, this must place before the isEnough() check

		/*
		 * Check the amount of peers' class is
		 * fullfill our requirement
		 *
		 * */
		if(isEnough(cls-1, counts[cls-1], num_peer))
		{
			if(exSet[cls-1] == 0)
				exSet[cls-1] = cls;
		}
	}

    // Print to Test
    using std::cout;
    using std::endl;

    cout << "Total Number of Peers: " << num_peer << endl;
	for(int i = 0; i < 3; i++) {
		cout << "class#" << i << " has "<<counts[i]
		     << " peers" << endl;
    }
    cout << endl;

	cout << "亂數節點 (class#1 ~ class#3):"
	     << endl;

	for(int i = 0; i < num_peer; i++) {
		if (i % 5 == 0) {
			cout << endl;
		}
		cout << peers[i] << " ";
	}

	cout << "\n\n\n";
	cout << "各節點類別的傳輸速度:" << endl << endl;

	for(int i=0; i<3; i++) {
		cout << "C" << (i+1) << " : "
		     << InitEnv::TT[i] << "(s)"
			 << endl;
	}
}

