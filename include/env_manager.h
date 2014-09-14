#ifndef _ENV_MANAGER_H
#define _ENV_MANAGER_H

#include <iostream>
#include <string>

#include "args.h"
#include "peer_manager.h"

// Singleton
class EnvManager {
public:
	static EnvManager &GetInstance();

	void Init(const std::string);

     //void RunEvents();

    void Destroy();

private:
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

#endif // for #ifndef _ENV_MANAGER_H
