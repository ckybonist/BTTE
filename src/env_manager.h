#ifndef _ENV_MANAGER_H
#define _ENV_MANAGER_H

#include <iostream>


// Singleton
class EnvManager
{
  public:
    static EnvManager& GetInstance();

    void Init(const std::string filename);

    void Simulate();

  private:
    EnvManager() {};
    EnvManager(const EnvManager&);
    void operator=(const EnvManager&);
    ~EnvManager() {};
};

#endif // for #ifndef _ENV_MANAGER_H
