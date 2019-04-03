#include "graphics.h"

SDL_Window		*window = nullptr;
SDL_Renderer	*renderer = nullptr;

bool initMedia(std::size_t w, std::size_t h)
{
	int imgFlags = IMG_INIT_PNG;
	
	RETERR(SDL_Init(SDL_INIT_VIDEO) < 0, "Failed to init SDL\n");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
										w, h, SDL_WINDOW_SHOWN);
	RETERR(!window, "Failed to create window\n");
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	RETERR(!renderer, "Failed to create renderer\n");

	RETERR(!(IMG_Init(imgFlags) & imgFlags), "Failed to set flags\n");

	return true;

error:
	return false;
}


bool loadMedia()
{
	return true;
}


void close()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = nullptr;
	renderer = nullptr;

	IMG_Quit();
	SDL_Quit();
}


SDL_Texture* loadTexture(const std::string &name)
{
	SDL_Texture *newTexture = nullptr;
	SDL_Surface *loadedSurface = IMG_Load(name.c_str());
	RETERR(!loadedSurface, "Failed to load surface: %s", name.c_str());
	
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    RETERR(!newTexture, "Failed to create texture\n");

	SDL_FreeSurface(loadedSurface);

	return newTexture;

error:
	return nullptr;
}

void clearWindow()
{
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
}

void drawSquare(std::size_t x, std::size_t y, std::size_t w, int color)
{
	SDL_Rect square = {(int)x, (int)y, (int)w, (int)w};

	if (color == COLOR_RED)
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
	else
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

	SDL_RenderFillRect(renderer, &square);
}

void renderWindow()
{
	SDL_RenderPresent(renderer);
}