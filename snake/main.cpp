#include "main.h"


int main(int argc, char* argv[])
{
	(void)argc; (void)argv;
	Game game(20, 15);
	std::size_t startTime = 0;

	RETERR(!game.init(), "Failed to init graphics\n");
	RETERR(!loadMedia(), "Failed to load media\n");

	game.render();
	for (bool quit = false; !quit; )
	{
		SDL_Event e;

		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				break;
			}

			game.handleKey(e);
		}

		auto ticks = SDL_GetTicks();
		if (ticks - startTime >= 100)
		{
			startTime = ticks;
			game.tick();
			game.render();
		}
	}

	close();
	return 0;

error:
	close();
	return 1;
}