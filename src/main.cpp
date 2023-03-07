#include <SDL.h>
#include <iostream>
#include <string>

#include "headers/Board.hpp";

using namespace std;

int main(int argc, char* argv[])
{
	Board b (45);

	if (SDL_Init(SDL_INIT_VIDEO) > 0) 
		cout << "SDL FAILED TO INITIALIZE: " << SDL_GetError() << endl;

	int stepRate = b.ticks_per_step();
	for (int i = 0; i < 1000; i++)
	{
		b.tick();
		if (b.total_ticks() % stepRate != 0)
			continue;

		string output = "";
		for (int y = 0; y < 18; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				string square = (b.get_square(x, y) > 0) ? "[O]" : "[ ]";
				output.append(square);
			}
			output.append("\n");
		}
		cout << output << endl;
	}

	

	SDL_Quit();
	return 0;
}
