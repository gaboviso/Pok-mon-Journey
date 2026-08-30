#include "engine/App.hpp"

#include <algorithm>
#include <cmath>
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
        "Pokemon Journey 0.2",
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

    createMap();
    running_ = true;
    return true;
}

void App::createMap() {
    map_.fill(Tile::Grass);

    // Caminho horizontal principal
    for (int y = 9; y <= 11; ++y) {
        for (int x = 1; x < MAP_WIDTH - 1; ++x) {
            map_[y * MAP_WIDTH + x] = Tile::Path;
        }
    }

    // Caminho vertical
    for (int y = 3; y <= 18; ++y) {
        for (int x = 13; x <= 15; ++x) {
            map_[y * MAP_WIDTH + x] = Tile::Path;
        }
    }

    // Lago
    for (int y = 3; y <= 7; ++y) {
        for (int x = 19; x <= 25; ++x) {
            map_[y * MAP_WIDTH + x] = Tile::Water;
        }
    }

    // Bordas com árvores
    for (int x = 0; x < MAP_WIDTH; ++x) {
        map_[x] = Tile::Tree;
        map_[(MAP_HEIGHT - 1) * MAP_WIDTH + x] = Tile::Tree;
    }

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        map_[y * MAP_WIDTH] = Tile::Tree;
        map_[y * MAP_WIDTH + (MAP_WIDTH - 1)] = Tile::Tree;
    }

    // Bosque
    for (int y = 3; y <= 7; ++y) {
        for (int x = 3; x <= 7; ++x) {
            if ((x + y) % 2 == 0) {
                map_[y * MAP_WIDTH + x] = Tile::Tree;
            }
        }
    }

    // Rochas
    map_[5 * MAP_WIDTH + 10] = Tile::Rock;
    map_[6 * MAP_WIDTH + 10] = Tile::Rock;
    map_[14 * MAP_WIDTH + 20] = Tile::Rock;
    map_[15 * MAP_WIDTH + 21] = Tile::Rock;
    map_[16 * MAP_WIDTH + 20] = Tile::Rock;
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

        if (event.type == SDL_EVENT_KEY_DOWN &&
            event.key.scancode == SDL_SCANCODE_ESCAPE) {
            running_ = false;
        }
    }
}

void App::update(float deltaTime) {
    updatePlayer(deltaTime);
    updateCamera();
}

void App::updatePlayer(float deltaTime) {
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

    if (dx != 0.0f || dy != 0.0f) {
        const float length = std::sqrt(dx * dx + dy * dy);
        dx /= length;
        dy /= length;
    }

    const bool running =
        keyboard[SDL_SCANCODE_LSHIFT] ||
        keyboard[SDL_SCANCODE_RSHIFT];

    const float speed = running ? player_.runSpeed : player_.walkSpeed;

    const float nextX = player_.x + dx * speed * deltaTime;
    const float nextY = player_.y + dy * speed * deltaTime;

    if (canMoveTo(nextX, player_.y)) {
        player_.x = nextX;
    }

    if (canMoveTo(player_.x, nextY)) {
        player_.y = nextY;
    }
}

bool App::canMoveTo(float x, float y) const {
    const float inset = 5.0f;

    const float left = x + inset;
    const float right = x + player_.width - inset;
    const float top = y + player_.height * 0.45f;
    const float bottom = y + player_.height - 2.0f;

    const int leftTile = static_cast<int>(left / TILE_SIZE);
    const int rightTile = static_cast<int>(right / TILE_SIZE);
    const int topTile = static_cast<int>(top / TILE_SIZE);
    const int bottomTile = static_cast<int>(bottom / TILE_SIZE);

    return
        !isSolidTile(leftTile, topTile) &&
        !isSolidTile(rightTile, topTile) &&
        !isSolidTile(leftTile, bottomTile) &&
        !isSolidTile(rightTile, bottomTile);
}

