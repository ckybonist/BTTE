#ifndef __INITENV_H__
#define __INITENV_H__

/**
 * Simulation arguments
 *
 * Piece:
 *     @piece_size: 512 (Kbytes)
 *
 * Category of peers:
 *	   @class1 : 10 (Mbps)
 *	   @class2 : 128 (Kbps)
 *	   @class3 : 56 (Kbps)
 *
 * */

const int PIECE_SIZE = 512 * 1024 * 8;

/* Download speed of each class */
const int DSPEED_C1 = 10 * 1024 * 1024;
const int DSPEED_C2 = 128 * 1024;
const int DSPEED_C3 = 56 * 1024;

/* Distributed rate(%) of each class */
const int DISTRIBUTED[3] =
{
	30,
	50,
	20
};


// Singleton
class InitEnv
{
public:
	static InitEnv& getInstance();

	//void createPeers(int*, int);
	void createPeers();

	/* Each class' transmission time */
	static const float TT[3];

private:
	InitEnv() {};
	InitEnv(InitEnv const&);
	void operator=(InitEnv const&);
};


/* Normal
class InitEnv
{
public:
	InitEnv() {};
	void createPeers(int*, int);

};
*/

#endif
