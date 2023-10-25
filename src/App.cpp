#include <iostream>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>


#if defined(_WIN32) || defined(WIN32)

#include <Windows.h>
#define OS_WINDOWS

#endif

#include "headers/App.hpp"
#include "headers/Board.hpp"
#include "headers/tetrominoes.hpp"
#include "headers/ai.hpp"


struct KeyHandler {
	int scancode;
	int delay;
	int firstDelay; // Add more input buffer between the first and second input
	int currentDelay;
	int inputCount;
};

App::App(unsigned int screenW, unsigned int screenH)
	: m_pBoard(nullptr)
	, m_pRenderer(nullptr)
	, m_pWindow(nullptr)
	, m_pFont28(nullptr)
	, m_pFont40(nullptr)
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

	TTF_Init();

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
	
	m_pFont28 = TTF_OpenFont("Retro Gaming.ttf", 28);
	m_pFont40 = TTF_OpenFont("Retro Gaming.ttf", 40);

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
	//int startTime = SDL_GetTicks();
	static int squareSize = 45;

	static int boardScreenW = m_pBoard->WIDTH * squareSize;
	static int boardScreenH = m_pBoard->VISIBLE_HEIGHT * squareSize;

	static int boardOffsetX = (m_screenW - boardScreenW) / 2;
	static int boardOffsetY = (m_screenH - boardScreenH) / 2;

	// offsets are so the blocks don't overlap with the border
	SDL_Rect boardOutline =
	{
		boardOffsetX-1,
		boardOffsetY-1,
		boardScreenW+2,
		boardScreenH+2
	};

	SDL_SetRenderDrawColor(m_pRenderer, 18, 18, 18, 255);
	SDL_RenderClear(m_pRenderer);

	SDL_SetRenderDrawColor(m_pRenderer, 93, 93, 93, 255);
	SDL_RenderDrawRect(m_pRenderer, &boardOutline);

	// Drawing ghost
	int ghostIdx = m_pBoard->GetGhost();
	ghostIdx -= Board::VANISH_ZONE_HEIGHT * Board::WIDTH;
	DrawGhostPiece
	(
		ghostIdx,
		boardOffsetX,
		boardOffsetY,
		m_pBoard->GetFallingPiece(),
		m_pBoard->GetFallingPieceRot(),
		squareSize
	);


	for (int y = 0; y < Board::VISIBLE_HEIGHT; y++)
	{
		int absy = y * squareSize + boardOffsetY;

		for (int x = 0; x < Board::WIDTH; x++)
		{
			int absx = x * squareSize + boardOffsetX;

			int sq = m_pBoard->GetSquare(x, y+Board::VANISH_ZONE_HEIGHT);
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
			SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
			SDL_RenderFillRect(m_pRenderer, &sqRect);
		}
	}

	SDL_Color txtColor = { 255, 255, 255 };

	int heldPiece = m_pBoard->GetHeldPiece();
	if (heldPiece > 0)
	{
		const int heldPieceOffsetX = boardOffsetX - 6 * squareSize;
		const int heldPieceOffsetY = boardOffsetY + 2 * squareSize;
		DrawPiece(heldPieceOffsetX, heldPieceOffsetY, heldPiece, 0, squareSize);
		DrawTxt(heldPieceOffsetX + 2 * squareSize, heldPieceOffsetY - squareSize, "HOLD", m_pFont28, txtColor);
	}

	const int upNextOffsetX = boardOffsetX + boardScreenW + 2 * squareSize;
	const int upNextOffsetY = boardOffsetY + 2 * squareSize;
	for (int i = 0; i < 3; i++) 
	{
		DrawPiece(upNextOffsetX, upNextOffsetY + 3*i*squareSize, m_pBoard->NthPiece(i), 0, squareSize);
	}
	DrawTxt(upNextOffsetX + 2 * squareSize, upNextOffsetY - squareSize, "UP NEXT", m_pFont28, txtColor);

	if (m_pBoard->GameOver())
	{
		/*
		boardOutline = 
		{
			boardOffsetX,
			boardOffsetY,
			boardScreenW,
			boardScreenH
		};
		*/
		SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 150);
		SDL_RenderFillRect(m_pRenderer, &boardOutline);
		SDL_Color gameOverTxtColor = { 217, 59, 59 };
		DrawTxt(boardOffsetX + boardScreenW / 2, boardOffsetY + boardScreenH / 2 - 20, "GAME OVER", m_pFont40, gameOverTxtColor);
		DrawTxt(boardOffsetX + boardScreenW / 2, boardOffsetY + boardScreenH / 2 + 20, "Press R to Restart", m_pFont28, gameOverTxtColor);
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

