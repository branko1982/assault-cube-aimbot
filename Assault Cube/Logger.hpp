#include <iostream>
#include <sstream>
class Logger
{
public:
	static void log(std::string toLog);
	static std::string currentDateTime();
};