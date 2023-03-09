#include <iostream>

#include <SDL.h>

#include "headers/App.hpp"

App::App(unsigned int screenW, unsigned int screenH)
	: m_pBoard(nullptr)
	, m_pRenderer(nullptr)
	, m_pWindow(nullptr)
{
	m_screenW = screenW;
	m_screenH = screenH;
}

void App::NewGame()
{
	delete m_pBoard;
	m_pBoard = nullptr;
	m_pBoard = new Board(750);
}

bool App::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
	{
		std::cout << "SDL FAILED TO INITIALIZE: " << SDL_GetError() << std::endl;
		return false;
	}

	m_pWindow = SDL_CreateWindow(
		"TetrisAI",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_screenW,
		m_screenH,
		0
	);

	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_SOFTWARE);

	NewGame();
	return true;
}

void App::Draw()
{
	static unsigned int squareSize = 50;

	static int boardW = m_pBoard->WIDTH * squareSize;
	static int boardH = m_pBoard->HEIGHT * squareSize;

	static int boardOffsetX = (m_screenW - boardW) / 2;
	static int boardOffsetY = (m_screenH - boardH) / 2;

	// offsets are so the blocks don't overlap with the border
	SDL_Rect boardOutline =
	{
		boardOffsetX-1,
		boardOffsetY-1,
		boardW+2,
		boardH+2
	};

	SDL_SetRenderDrawColor(m_pRenderer, 18, 18, 18, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(m_pRenderer);

	SDL_SetRenderDrawColor(m_pRenderer, 93, 93, 93, 255);
	SDL_RenderDrawRect(m_pRenderer, &boardOutline);

	for (int y = 0; y < m_pBoard->HEIGHT; y++)
	{
		int absy = y * squareSize + boardOffsetY;

		for (int x = 0; x < m_pBoard->WIDTH; x++)
		{
			int absx = x * squareSize + boardOffsetX;

			int sq = m_pBoard->GetSquare(x, y);
			if (sq == 0) continue;

			SDL_Rect sqRect =
			{
				absx,
				absy,
				squareSize,
				squareSize
			};

			SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, 255); // TODO : different colors based on piece
			SDL_RenderFillRect(m_pRenderer, &sqRect);
		}
	}

	SDL_RenderPresent(m_pRenderer);
}

void App::Run()
{
	bool end = false;
	while (!end)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				end = true;
			}
		}

		Uint32 currentTimeMs = SDL_GetTicks();
		m_pBoard->Update((int)currentTimeMs);

		Draw();
	}

}

void App::ShutDown()
{
	delete m_pBoard;
	m_pBoard = nullptr;

	SDL_DestroyWindow(m_pWindow);

	SDL_Quit();
}