#pragma once
#include <iostream>

class Log
{
public:
	static void Init();
};

#ifdef NDEBUG

#define CORE_LOG(...)

#define LOG(...)

#else

#define CORE_LOG(...) std::cout << "Core: " << __VA_ARGS__ << '\n';

#define LOG(...) std::cout << "Client: " << __VA_ARGS__ << '\n';

#endif