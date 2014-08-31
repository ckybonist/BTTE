#ifndef __INITENV_H__
#define __INITENV_H__

#include <iostream>
#include <string>

#include "PeerManager.h"

// Singleton Class
// Tracker and Other Else Manager
class EnvManager
{
public:
	static EnvManager &getInstance();

	void init(std::string, PeerManager) const;

    void destroy() const;

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

#endif
