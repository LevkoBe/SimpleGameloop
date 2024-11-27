#include "raylib.h"
#include <math.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const Color BACKGROUND_COLOR = Color{ 255, 239, 213, 255 };
const Color PAUSED_TEXT_COLOR = Color{ 255, 165, 0, 255 };
const Color INSTRUCTION_TEXT_COLOR = Color{ 255, 69, 0, 255 };
const float ROTATION_OFFSET = 20.0f;

const float ACCELERATION = 2000.0f;

const float SUSPICIOUS_DELTA_TIME_THRESHOLD = 0.1f;
const float MAX_FPS = 60.0f;

class Player {
public:
    Vector2 position;
    float rotation;
    Vector2 velocity;
    Texture2D texture;

    Player(Vector2 initialPosition, const char* texturePath)
        : position(initialPosition), rotation(0.0f), velocity{ 0.0f, 0.0f } {
        texture = LoadTexture(texturePath);
    }

    ~Player() {
        UnloadTexture(texture);
    }

    void Update(float deltaTime) {
        if (IsKeyDown(KEY_W)) velocity.y -= ACCELERATION * deltaTime;
        if (IsKeyDown(KEY_S)) velocity.y += ACCELERATION * deltaTime;
        if (IsKeyDown(KEY_A)) velocity.x -= ACCELERATION * deltaTime;
        if (IsKeyDown(KEY_D)) velocity.x += ACCELERATION * deltaTime;

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        Vector2 mousePosition = GetMousePosition();
        rotation = atan2f(mousePosition.y - position.y, mousePosition.x - position.x) * RAD2DEG + ROTATION_OFFSET;
    }

    void ConstrainToBounds(int screenWidth, int screenHeight) {
        float halfWidth = texture.width / 2.0f;
        float halfHeight = texture.height / 2.0f;

        if (position.x - halfWidth < 0) { position.x = halfWidth; velocity.x = 0; }
        if (position.x + halfWidth > screenWidth) { position.x = screenWidth - halfWidth; velocity.x = 0; }
        if (position.y - halfHeight < 0) { position.y = halfHeight; velocity.y = 0; }
        if (position.y + halfHeight > screenHeight) { position.y = screenHeight - halfHeight; velocity.y = 0; }
    }

    void Draw() const {
        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destination = { position.x, position.y, (float)texture.width, (float)texture.height };
        Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };

        DrawTexturePro(texture, source, destination, origin, rotation, WHITE);
    }
};

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple game loop");

    Player player(Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f }, "resources\\p1.png");

    bool isPaused = false;

    SetTargetFPS(MAX_FPS);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) isPaused = !isPaused;

        float deltaTime = GetFrameTime();

        if (!isPaused && IsWindowFocused() && !(deltaTime > SUSPICIOUS_DELTA_TIME_THRESHOLD)) {
            player.Update(deltaTime);
            player.ConstrainToBounds(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        BeginDrawing();

        ClearBackground(BACKGROUND_COLOR);

        if (isPaused) {
            DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 20, PAUSED_TEXT_COLOR);
        }
        else {
            DrawText("Use WASD to control speed, P to pause.", 10, 10, 20, INSTRUCTION_TEXT_COLOR);
            player.Draw();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
