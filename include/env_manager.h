#ifndef __INITENV_H__
#define __INITENV_H__

#include <iostream>
#include <string>

#include "PeerManager.h"

// Singleton Class
// Tracker and Other Else Manager
class EnvManager {
public:
	static EnvManager &GetInstance();

	void Init(const std::string);

    void Destroy();

private:
    PeerManager pm;
	EnvManager() {};
	EnvManager(const EnvManager&);
	void operator=(const EnvManager&);
    ~EnvManager() {};
};

/* Normal
class EnvManager
{
public:
	EnvManager() {};
	void createPeers(int*, int);

};
*/

#endif
