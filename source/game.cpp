#include "game.hpp"
#include "scrollState.hpp"

void Game::changeState(int state) {
    if(this->state != nullptr) {this->state->unload();}

    this->state = states[state];

    this->state->load();
    this->state->init(window, this);
}

void Game::init(Window* window) {
    this->window = window;

    states[0] = new ScrollState;

    state = nullptr;

    changeState(0);
    running = true;
}

void Game::update() {state->update(window, this);}
void Game::render() {state->render(window, this);}

void Game::handleEvent(SDL_Event* event) {state->handleEvent(event);}