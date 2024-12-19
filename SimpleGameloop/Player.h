#pragma once

#include "raylib.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include <cmath>

constexpr float ACCELERATION = 2000.0f;
constexpr float ROTATION_OFFSET = 20.0f;

class Player : public Sprite {
private:
    std::string texturePath;
    std::string bounceSoundPath;
    Texture2D texture;
    Sound bounceSound;
    ResourceManager& resourceManager;

public:
    Player(
        ResourceManager& resourceManager,
        Vector2 initialPosition,
        Vector2 size,
        ShapeType shape = Circular,
        const std::string& texturePath = "resources/player.png",
        const std::string& bounceSoundPath = "resources/bounce.mp3",
        bool collidable = true
    );

    void Update(float deltaTime, int screenWidth, int screenHeight) override;
    void Draw() const override;
    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;

private:
    void Move(float deltaTime);
    void ConstrainToBounds(int screenWidth, int screenHeight);
};
