#include <iostream>

#include "gfx/App.h"

int main(int argc, char* argv[])
{
	srand(time(0)); // Set the seed for random numbers

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
