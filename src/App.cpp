#include <iostream>

#include <SDL.h>

#if defined(_WIN32) || defined(WIN32)

#include <Windows.h>
#define OS_WINDOWS

#endif

#include "headers/App.hpp"
#include "headers/Board.hpp"
#include "headers/Tetrominoes.hpp"


struct InputDelays {
	int moveLeft;
	int moveRight;
	int rotClockwise;
	int rotCountClockwise;
	int softDrop;
};

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

static SDL_Color ConvertHex (int hex)
{
	SDL_Color color;
	// Bitwise right (>>) cuts off last x number of bits
	// Move enough bits so individual value is rightmost 2 hex digits
	// After, perform an & operation with 255 to chop off the rest
	// e.g. 1111 & 0011 == 0011
	color.r = ((hex >> 24) & 0xFF);
	color.g = ((hex >> 16) & 0xFF);
	color.b = ((hex >> 8) & 0xFF);
	color.a = (hex & 0xFF);
	return color;
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

			int hex = TetrominoData::hexCodes[abs(sq) - 1];
			SDL_Color color = ConvertHex(hex);
			SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a); // TODO : different colors based on piece
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

static void PersistantKey(const Uint8* keystate, int scancode, int delay, int& currentDelay, bool& input)
{
	if (keystate[scancode])
	{
		if (currentDelay == 0)
			input = true;
		currentDelay++;
		if (currentDelay >= delay)
			currentDelay = 0;
	}
	else currentDelay = 0;
}

void App::Run()
{
	InputDelays delays = {};

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
				if (event.key.keysym.sym == SDLK_c)
					input.holdPiece = true;
				if (event.key.keysym.sym == SDLK_SPACE)
					input.hardDrop = true;
			}
		}

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		PersistantKey(keystate, SDL_SCANCODE_DOWN, 120, delays.softDrop, input.softDrop);
		PersistantKey(keystate, SDL_SCANCODE_LEFT, 120, delays.moveLeft, input.moveLeft);
		PersistantKey(keystate, SDL_SCANCODE_RIGHT, 120, delays.moveRight, input.moveRight);
		PersistantKey(keystate, SDL_SCANCODE_UP, 360, delays.rotClockwise, input.rotClockwise);
		PersistantKey(keystate, SDL_SCANCODE_Z, 360, delays.rotCountClockwise, input.rotCountClockwise);

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