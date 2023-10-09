#pragma once
#include <iostream>

class Log
{
public:
	static void Init();
};

#define CORE_LOG(...) std::cout << "Core: " << __VA_ARGS__ << '\n'

#define LOG(...) std::cout << "Client: " << __VA_ARGS__ << '\n'