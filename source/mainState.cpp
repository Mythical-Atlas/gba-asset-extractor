#include <string>
#include <iostream>

#include "windows.h"
#include "psapi.h"

#include "mainState.hpp"
#include "audio.hpp"

#define VERTS_SIZE 16
#define NUM_SPRITES 4

using namespace std;

void MainState::load()  {
	rp = RenderProgram("Sprite Program");
    rp.attachShader(GL_VERTEX_SHADER, "resources/versatileShader.vert");
    rp.attachShader(GL_FRAGMENT_SHADER, "resources/versatileShader.frag");
    rp.link();

	fontTexture.load("resources/font.png");
    wireTexture.load("resources/wires.png");
    invertFontTexture.load("resources/inverted font.png");

    glGenTextures(1, &tileTexture.pointer);
    glBindTexture(GL_TEXTURE_2D, tileTexture.pointer);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 8 * 64, 8 * 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);

    tileTexture.size[0] = 8 * 64;
    tileTexture.size[1] = 8 * 64;

    fontSprite.init(&fontTexture, 0, 0, 0, 8, 12); // 512x144
    wireSprite.init(&wireTexture, 1, 0, 0, 8, 12); // 112x48
    invertFontSprite.init(&invertFontTexture, 0, 0, 0, 8, 12); // 512x144

    tileSprite.init(&tileTexture, 3, 0, 0, 8, 8);

    int attribSizes[2] = {2, 2};
    rb = RenderBuffer(2, attribSizes, NUM_SPRITES * VERTS_SIZE);

    float vertDataBuffer[VERTS_SIZE];
    fontSprite.getData(vertDataBuffer); rb.uploadData(0 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    wireSprite.getData(vertDataBuffer); rb.uploadData(1 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    invertFontSprite.getData(vertDataBuffer); rb.uploadData(2 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);
    tileSprite.getData(vertDataBuffer); rb.uploadData(3 * VERTS_SIZE, VERTS_SIZE, vertDataBuffer);

    romFile = ifstream("resources/sma4.gba", ios::binary);
}
void MainState::init(Window* window, Game* game)  {
    cam.init((int)window->getScreenSize().x, (int)window->getScreenSize().y);

    mixer.init();
    memset(&controller, 0, sizeof(Controller)); // could be done with a union in Controller

    stateStartTime = steady_clock::now();
    frameStartTime = stateStartTime;

    debugPrintTimer = 0;

    tileOffset = 0;
    tileTextureOffset = 0;
    initTexture();
}
void MainState::update(Window* window, Game* game)  {
    time_point<steady_clock> currentTime = steady_clock::now();
    dt = duration_cast<milliseconds>(currentTime - frameStartTime).count();
    frameStartTime = currentTime;
    timeSinceInit = duration_cast<milliseconds>(currentTime - stateStartTime).count();

    ticksSinceLastDebugPrint++;

    if(controller.up) {
        controller.up = false;
        tileOffset -= 64;
        updateTexture(-64);
    }
    if(controller.down) {
        controller.down = false;
        tileOffset += 64;
        updateTexture(64);
    }
}

void MainState::initTexture() {
    uint8_t tileData[4 * 8 * 8];
    uint8_t rawData[32];

    for(int i = 0; i < 64 * 64; i++) {
        romFile.seekg(0x1990B4 + i * 32, std::ios::beg);
        romFile.read((char*)rawData, 32);

        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                uint8_t colorIndex = (rawData[y * 4 + (int)(x / 2)] >> ((x % 2) * 4)) & 0xF;
                if(colorIndex >= 0 && colorIndex <= 15) {
                    uint32_t color = palette[colorIndex];

                    tileData[(x + y * 8) * 4 + 0] = (color & 0xFF000000) >> 24; // r
                    tileData[(x + y * 8) * 4 + 1] = (color & 0x00FF0000) >> 16; // g
                    tileData[(x + y * 8) * 4 + 2] = (color & 0x0000FF00) >>  8; // b
                    tileData[(x + y * 8) * 4 + 3] = (color & 0x000000FF) >>  0; // a
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, tileTexture.pointer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, (i % 64) * 8, (int)(i / 64) * 8, 8, 8, GL_RGBA, GL_UNSIGNED_BYTE, tileData);
    }
}
void MainState::updateTexture(int tileMovement) {
    if(tileMovement == 0) {return;}

	uint8_t tileData[4 * 8 * 8];
    uint8_t rawData[32];

    if(tileMovement < 0) {
        for(int i = 0; i < abs(tileMovement); i++) {
            if(tileTextureOffset > 0) {tileTextureOffset--;}
            else {tileTextureOffset = 64 * 64 - 1;}
        }
    }

    for(int i = 0; i < abs(tileMovement); i++) {
        if(tileMovement < 0) {romFile.seekg(0x1990B4 + (i + tileOffset) * 32, std::ios::beg);}
        else {romFile.seekg(0x1990B4 + (i + tileOffset + 63 * 64) * 32, std::ios::beg);}
        romFile.read((char*)rawData, 32);

        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++) {
                uint8_t colorIndex = (rawData[y * 4 + (int)(x / 2)] >> ((x % 2) * 4)) & 0xF;
                if(colorIndex >= 0 && colorIndex <= 15) {
                    uint32_t color = palette[colorIndex];

                    tileData[(x + y * 8) * 4 + 0] = (color & 0xFF000000) >> 24; // r
                    tileData[(x + y * 8) * 4 + 1] = (color & 0x00FF0000) >> 16; // g
                    tileData[(x + y * 8) * 4 + 2] = (color & 0x0000FF00) >>  8; // b
                    tileData[(x + y * 8) * 4 + 3] = (color & 0x000000FF) >>  0; // a
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, tileTexture.pointer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, (tileTextureOffset % 64) * 8, (int)(tileTextureOffset / 64) * 8, 8, 8, GL_RGBA, GL_UNSIGNED_BYTE, tileData);
        
        tileTextureOffset++;
        if(tileTextureOffset >= 64 * 64) {tileTextureOffset = 0;}
    }

    if(tileMovement < 0) {
        for(int i = 0; i < abs(tileMovement); i++) {
            if(tileTextureOffset > 0) {tileTextureOffset--;}
            else {tileTextureOffset = 64 * 64 - 1;}
        }
    }
}

void MainState::render(Window* window, Game* game)  { // TODO: layering using z position
    if(timeSinceInit - debugPrintTimer >= 1000) {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

        float tickRate = (float)ticksSinceLastDebugPrint / (timeSinceInit - debugPrintTimer) * 1000.0f;

        cout << "Virtual memory used: " << virtualMemUsedByMe << " bytes" << endl;
        cout << "Tick rate: " << tickRate << " fps" << endl;

        ticksSinceLastDebugPrint = 0;
        while(timeSinceInit - debugPrintTimer >= 1000) {debugPrintTimer += 1000;}
    }

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rp.useViewMatrix(&cam);

    renderString(fontSprite, vec2(600, 100), "tileTextureOffset = 0x" + hex32(tileTextureOffset), &rp, &rb);
    renderString(fontSprite, vec2(600, 112), "tileOffset = 0x" + hex32(tileOffset), &rp, &rb);

    for(int i = 0; i < 64 * 64; i++) {
        int drawIndex = (i - tileTextureOffset) % (64 * 64);
        //drawIndex = i;
        //drawIndex = (i - tileOffset) % (64 * 64);

        /*tileSprite.pos = vec2((i % 16) * 8,  (int)(i / 16) * 8);
        tileSprite.scale = vec2(1, 1);
        tileSprite.render(&rp, &rb, i % 64, (int)(i / 64));*/

        tileSprite.pos = vec2((drawIndex % 64) * 8,  (int)(drawIndex / 64) * 8);
        //tileSprite.scale = vec2(2, 2);
        tileSprite.render(&rp, &rb, i % 64, (int)(i / 64));
    }
}
void MainState::unload() {
	mixer.unload();
}

void MainState::handleEvent(SDL_Event* event) {controller.handleEvent(event);}