bool App::isSolidTile(int tileX, int tileY) const {
    if (tileX < 0 || tileY < 0 ||
        tileX >= MAP_WIDTH || tileY >= MAP_HEIGHT) {
        return true;
    }

    const Tile tile = map_[tileY * MAP_WIDTH + tileX];

    return tile == Tile::Water ||
           tile == Tile::Tree ||
           tile == Tile::Rock;
}

void App::updateCamera() {
    int screenWidth = 0;
    int screenHeight = 0;

    SDL_GetRenderOutputSize(renderer_, &screenWidth, &screenHeight);

    const float mapPixelWidth =
        static_cast<float>(MAP_WIDTH * TILE_SIZE);

    const float mapPixelHeight =
        static_cast<float>(MAP_HEIGHT * TILE_SIZE);

    camera_.x =
        player_.x + player_.width * 0.5f -
        static_cast<float>(screenWidth) * 0.5f;

    camera_.y =
        player_.y + player_.height * 0.5f -
        static_cast<float>(screenHeight) * 0.5f;

    camera_.x = std::clamp(
        camera_.x,
        0.0f,
        std::max(0.0f, mapPixelWidth - static_cast<float>(screenWidth))
    );

    camera_.y = std::clamp(
        camera_.y,
        0.0f,
        std::max(0.0f, mapPixelHeight - static_cast<float>(screenHeight))
    );
}

void App::render() {
    SDL_SetRenderDrawColor(renderer_, 28, 42, 35, 255);
    SDL_RenderClear(renderer_);

    drawWorld();
    drawPlayer();

    SDL_RenderPresent(renderer_);
}

void App::drawWorld() {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            const Tile tile = map_[y * MAP_WIDTH + x];

            const float worldX =
                static_cast<float>(x * TILE_SIZE) - camera_.x;

            const float worldY =
                static_cast<float>(y * TILE_SIZE) - camera_.y;

            drawTile(tile, worldX, worldY);
        }
    }
}

void App::drawTile(Tile tile, float x, float y) {
    SDL_FRect rect{
        x,
        y,
        static_cast<float>(TILE_SIZE),
        static_cast<float>(TILE_SIZE)
    };

    switch (tile) {
        case Tile::Grass:
            SDL_SetRenderDrawColor(renderer_, 76, 139, 84, 255);
            break;

        case Tile::Path:
            SDL_SetRenderDrawColor(renderer_, 184, 154, 108, 255);
            break;

        case Tile::Water:
            SDL_SetRenderDrawColor(renderer_, 69, 140, 191, 255);
            break;

        case Tile::Tree:
            SDL_SetRenderDrawColor(renderer_, 31, 88, 53, 255);
            break;

        case Tile::Rock:
            SDL_SetRenderDrawColor(renderer_, 98, 104, 105, 255);
            break;
    }

    SDL_RenderFillRect(renderer_, &rect);

    // Grade sutil para visualizar os tiles nesta etapa
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 18);
    SDL_RenderRect(renderer_, &rect);
}

void App::drawPlayer() {
    SDL_FRect shadow{
        player_.x - camera_.x + 3.0f,
        player_.y - camera_.y + player_.height - 9.0f,
        player_.width - 6.0f,
        10.0f
    };

    SDL_SetRenderDrawColor(renderer_, 25, 35, 30, 110);
    SDL_RenderFillRect(renderer_, &shadow);

    SDL_FRect body{
        player_.x - camera_.x,
        player_.y - camera_.y,
        player_.width,
        player_.height
    };

    SDL_SetRenderDrawColor(renderer_, 235, 239, 246, 255);
    SDL_RenderFillRect(renderer_, &body);

    SDL_FRect head{
        body.x + 6.0f,
        body.y + 5.0f,
        body.w - 12.0f,
        18.0f
    };

    SDL_SetRenderDrawColor(renderer_, 55, 70, 98, 255);
    SDL_RenderFillRect(renderer_, &head);
}

} // namespace journey
