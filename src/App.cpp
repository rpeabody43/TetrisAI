#include <iostream>

#include <SDL.h>

#if defined(_WIN32) || defined(WIN32)

#include <Windows.h>
#define OS_WINDOWS

#endif

#include "headers/App.hpp"
#include "headers/Board.hpp"

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
	m_pBoard = new Board(250);
}

bool App::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
	{
		std::cout << "SDL FAILED TO INITIALIZE: " << SDL_GetError() << std::endl;
		return false;
	}

	// Fix for non-native scaling/DPI (150% in Windows, etc.)
#ifdef OS_WINDOWS
	SetProcessDPIAware();
#endif

	m_pWindow = SDL_CreateWindow(
		"TetrisAI",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_screenW,
		m_screenH,
		SDL_WINDOW_ALLOW_HIGHDPI
	);

	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_SOFTWARE);
	
#ifdef __unix__
	if (!UnixScaling())
		return false;
#endif

	NewGame();
	return true;
}

bool App::UnixScaling()
{
	int rw = 0, rh = 0;
	SDL_GetRendererOutputSize(m_pRenderer, &rw, &rh);

	std::cout << "rw: " << rw << ", rh: " << rh << std::endl;

	if (rw != m_screenW)
	{
		float wScale = (float)rw / (float)m_screenW;
		float hScale = (float)rh / (float)m_screenH;

		if (wScale != hScale)
		{
			std::cout << "SCALING ERROR: Width scale != Height scale" << std::endl;
			return false;
		}

		SDL_RenderSetScale(m_pRenderer, wScale, hScale);
	}
	return true;
}

void App::Draw()
{
	static int squareSize = 50;

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

	/*int anchor = m_pBoard->FallingPieceAnchor();
	int x = (anchor % 10)*squareSize + boardOffsetX;
	int y = (anchor / 10)*squareSize + boardOffsetY;

	SDL_Rect anchorSq =
	{
		x,
		y,
		squareSize,
		squareSize
	};

	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 255);
	SDL_RenderFillRect(m_pRenderer, &anchorSq);*/

	SDL_RenderPresent(m_pRenderer);
}

void App::Run()
{
	bool end = false;
	while (!end)
	{
		Input input = {};

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				end = true;
			}

			if (event.type == SDL_KEYDOWN)
			{
				// Copying Tetris.com controls
				if (event.key.keysym.sym == SDLK_DOWN)
					input.softDrop = true;
				if (event.key.keysym.sym == SDLK_LEFT)
					input.moveLeft = true;
				if (event.key.keysym.sym == SDLK_RIGHT)
					input.moveRight = true;
				if (event.key.keysym.sym == SDLK_UP)
					input.rotClockwise = true;
				if (event.key.keysym.sym == SDLK_z)
					input.rotCountClockwise = true;
				if (event.key.keysym.sym == SDLK_c)
					input.holdPiece = true;
				if (event.key.keysym.sym == SDLK_SPACE)
					input.hardDrop = true;
			}
		}

		Uint32 currentTimeMs = SDL_GetTicks();
		m_pBoard->Update(input, (int)currentTimeMs);

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