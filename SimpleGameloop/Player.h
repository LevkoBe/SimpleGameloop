#pragma once
#include "Sprite.h"
#include "raylib.h"
#include "ResourceManager.h"

const float ACCELERATION = 2000.0f;
const float ROTATION_OFFSET = 20.0f;

class Player : public Sprite {
private:
    std::string texturePath;
    Vector2 size;
    std::string bounceSoundPath;
    Texture2D texture;
    Sound bounceSound;

    ResourceManager& resourceManager;

public:
    Player(Vector2 initialPosition, const std::string& texturePath, Vector2 size, const std::string& bounceSoundPath, ResourceManager& resourceManager)
        : Sprite(initialPosition), texturePath(texturePath), size(size), bounceSoundPath(bounceSoundPath), resourceManager(resourceManager) {
        texture = resourceManager.GetTexture(texturePath, size.x, size.y);
        bounceSound = resourceManager.GetSound(bounceSoundPath);
    }

    void Update(float deltaTime) override {
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

    void Draw() const override {
        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destination = { position.x, position.y, size.x, size.y };
        Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

        DrawTexturePro(texture, source, destination, origin, rotation, WHITE);
    }

    void Save(std::ofstream& file) const override {
        Sprite::Save(file);
        file.write((char*)&size, sizeof(size));
        resourceManager.SaveResourceKey(file, texturePath);
        resourceManager.SaveResourceKey(file, bounceSoundPath);
    }

    void Load(std::ifstream& file) override {
        Sprite::Load(file);
        file.read((char*)&size, sizeof(size));
        texturePath = resourceManager.LoadResourceKey(file);
        bounceSoundPath = resourceManager.LoadResourceKey(file);

        texture = resourceManager.GetTexture(texturePath, size.x, size.y);
        bounceSound = resourceManager.GetSound(bounceSoundPath);
    }
};