void App::DrawPiece(int x, int y, int piece, int rot, int sqSize)
{
	if (piece != TetrominoData::I && piece != TetrominoData::O)
		x += sqSize / 2;
	if (piece == TetrominoData::I)
		y -= sqSize / 2;

	for (int i = 0; i < 4; i++)
	{
		int delta = TetrominoData::GetPieceMap(piece, rot, i);
		int r = Board::Row(delta);
		int c = Board::Col(delta);
		SDL_Rect pieceSq = {
			x + c * sqSize,
			y + r * sqSize,
			sqSize,
			sqSize
		};

		int hex = TetrominoData::hexCodes[piece - 1];
		SDL_Color color = ConvertHex(hex);
		SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(m_pRenderer, &pieceSq);
	}
}

void App::DrawGhostPiece(int boardPos, int xOffset, int yOffset, int piece, int rot, int sqSize)
{
	for (int i = 0; i < 4; i++)
	{
		int delta = TetrominoData::GetPieceMap(piece, rot, i);
		int c = Board::Col(boardPos + delta);
		int r = Board::Row(boardPos + delta);
		SDL_Rect pieceSq = {
			c * sqSize + xOffset,
			r * sqSize + yOffset,
			sqSize,
			sqSize
		};

		int hex = TetrominoData::hexCodes[piece - 1];
		SDL_Color color = ConvertHex(hex);
		color.a = 75;
		SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(m_pRenderer, &pieceSq);
	}
}

void App::DrawTxt(int x, int y, const char* txt, TTF_Font* font, SDL_Color color) 
{
	if (font == NULL)
	{
		std::cout << "Failed to load font" << std::endl;
		std::cout << "SDL_TTF ERR: " << TTF_GetError() << std::endl;
		return;
	}
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, txt, color);
	if (textSurface == NULL) 
	{
		std::cout << "Failed to initialize text surface" << std::endl;
		return;
	}
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);
	if (textTexture == NULL) 
	{
		std::cout << "Failed to initialize text texture" << std::endl;
		return;
	}
	
	SDL_Rect src = 
	{
		0,
		0,
		textSurface->w,
		textSurface->h
	};
	SDL_Rect dst =
	{
		x - src.w / 2,
		y - src.h / 2,
		src.w,
		src.h
	};


	SDL_RenderCopy(m_pRenderer, textTexture, &src, &dst);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

static void PersistantKey(const Uint8* keystate, KeyHandler& k, bool& input)
{
	if (keystate[k.scancode])
	{
		if (k.currentDelay == 0)
		{
			input = true;
			k.inputCount++;
		}
		SDL_Delay(1); // Delay one millisecond for framerate-independence
		k.currentDelay++;
		if (k.inputCount == 1)
		{
			if (k.currentDelay >= k.firstDelay)
				k.currentDelay = 0;
		}
		else if (k.currentDelay >= k.delay)
			k.currentDelay = 0;
	}
	else
	{
		k.currentDelay = 0;
		k.inputCount = 0;
	}
}

void App::Run()
{
	KeyHandler moveLeft			 = { SDL_SCANCODE_LEFT	, 25, 50, 0, 0 };
	KeyHandler moveRight		 = { SDL_SCANCODE_RIGHT	, 25, 50, 0, 0 };
	KeyHandler softDrop			 = { SDL_SCANCODE_DOWN	, 20, 20, 0, 0 };
	KeyHandler rotClockwise		 = { SDL_SCANCODE_UP	, 75, 75, 0, 0 };
	KeyHandler rotCountClockwise = { SDL_SCANCODE_Z		, 75, 75, 0, 0 };

	SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);

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
				if (event.key.keysym.sym == SDLK_r) 
				{
					delete m_pBoard;
					m_pBoard = new Board(250);
					continue;
				}
			}
		}

		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		PersistantKey(keystate, softDrop, input.softDrop);
		PersistantKey(keystate, moveLeft, input.moveLeft);
		PersistantKey(keystate, moveRight, input.moveRight);
		PersistantKey(keystate, rotClockwise, input.rotClockwise);
		PersistantKey(keystate, rotCountClockwise, input.rotCountClockwise);

		Uint32 currentTimeMs = SDL_GetTicks();
		m_pBoard->Update(input, (int)currentTimeMs);

		Draw();
	}

}

void App::ShutDown()
{
	delete m_pBoard;
	m_pBoard = nullptr;

	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	TTF_CloseFont(m_pFont28);
	TTF_CloseFont(m_pFont40);

	TTF_Quit();
	SDL_Quit();
}