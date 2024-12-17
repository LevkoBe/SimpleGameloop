#pragma once
#include "Sprite.h"
#include "raylib.h"
#include "ResourceManager.h"

class Background : public Sprite {
private:
    std::string texturePath;
    Texture2D texture;
    ResourceManager& resourceManager;

    Vector2 position;
    float scrollSpeed;

public:
    Background(const std::string& texturePath, ResourceManager& resourceManager, float scrollSpeed = 100.0f)
        : texturePath(texturePath), resourceManager(resourceManager), scrollSpeed(scrollSpeed), position({ 0, 0 }) {
        texture = resourceManager.GetTexture(texturePath);
    }

    void Update(float deltaTime, int screenWidth, int screenHeight) override {
        Vector2 scrollDelta = GetMouseWheelMoveV();

        position.x += scrollDelta.x * scrollSpeed;
        position.y += scrollDelta.y * scrollSpeed;

        if (position.x <= -texture.width) position.x += texture.width;
        if (position.x >= screenWidth) position.x -= texture.width;
        if (position.y <= -texture.height) position.y += texture.height;
        if (position.y >= screenHeight) position.y -= texture.height;
    }

    void Draw() const override {
        for (int x = static_cast<int>(position.x); x < GetScreenWidth(); x += texture.width) {
            for (int y = 0; y < GetScreenHeight(); y += texture.height) {
                DrawTexture(texture, x, y, WHITE);
            }
        }
    }

    void Save(std::ofstream& file) const override {
        Sprite::Save(file);

        resourceManager.SaveResourceKey(file, texturePath);
        file.write(reinterpret_cast<const char*>(&scrollSpeed), sizeof(scrollSpeed));
        file.write(reinterpret_cast<const char*>(&position), sizeof(position));
    }

    void Load(std::ifstream& file) override {
        Sprite::Load(file);

        texturePath = resourceManager.LoadResourceKey(file);
        texture = resourceManager.GetTexture(texturePath);

        file.read(reinterpret_cast<char*>(&scrollSpeed), sizeof(scrollSpeed));
        file.read(reinterpret_cast<char*>(&position), sizeof(position));
    }
};
