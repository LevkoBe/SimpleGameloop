#include "raylib.h"
#include <math.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const Color BACKGROUND_COLOR = Color{ 255, 239, 213, 255 };
const Color PAUSED_TEXT_COLOR = Color{ 255, 165, 0, 255 };
const Color INSTRUCTION_TEXT_COLOR = Color{ 255, 69, 0, 255 };
const Color PLAYER_COLOR = Color{ 255, 140, 0, 255 };

const float PLAYER_SIZE = 30.0f;
const float ACCELERATION = 2000.0f;

const float SUSPICIOUS_DELTA_TIME_THRESHOLD = 0.1f;
const float MAX_FPS = 60.0f;

class Player {
public:
    Vector2 position;
    float rotation;
    Vector2 velocity;

    Player(Vector2 initialPosition) : position(initialPosition), rotation(0.0f), velocity{ 0.0f, 0.0f } {}

    void Update(float deltaTime) {
        if (IsKeyDown(KEY_W)) velocity.y -= ACCELERATION * deltaTime;
        if (IsKeyDown(KEY_S)) velocity.y += ACCELERATION * deltaTime;
        if (IsKeyDown(KEY_A)) velocity.x -= ACCELERATION * deltaTime;
        if (IsKeyDown(KEY_D)) velocity.x += ACCELERATION * deltaTime;

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        Vector2 mousePosition = GetMousePosition();
        rotation = atan2f(mousePosition.y - position.y, mousePosition.x - position.x) * RAD2DEG + 90.0f;
    }

    void ConstrainToBounds(int screenWidth, int screenHeight) {
        if (position.x - PLAYER_SIZE < 0) { position.x = PLAYER_SIZE; velocity.x = 0; }
        if (position.x + PLAYER_SIZE > screenWidth) { position.x = screenWidth - PLAYER_SIZE; velocity.x = 0; }
        if (position.y - PLAYER_SIZE < 0) { position.y = PLAYER_SIZE; velocity.y = 0; }
        if (position.y + PLAYER_SIZE > screenHeight) { position.y = screenHeight - PLAYER_SIZE; velocity.y = 0; }
    }

    void Draw() const {
        for (int i = 0; i < 10; i++) {
            float angle = (rotation + i * 36.0f) * DEG2RAD;
            float length = (i % 2 == 0) ? PLAYER_SIZE : PLAYER_SIZE / 2.0f;
            Vector2 vertex = {
                position.x + cosf(angle) * length,
                position.y + sinf(angle) * length
            };

            Vector2 nextVertex = {
                position.x + cosf((rotation + (i + 1) * 36.0f) * DEG2RAD) * ((i % 2 == 0) ? PLAYER_SIZE / 2.0f : PLAYER_SIZE),
                position.y + sinf((rotation + (i + 1) * 36.0f) * DEG2RAD) * ((i % 2 == 0) ? PLAYER_SIZE / 2.0f : PLAYER_SIZE)
            };

            DrawLineEx(vertex, nextVertex, 3.0f, PLAYER_COLOR);
        }
    }
};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple game loop");

    Player player(Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f });

    bool isPaused = false;

    SetTargetFPS(MAX_FPS);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        float deltaTime = GetFrameTime();

        if (!isPaused && IsWindowFocused() && !(deltaTime > SUSPICIOUS_DELTA_TIME_THRESHOLD))
        {
            player.Update(deltaTime);

            player.ConstrainToBounds(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        BeginDrawing();

        ClearBackground(BACKGROUND_COLOR);

        if (isPaused) DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 20, PAUSED_TEXT_COLOR);
        else
        {
            DrawText("Use WASD to control speed, P to pause.", 10, 10, 20, INSTRUCTION_TEXT_COLOR);

            player.Draw();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
