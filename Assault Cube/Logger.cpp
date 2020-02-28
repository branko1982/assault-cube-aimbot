#include "Logger.hpp"
#include <time.h>


void Logger::log(std::string toLog)
{
#ifdef _DEBUG_CHEAT
	std::stringstream ss;
	ss << currentDateTime() << toLog;

	std::cout << ss.str() << std::endl;
#endif
	return;
}

std::string Logger::currentDateTime() {
	std::stringstream ss;
	time_t actualTime;
	tm* p_localTime;
	tm localTime;
	actualTime = time(0);
	p_localTime = localtime(&actualTime);
	ss << "[";
	if (p_localTime != nullptr) {
		localTime = *p_localTime;
		ss << localTime.tm_mday << "." << (localTime.tm_mon + 1) << "." << (localTime.tm_year + 1900) << " " << (localTime.tm_hour + 1) << ":" << localTime.tm_min << ":" << localTime.tm_sec;
	}
	ss << "]";

	return ss.str();
}