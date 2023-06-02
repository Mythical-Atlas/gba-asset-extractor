#ifndef MAIN_STATE_H
#define MAIN_STATE_H

#include <chrono>

#include "game.hpp"
#include "graphics.hpp"
#include "renderProgram.hpp"
#include "renderBuffer.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "audio.hpp"
#include "controller.hpp"

using namespace chrono;

class MainState: public State {
public:
	time_point<steady_clock> stateStartTime;
	time_point<steady_clock> frameStartTime;

	uint32_t dt;
	uint32_t timeSinceInit;

	uint32_t ticksSinceLastDebugPrint;
	uint32_t debugPrintTimer;

	RenderProgram rp;
	RenderBuffer rb;
	Camera cam;
	Controller controller;
	AudioMixer mixer;

	Texture invertFontTexture;
	Texture fontTexture;
	Texture wireTexture;

	Sprite invertFontSprite;
	Sprite fontSprite;
	Sprite wireSprite;

	void load();
	void init(class Window* window, class Game* game);
	void update(Window* window, Game* game);
	void render(Window* window, Game* game);
	void unload();

	void handleEvent(SDL_Event* event);

	void renderString(Sprite font, vec2 pos, string text, RenderProgram* rp, RenderBuffer* rb) {
		int line = 0;
		int offset = 0;
		const char* chars = text.c_str();

		for(int c = 0; c < text.length(); c++) {
			font.pos = pos + vec2((c - offset) * 8, line * 12);

			if(chars[c] == '\n') {
				line++;
				offset = c + 1;
			}
			else if(chars[c] == 0) {}
			else {font.render(rp, rb, (chars[c] - 32) % 64, (int)((chars[c] - 32) / 64));}
		}
	}
};

#endif