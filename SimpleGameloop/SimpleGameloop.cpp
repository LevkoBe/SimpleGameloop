#include "Player.h"
#include "ResourceManager.h"
#include "GameState.h"
#include "Background.h"
#include <ctime>
#include "SpriteFactory.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const Color BACKGROUND_COLOR = Color{ 255, 239, 213, 255 };
const Color PAUSED_TEXT_COLOR = Color{ 255, 165, 0, 255 };
const Color INSTRUCTION_TEXT_COLOR = Color{ 255, 69, 0, 255 };
const float SUSPICIOUS_DELTA_TIME_THRESHOLD = 0.1f;
const float MAX_FPS = 60.0f;
const std::string SAVE_FILE = "savegame.dat";

int main() {
    int lastSpriteId = -1;

    srand(static_cast<unsigned int>(time(0)));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game with Scene Graph and Quadtree");
    InitAudioDevice();

    ResourceManager resourceManager;
    GameState gameState(resourceManager, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });

    auto backgroundSprites = SpriteFactory::CreateSprites("Background", 1, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, resourceManager);
    for (auto& sprite : backgroundSprites) lastSpriteId = gameState.RegisterEntity(std::move(sprite));

    auto playerSprites = SpriteFactory::CreateSprites("Player", 3, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2}, resourceManager);
    for (auto& sprite : playerSprites) lastSpriteId = gameState.RegisterEntity(std::move(sprite));
    int mainSprite = lastSpriteId;

    auto otherSprites = SpriteFactory::CreateSprites("Player", 3, { 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT }, resourceManager);
    for (auto& sprite : otherSprites) lastSpriteId = gameState.RegisterEntity(std::move(sprite));

    bool isPaused = false;
    SetTargetFPS(MAX_FPS);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        float deltaTime = GetFrameTime();

        if (!isPaused && IsWindowFocused() && !(deltaTime > SUSPICIOUS_DELTA_TIME_THRESHOLD)) {
            gameState.Update(deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        if (IsKeyPressed(KEY_ZERO)) {
            std::ofstream file(SAVE_FILE, std::ios::binary);
            if (file.is_open()) {
                gameState.Save(file);
                file.close();
            }
        }

        if (IsKeyPressed(KEY_ONE)) {
            std::ifstream file(SAVE_FILE, std::ios::binary);
            if (file.is_open()) {
                gameState.Load(file);
                file.close();
            }
        }

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        if (isPaused) {
            DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 20, PAUSED_TEXT_COLOR);
        }
        else {
            gameState.Draw();
            DrawText("Use WASD to control speed, P to pause.", 10, 10, 20, INSTRUCTION_TEXT_COLOR);
            DrawText("Press 0 to Save, 1 to Load.", 10, 30, 20, INSTRUCTION_TEXT_COLOR);
        }

        EndDrawing();
    }

    resourceManager.UnloadAll();
    CloseWindow();
    return 0;
}
