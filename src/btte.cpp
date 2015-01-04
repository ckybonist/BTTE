#include <iostream>
#include <cstring>
#include <iomanip>
#include <time.h>

#include "error.h"
#include "env_manager.h"

namespace
{
    void TimeCompute(double time_taken)
    {
    	int t_min = time_taken / 60;
	int t_sec = (int)time_taken % 60;
	int t_hour = t_min / 60;
	t_min %= 60;

	std::cout << "Total time: " << t_hour << " h " << t_min << " m " << t_sec << " s \n";
    }
}

int main(int argc, const char* argv[])
{
    clock_t start_time = clock();

    if (argc != 3 ||
        std::strcmp(argv[2], "peer_sel") != 0 &&
        std::strcmp(argv[2], "piece_sel") != 0)
    {
        ExitError("\n\tUsage: ./BTTE [config] [peer_sel/piece_sel]");
    }

    /* -------------- Start Simulation -------------------- */
    EnvManager& env = EnvManager::GetInstance();
    // argv[1]: config file
    // argv[2]: record which algorithm
    env.Init(argv[1]);
    env.Simulate(argv[2]);
    /* ----------------- End Simulation -------------------- */

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    TimeCompute(time_taken);

    return 0;
}
