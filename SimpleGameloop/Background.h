#pragma once
#include "Sprite.h"
#include "raylib.h"
#include "ResourceManager.h"

class Background : public Sprite {
private:
    std::string texturePath;
    Texture2D texture;
    ResourceManager& resourceManager;

    float scrollSpeed;

public:
    Background(ResourceManager& resourceManager, const std::string& texturePath = "resources/background2.png", float scrollSpeed = 100.0f);

    void Update(float deltaTime, int screenWidth, int screenHeight) override;
    void Draw(int global_x, int global_y) const override;
    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
