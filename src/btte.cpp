#include <iostream>
#include <iomanip>
#include <time.h>

#include "error.h"
#include "env_manager.h"


int main(int argc, const char* argv[])
{
    clock_t start_time = clock();

    if (argc == 1)
    {
        ExitError("First argument: path of config file");
    }
    else if (argc > 2)
    {
        ExitError("Too much arguments.");
    }


    /* -------------- Start Simulation -------------------- */
    EnvManager& env = EnvManager::GetInstance();
    env.Init(argv[1]);
    env.Simulate();
    /* ----------------- End Simulation -------------------- */


    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << "Time taken: " << time_taken << " s\n";

    return 0;
}
