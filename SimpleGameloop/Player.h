#pragma once
#include "Saveable.h"
#include <raylib.h>
#include "TextureManager.h"

const float ACCELERATION = 2000.0f;
const float ROTATION_OFFSET = 20.0f;


class Player : public Saveable {
public:
    Vector2 position;
    float rotation;
    Vector2 velocity;
    Texture2D texture;
    std::string texturePath;
    Vector2 size;
    Sound bounceSound;

    Player(Vector2 initialPosition, const char* texturePath, Vector2 size, Sound bounceSound)
        : position(initialPosition), rotation(0.0f), velocity{ 0.0f, 0.0f }, texturePath(texturePath), size(size), bounceSound(bounceSound) {
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

        if (position.x - halfWidth < 0) { position.x = halfWidth; velocity.x = -velocity.x; PlaySound(bounceSound); }
        if (position.x + halfWidth > screenWidth) { position.x = screenWidth - halfWidth; velocity.x = -velocity.x; PlaySound(bounceSound); }
        if (position.y - halfHeight < 0) { position.y = halfHeight; velocity.y = -velocity.y; PlaySound(bounceSound); }
        if (position.y + halfHeight > screenHeight) { position.y = screenHeight - halfHeight; velocity.y = -velocity.y; PlaySound(bounceSound); }
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
