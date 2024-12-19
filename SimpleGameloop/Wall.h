#pragma once

#include "raylib.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include <cmath>

class Wall : public Sprite {
private:
    std::string texturePath;
    std::string bounceSoundPath;
    Texture2D texture;
    Sound bounceSound;
    ResourceManager& resourceManager;

public:
    Wall(
        ResourceManager& resourceManager,
        Vector2 initialPosition,
        Vector2 size,
        ShapeType shape = Circular,
        const std::string& texturePath = "resources/background.png",
        const std::string& bounceSoundPath = "resources/bounce.mp3",
        bool collidable = true
    );

    void OnCollision() const override;
    void Draw(int global_x, int global_y) const override;
    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;
};
