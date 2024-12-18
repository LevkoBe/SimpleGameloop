#pragma once
#include "Sprite.h"
#include "ResourceManager.h"

const float ACCELERATION = 2000.0f;
const float ROTATION_OFFSET = 20.0f;

class Player : public Sprite {
private:
    std::string texturePath;
    std::string bounceSoundPath;
    Texture2D texture;
    Sound bounceSound;

    ResourceManager& resourceManager;

public:
    Player(Vector2 initialPosition, const std::string& texturePath, Vector2 size, const std::string& bounceSoundPath, ResourceManager& resourceManager);

    void Update(float deltaTime, int screenWidth, int screenHeight) override;
    void Draw() const override;

    void Save(std::ofstream& file) const override;
    void Load(std::ifstream& file) override;

private:
    void Move(float deltaTime);
    void ConstrainToBounds(int screenWidth, int screenHeight);
};
