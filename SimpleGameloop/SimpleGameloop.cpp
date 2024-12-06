#include "raylib.h"
#include <cmath>
#include <fstream>
#include <random>
#include <vector>
#include <iostream>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const Color BACKGROUND_COLOR = Color{ 255, 239, 213, 255 };
const Color PAUSED_TEXT_COLOR = Color{ 255, 165, 0, 255 };
const Color INSTRUCTION_TEXT_COLOR = Color{ 255, 69, 0, 255 };
const float ROTATION_OFFSET = 20.0f;
const float ACCELERATION = 2000.0f;
const float SUSPICIOUS_DELTA_TIME_THRESHOLD = 0.1f;
const float MAX_FPS = 60.0f;
const char* SAVE_FILE = "savegame.dat";

struct Saveable {
    virtual void Save(std::ofstream& file) const = 0;
    virtual void Load(std::ifstream& file) = 0;
    virtual ~Saveable() = default;
};

class TextureManager {
public:
    static Texture2D LoadTextureWithFallback(const char* path, int width = 0, int height = 0) {
        Texture2D texture = LoadTexture(path);
        if (texture.id == 0) {
            if (width == 0 || height == 0) {
                width = 100;
                height = 100;
            }
            Image img = GenImageColor(width, height, Color{
                static_cast<unsigned char>(rand() % 256),
                static_cast<unsigned char>(rand() % 256),
                static_cast<unsigned char>(rand() % 256),
                255
                });
            texture = LoadTextureFromImage(img);
            UnloadImage(img);
        }
        return texture;
    }
};


class Player : public Saveable {
public:
    Vector2 position;
    float rotation;
    Vector2 velocity;
    Texture2D texture;
    std::string texturePath;
    Vector2 size;

    Player(Vector2 initialPosition, const char* texturePath, Vector2 size)
        : position(initialPosition), rotation(0.0f), velocity{ 0.0f, 0.0f }, texturePath(texturePath), size(size) {
        texture = TextureManager::LoadTextureWithFallback(texturePath, size.x, size.y);
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
        float halfWidth = size.x / 2.0f;
        float halfHeight = size.y / 2.0f;

        if (position.x - halfWidth < 0) { position.x = halfWidth; velocity.x = -velocity.x; }
        if (position.x + halfWidth > screenWidth) { position.x = screenWidth - halfWidth; velocity.x = -velocity.x; }
        if (position.y - halfHeight < 0) { position.y = halfHeight; velocity.y = -velocity.y; }
        if (position.y + halfHeight > screenHeight) { position.y = screenHeight - halfHeight; velocity.y = -velocity.y; }
    }

    void Draw() const {
        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destination = { position.x, position.y, size.x, size.y };
        Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

        DrawTexturePro(texture, source, destination, origin, rotation, WHITE);
    }

    void Save(std::ofstream& file) const override {
        file.write((char*)&position, sizeof(position));
        file.write((char*)&rotation, sizeof(rotation));
        file.write((char*)&velocity, sizeof(velocity));
        size_t pathLength = texturePath.length();
        file.write((char*)&pathLength, sizeof(pathLength));
        file.write(texturePath.c_str(), pathLength);
        file.write((char*)&size, sizeof(size));
    }

    void Load(std::ifstream& file) override {
        file.read((char*)&position, sizeof(position));
        file.read((char*)&rotation, sizeof(rotation));
        file.read((char*)&velocity, sizeof(velocity));
        size_t pathLength;
        file.read((char*)&pathLength, sizeof(pathLength));
        char* buffer = new char[pathLength + 1];
        file.read(buffer, pathLength);
        buffer[pathLength] = '\0';
        texturePath = buffer;
        delete[] buffer;
        file.read((char*)&size, sizeof(size));
        UnloadTexture(texture);
        texture = TextureManager::LoadTextureWithFallback(texturePath.c_str(), size.x, size.y);
    }
};

class ResourceManager {
    std::vector<Saveable*> resources;

public:
    void Register(Saveable* resource) {
        resources.push_back(resource);
    }

    void SaveAll(const char* filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            for (auto* resource : resources) {
                resource->Save(file);
            }
            file.close();
        }
    }

    void LoadAll(const char* filename) {
        try
        {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                for (auto* resource : resources) {
                    resource->Load(file);
                }
                file.close();
            }
        }
        catch (const std::exception&)
        {
            std::cout << "The file could not be read." << std::endl;
        }
    }
};

int main(void) {
    srand(static_cast<unsigned int>(time(0)));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple game loop");

    Texture2D background = TextureManager::LoadTextureWithFallback("resources/b4.png", SCREEN_WIDTH, SCREEN_HEIGHT);
    Player player(Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f }, "resources/p1.png", { 180, 180 });
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
