#pragma once

#include "Board.hpp"

struct SDL_Renderer;
struct SDL_Window;

class App
{
public:
	App(unsigned int screenW, unsigned int screenH);

	bool Init();
	void Run();
	void ShutDown();
	void NewGame();

private:
	void Draw();

	unsigned int m_screenW;
	unsigned int m_screenH;

	Board* m_pBoard;
	SDL_Renderer* m_pRenderer;
	SDL_Window* m_pWindow;
};