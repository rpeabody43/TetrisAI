#pragma once

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Board.hpp"


class App
{
public:
	App(unsigned int screenW, unsigned int screenH);

	bool Init();
	void Run();
	void ShutDown();
	void NewGame();

private:
	bool UnixScaling();
	void Draw();
	void DrawPiece(int x, int y, int piece, int sqSize);
	void DrawTxt(int x, int y, const char* txt, SDL_Color color);

	unsigned int m_screenW;
	unsigned int m_screenH;

	Board* m_pBoard;
	SDL_Renderer* m_pRenderer;
	SDL_Window* m_pWindow;
	TTF_Font* m_pFont;
};