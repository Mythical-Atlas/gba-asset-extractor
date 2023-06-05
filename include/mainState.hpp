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
	Texture tileTexture;

	Sprite invertFontSprite;
	Sprite fontSprite;
	Sprite wireSprite;
	Sprite tileSprite;

	std::ifstream romFile;

	uint32_t palette[16] = {
		0xb0e0f8ff, 0xf8f8f8ff, 0x303030ff, 0xb86800ff, 0xe89048ff, 0xf0d848ff, 0xf07868ff, 0xf0a070ff,
		0xf8c890ff, 0x885840ff, 0x406090ff, 0x5088b8ff, 0x60a8e0ff, 0xf83050ff, 0xd80028ff, 0xb00000ff
	};

	int32_t tileOffset;
	uint32_t tileTextureOffset;

	void load();
	void init(class Window* window, class Game* game);
	void update(Window* window, Game* game);
	void render(Window* window, Game* game);
	void unload();

	void handleEvent(SDL_Event* event);

	void initTexture();
	void updateTexture(int tileMovement);

	string hex32(uint32_t value) {
        char output[8];
        uint8_t temp;
        
        for(int i = 0; i < 8; i++) {
            temp = (value >> (i * 4)) & 0xF;

            if(temp < 0xA) {output[7 - i] = temp + '0';}
            else {output[7 - i] = temp - 0xA + 'A';}
        }

        return string(output);
    }
	string hex16(uint16_t value) {
        char output[4];
        uint8_t temp;
        
        for(int i = 0; i < 4; i++) {
            temp = (value >> (i * 4)) & 0xF;

            if(temp < 0xA) {output[3 - i] = temp + '0';}
            else {output[3 - i] = temp - 0xA + 'A';}
        }

        return string(output);
    }
    string hex8(uint8_t value) {
        char output[2];
        uint8_t temp;
        
        for(int i = 0; i < 2; i++) {
            temp = (value >> (i * 4)) & 0xF;

            if(temp < 0xA) {output[1 - i] = temp + '0';}
            else {output[1 - i] = temp - 0xA + 'A';}
        }

        return string(output);
    }

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