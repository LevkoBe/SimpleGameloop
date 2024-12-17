#pragma once
#include "Sprite.h"
#include "raylib.h"
#include "ResourceManager.h"

class Background : public Sprite {
private:
    std::string texturePath;
    Texture2D texture;
    ResourceManager& resourceManager;

public:
    Background(const std::string& texturePath, ResourceManager& resourceManager)
        : texturePath(texturePath), resourceManager(resourceManager) {
        texture = resourceManager.GetTexture(texturePath);
    }

    void Update(float deltaTime) override {
        (void)deltaTime;
    }

    void Draw() const override {
        DrawTexture(texture, 0, 0, WHITE);
    }

    void Save(std::ofstream& file) const override {
        Sprite::Save(file);
        resourceManager.SaveResourceKey(file, texturePath);
    }

    void Load(std::ifstream& file) override {
        Sprite::Load(file);
        texturePath = resourceManager.LoadResourceKey(file);
        texture = resourceManager.GetTexture(texturePath);
    }
};
