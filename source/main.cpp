#include <string>
#include <iostream>
#include <fstream>

#include "main.hpp"
#include "window.hpp"
#include "audio.hpp"
#include "game.hpp"

using namespace std;

int main(int argc, char* args[]) {
	Window window;
	window.init(false, 1824, 1086, "GBA Asset Extractor");

	Game game;
	game.init(&window);

	while(game.running) {
		game.update();
		game.render();

		window.swap();
		SDL_Event event;
		while(window.pollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					game.running = false;
					break;
				default: game.handleEvent(&event);
			}
		}
	}

	SDL_Quit();

	return 0;
}