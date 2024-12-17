#include "Player.h"
#include "ResourceManager.h"
#include "GameState.h"
#include <ctime>
#include "Background.h"

const Color BACKGROUND_COLOR = Color{ 255, 239, 213, 255 };
const Color PAUSED_TEXT_COLOR = Color{ 255, 165, 0, 255 };
const Color INSTRUCTION_TEXT_COLOR = Color{ 255, 69, 0, 255 };
const float SUSPICIOUS_DELTA_TIME_THRESHOLD = 0.1f;
const float MAX_FPS = 60.0f;
const std::string SAVE_FILE = "savegame.dat";

int main() {
    srand(static_cast<unsigned int>(time(0)));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple game loop");
    InitAudioDevice();

    ResourceManager resourceManager;
    GameState gameState;

    gameState.Register(std::make_unique<Background>("resources/b4.png", resourceManager));
    gameState.Register(std::make_unique<Player>(
        Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "resources/p1.png",
        Vector2{ 180, 180 },
        "resources/audiomass-output.mp3",
        resourceManager
    ));

    bool isPaused = false;
    SetTargetFPS(MAX_FPS);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        float deltaTime = GetFrameTime();

        if (!isPaused && IsWindowFocused() && !(deltaTime > SUSPICIOUS_DELTA_TIME_THRESHOLD)) {
            gameState.Update(deltaTime);
            //player.Update(deltaTime);
            //player.ConstrainToBounds(SCREEN_WIDTH, SCREEN_HEIGHT);
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

        if (isPaused) DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 20, PAUSED_TEXT_COLOR);
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
