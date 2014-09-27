#ifndef _ENV_MANAGER_H
#define _ENV_MANAGER_H

#include <iostream>
#include <string>

#include "args.h"
#include "peer_manager.h"

// Singleton
class EnvManager {
public:
	static EnvManager& GetInstance();

	void Init(const std::string filename);

     //void RunEvents();

    void Destroy();

private:
	EnvManager() {};
	EnvManager(const EnvManager&);
	void operator=(const EnvManager&);
    ~EnvManager() {};
};

#endif // for #ifndef _ENV_MANAGER_H
