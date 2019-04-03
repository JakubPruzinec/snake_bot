#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils.h"


constexpr std::size_t SCREEN_WIDTH =	640;
constexpr std::size_t SCREEN_HEIGHT =	480;

#define COLOR_RED	1
#define COLOR_GREEN	2

extern SDL_Window	*window;
extern SDL_Renderer	*renderer;

/**
 * Inits SDL structures
 * @param w Canvas width
 * @param h Canvas height
 * @return Success
 */
bool initMedia(std::size_t w, std::size_t h);

/**
 * Loads necessary media
 * @return Success
 */
bool loadMedia(); 

/**
 * Cleanup
 */
void close();

/**
 * Loads teacture from image
 * @param name Filename
 * @return Texture
 */
SDL_Texture *loadTexture(const std::string &name);

/**
 * Clears window
 */
void clearWindow();

/**
 * Draws square at given coordinates
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Square Width
 * @param color Color of square
 */
void drawSquare(std::size_t x, std::size_t y, std::size_t w, int color);

/**
 * Renders window
 */
void renderWindow();