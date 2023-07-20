#include "Application.h"

int main(int argc, char* argv[])
{
	// Implement Raiii in application
	Application* app = new Application;
	app->Run();
	delete app;
	return 0;
}