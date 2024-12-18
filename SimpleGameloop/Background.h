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
    Background(const std::string& texturePath, ResourceManager& resourceManager, float scrollSpeed = 100.0f);

    void Update(float deltaTime, int screenWidth, int screenHeight) override;
    void Draw() const override;
    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
