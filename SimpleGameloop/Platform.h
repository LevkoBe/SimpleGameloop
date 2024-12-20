#pragma once

#include "raylib.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include <cmath>

class Platform : public Sprite {
private:
    std::string texturePath;
    std::string bounceSoundPath;
    Texture2D texture;
    Sound bounceSound;
    Vector2 expectedVelocity;
    ResourceManager& resourceManager;

public:
    Platform(
        ResourceManager& resourceManager,
        Vector2 initialPosition,
        Vector2 size,
        Vector2 expectedVelocity,
        ShapeType shape = Rectangular,
        const std::string& texturePath = "resources/background.png",
        const std::string& bounceSoundPath = "resources/bounce.mp3",
        bool collidable = true
    );

    void Update(float deltaTime, int screenWidth, int screenHeight) override;
    void OnCollision() const override;
    void Draw(int global_x, int global_y) const override;
    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
