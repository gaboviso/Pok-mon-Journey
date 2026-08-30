#pragma once

#include <SDL3/SDL.h>

namespace journey {

class App {
public:
    App();
    ~App();

    bool initialize();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    bool running_ = false;

    float playerX_ = 400.0f;
    float playerY_ = 300.0f;
    float playerSpeed_ = 220.0f;
    float playerSize_ = 32.0f;
};

} // namespace journey
