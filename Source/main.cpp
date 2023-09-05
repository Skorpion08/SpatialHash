#include "Application.h"

int main(int argc, char* argv[])
{
	// Implement Raiii in application
	Application* app = nullptr;
	try
	{
		app = new Application;
		app->Run();
		delete app;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		delete app;
		return 1;
	}
	return 0;
}