#include <chrono>
#include "Core/Time.h"

auto __TimeCPPStartTime = std::chrono::high_resolution_clock::now();

double GetCurrentTimeSeconds()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double, std::chrono::seconds::period>( currentTime - __TimeCPPStartTime ).count();
}
