#include <celero/Celero.h>
#include <chrono>
#include <thread>

BASELINE(DemoSimple, Baseline, 0, 0)
{
    std::this_thread::sleep_for(std::chrono::nanoseconds{1});
}
