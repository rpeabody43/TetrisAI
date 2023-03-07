#include <SDL.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0) 
		cout << "SDL FAILED TO INITIALIZE: " << SDL_GetError() << endl;

	cout << "test" << endl;
	

	SDL_Quit();
	return 0;
}
