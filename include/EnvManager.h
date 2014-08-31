#ifndef __INITENV_H__
#define __INITENV_H__

class PeerManager; // forward declaration

// Singleton Class
// Tracker and Other Else Manager
class EnvManager
{
public:
	static EnvManager &getInstance();

	void init(const PeerManager&) const;

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
