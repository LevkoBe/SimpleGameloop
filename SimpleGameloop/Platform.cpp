#include "Platform.h"

Platform::Platform(
    ResourceManager& resourceManager,
    Vector2 initialPosition,
    Vector2 size,
    Vector2 expectedVelocity,
    ShapeType shape,
    const std::string& texturePath,
    const std::string& bounceSoundPath,
    bool collidable
) : Sprite(initialPosition, size, 0.0, expectedVelocity, shape, collidable),
expectedVelocity(expectedVelocity),
texturePath(texturePath),
bounceSoundPath(bounceSoundPath),
resourceManager(resourceManager)
{
    texture = resourceManager.GetTexture(texturePath, size.x, size.y);
    bounceSound = resourceManager.GetSound(bounceSoundPath);
}

void Platform::Update(float deltaTime, int screenWidth, int screenHeight) {
    float dotProduct = velocity.x * expectedVelocity.x + velocity.y * expectedVelocity.y;

    if (dotProduct >= 0.0f) velocity = expectedVelocity;
    else velocity = { -expectedVelocity.x, -expectedVelocity.y };
}

void Platform::OnCollision() const {
    PlaySound(bounceSound);
}

void Platform::Draw(int global_x, int global_y) const {
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle destination = { global_x, global_y, size.x, size.y };
    Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

    DrawTexturePro(texture, source, destination, origin, rotation, WHITE);
}

void Platform::Save(std::ofstream& file) const {
    Sprite::Save(file);
    file.write((char*)&expectedVelocity, sizeof(expectedVelocity));
    resourceManager.SaveResourceKey(file, texturePath);
    resourceManager.SaveResourceKey(file, bounceSoundPath);
}

void Platform::Load(std::ifstream& file) {
    Sprite::Load(file);
    file.read((char*)&expectedVelocity, sizeof(expectedVelocity));
    texturePath = resourceManager.LoadResourceKey(file);
    bounceSoundPath = resourceManager.LoadResourceKey(file);

    texture = resourceManager.GetTexture(texturePath, size.x, size.y);
    bounceSound = resourceManager.GetSound(bounceSoundPath);
}
