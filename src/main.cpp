#include <iostream>

#include "headers/App.hpp"

int main(int argc, char* argv[])
{
	unsigned int windowW = 1280;
	unsigned int windowH = 960;

	App app (windowW, windowH);

	if (!app.Init())
	{
		std::cout << "ERR: Could not initialize" << std::endl;
		app.ShutDown();
		return 1;
	}

	app.Run();

	app.ShutDown();

	return 0;
}
