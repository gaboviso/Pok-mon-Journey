#include "engine/App.hpp"

#include <algorithm>
#include <cstdio>

namespace journey {

App::App() = default;

App::~App() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }

    if (window_) {
        SDL_DestroyWindow(window_);
    }

    SDL_Quit();
}

bool App::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow(
        "Pokemon Journey 0.1",
        1280,
        720,
        SDL_WINDOW_RESIZABLE
    );

    if (!window_) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);

    if (!renderer_) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }

    running_ = true;
    return true;
}

void App::run() {
    Uint64 previousTicks = SDL_GetTicks();

    while (running_) {
        const Uint64 currentTicks = SDL_GetTicks();

        float deltaTime =
            static_cast<float>(currentTicks - previousTicks) / 1000.0f;

        previousTicks = currentTicks;
        deltaTime = std::min(deltaTime, 0.05f);

        processEvents();
        update(deltaTime);
        render();
    }
}

void App::processEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running_ = false;
        }
    }
}

void App::update(float deltaTime) {
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP]) {
        dy -= 1.0f;
    }

    if (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN]) {
        dy += 1.0f;
    }

    if (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT]) {
        dx -= 1.0f;
    }

    if (keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT]) {
        dx += 1.0f;
    }

    playerX_ += dx * playerSpeed_ * deltaTime;
    playerY_ += dy * playerSpeed_ * deltaTime;

    int width = 0;
    int height = 0;

    SDL_GetRenderOutputSize(renderer_, &width, &height);

    playerX_ = std::clamp(
        playerX_,
        0.0f,
        static_cast<float>(width) - playerSize_
    );

    playerY_ = std::clamp(
        playerY_,
        0.0f,
        static_cast<float>(height) - playerSize_
    );
}

void App::render() {
    int width = 0;
    int height = 0;

    SDL_GetRenderOutputSize(renderer_, &width, &height);

    SDL_SetRenderDrawColor(renderer_, 42, 90, 62, 255);
    SDL_RenderClear(renderer_);

    SDL_FRect path {
        0.0f,
        static_cast<float>(height) * 0.42f,
        static_cast<float>(width),
        150.0f
    };

    SDL_SetRenderDrawColor(renderer_, 174, 145, 101, 255);
    SDL_RenderFillRect(renderer_, &path);

    SDL_FRect player {
        playerX_,
        playerY_,
        playerSize_,
        playerSize_
    };

    SDL_SetRenderDrawColor(renderer_, 238, 238, 245, 255);
    SDL_RenderFillRect(renderer_, &player);

    SDL_RenderPresent(renderer_);
}

} // namespace journey
