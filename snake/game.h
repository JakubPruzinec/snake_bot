#pragma once

#include <list>

/* 40x40 px cells */
constexpr std::size_t CELL_DIMENSION = 40;

struct Coords
{
	std::size_t x;
	std::size_t y;

	Coords() {};

	Coords(std::size_t _x, size_t _y) : x(_x), y(_y)
	{
	};
};

enum class Direction {UP, DOWN, LEFT, RIGHT};

struct Snake
{
	Direction dir;
	Direction declDir;
	std::list<struct Coords> body;
};

class Game
{
	private:
		struct Snake snake;
		struct Coords food;
		std::size_t width;
		std::size_t height;

		/**
		 * Generates food coordinates
		 */
		void generateFood();
		std::size_t getPixelWidth() const;
		std::size_t getPixelHeight() const;

	public:
		Game(std::size_t w, std::size_t h);
		~Game();

		/**
		 * Initializes game
		 * @return Success
		 */
		bool init();

		/**
		 * Handles user key input
		 */
		void handleKey(const SDL_Event &e);

		/**
		 * Ticks periodically
		 */
		void tick();

		/**
		 * Renders game field
		 */
		void render() const;
};