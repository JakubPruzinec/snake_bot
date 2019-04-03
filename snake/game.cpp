#include <SDL2/SDL.h>

#include <iostream>
#include <ctime>
#include <algorithm>

#include "game.h"
#include "graphics.h"

Game::Game(std::size_t w, std::size_t h)
{
	std::srand(std::time(nullptr));

	width = (w < 10) ? 10 : w;
	height = (h < 10) ? 10 : h;

	for (std::size_t i = 0; i < 5; i++)
		snake.body.emplace_back(i, height / 2);

	snake.dir = Direction::RIGHT;
	snake.declDir = Direction::RIGHT;

	generateFood();
}

Game::~Game()
{
}

void Game::handleKey(const SDL_Event &e)
{
	if (e.type != SDL_KEYDOWN)
		return;

	switch (e.key.keysym.sym)
	{
		case SDLK_UP:
			if (snake.dir != Direction::DOWN)
				snake.declDir = Direction::UP;
			break;
		case SDLK_DOWN:
			if (snake.dir != Direction::UP)
				snake.declDir = Direction::DOWN;
			break;
		case SDLK_LEFT:
			if (snake.dir != Direction::RIGHT)
				snake.declDir = Direction::LEFT;
			break;
		case SDLK_RIGHT:
			if (snake.dir != Direction::LEFT)
				snake.declDir = Direction::RIGHT;
			break;
	}
}

void Game::tick()
{
	auto newX = snake.body.back().x;
	auto newY = snake.body.back().y;

	snake.dir = snake.declDir;

	switch (snake.dir)
	{
		case Direction::UP:
			if (newY == 0)
				exit(0);
			newY--;
			break;
		case Direction::DOWN:
			if (newY == height - 1)
				exit(0);
			newY++;
			break;
		case Direction::LEFT:
			if (newX == 0)
				exit(0);
			newX--;
			break;
		case Direction::RIGHT:
			if (newX == width - 1)
				exit(0);
			newX++;
			break;
	}

	auto found = std::find_if(snake.body.begin(), snake.body.end(),
		[newX, newY](struct Coords &cell)->bool {
			return newX == cell.x && newY == cell.y;
		});

	if (found != snake.body.end())
	{
		exit(0);
	}

	snake.body.emplace_back(newX, newY);

	if (newX == food.x && newY == food.y)
	{
		if (snake.body.size() == width * height)
		{
			std::cout << "Congrats\n";
			exit(0);
		}
		generateFood();
	}
	else
		snake.body.pop_front();
}

void Game::generateFood()
{
	while (1)
	{
		auto x = std::rand() % width;
		auto y = std::rand() % height;

		auto found = std::find_if(snake.body.begin(), snake.body.end(),
			[x, y](struct Coords &cell)->bool {
				return x == cell.x && y == cell.y;
			});

		if (found == snake.body.end())
		{
			food.x = x;
			food.y = y;
			return;
		}
	}
}

void Game::render() const
{
	clearWindow();

	for (const auto &cell : snake.body)
		drawSquare(cell.x * CELL_DIMENSION, cell.y * CELL_DIMENSION, CELL_DIMENSION, COLOR_RED);

	drawSquare(food.x * CELL_DIMENSION, food.y * CELL_DIMENSION, CELL_DIMENSION, COLOR_GREEN);
	renderWindow();
}

bool Game::init()
{
	return initMedia(getPixelWidth(), getPixelHeight());
}

std::size_t Game::getPixelWidth() const
{
	return width * CELL_DIMENSION;
}

std::size_t Game::getPixelHeight() const
{
	return height * CELL_DIMENSION;
}