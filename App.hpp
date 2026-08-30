#pragma once

#include <SDL3/SDL.h>
#include <array>

namespace journey {

class App {
public:
    App();
    ~App();

    bool initialize();
    void run();

private:
    static constexpr int TILE_SIZE = 64;
    static constexpr int MAP_WIDTH = 30;
    static constexpr int MAP_HEIGHT = 22;

    enum class Tile {
        Grass,
        Path,
        Water,
        Tree,
        Rock
    };

    struct Player {
        float x = 6.0f * TILE_SIZE;
        float y = 6.0f * TILE_SIZE;
        float width = 36.0f;
        float height = 48.0f;
        float walkSpeed = 210.0f;
        float runSpeed = 340.0f;
    };

    struct Camera {
        float x = 0.0f;
        float y = 0.0f;
    };

    void createMap();
    void processEvents();
    void update(float deltaTime);
    void render();

    void updatePlayer(float deltaTime);
    void updateCamera();

    bool canMoveTo(float x, float y) const;
    bool isSolidTile(int tileX, int tileY) const;

    void drawWorld();
    void drawTile(Tile tile, float x, float y);
    void drawPlayer();

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    bool running_ = false;

    Player player_;
    Camera camera_;

    std::array<Tile, MAP_WIDTH * MAP_HEIGHT> map_;
};

} // namespace journey
