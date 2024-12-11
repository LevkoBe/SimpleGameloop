#include "Player.h"
#include "ResourceManager.h"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const Color BACKGROUND_COLOR = Color{ 255, 239, 213, 255 };
const Color PAUSED_TEXT_COLOR = Color{ 255, 165, 0, 255 };
const Color INSTRUCTION_TEXT_COLOR = Color{ 255, 69, 0, 255 };
const float SUSPICIOUS_DELTA_TIME_THRESHOLD = 0.1f;
const float MAX_FPS = 60.0f;
const char* SAVE_FILE = "savegame.dat";


int main(void) {
    srand(static_cast<unsigned int>(time(0)));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple game loop");
    InitAudioDevice();

    Texture2D background = TextureManager::LoadTextureWithFallback("resources/b4.png", SCREEN_WIDTH, SCREEN_HEIGHT);
    Sound bounceSound = LoadSound("resources/audiomass-output.mp3");

    Player player(Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f }, "resources/p1.png", { 180, 180 }, bounceSound);
    ResourceManager resourceManager;
    resourceManager.Register(&player);

    bool isPaused = false;
    SetTargetFPS(MAX_FPS);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        float deltaTime = GetFrameTime();

        if (!isPaused && IsWindowFocused() && !(deltaTime > SUSPICIOUS_DELTA_TIME_THRESHOLD)) {
            player.Update(deltaTime);
            player.ConstrainToBounds(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        if (IsKeyPressed(KEY_ZERO)) resourceManager.SaveAll(SAVE_FILE);
        if (IsKeyPressed(KEY_ONE)) resourceManager.LoadAll(SAVE_FILE);

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        DrawTexture(background, 0, 0, WHITE);

        if (isPaused) {
            DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 20, PAUSED_TEXT_COLOR);
        }
        else {
            DrawText("Use WASD to control speed, P to pause.", 10, 10, 20, INSTRUCTION_TEXT_COLOR);
            DrawText("Press 0 to Save, 1 to Load.", 10, 30, 20, INSTRUCTION_TEXT_COLOR);
            player.Draw();
        }

        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();
    return 0;
}
