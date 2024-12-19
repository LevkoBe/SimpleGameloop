#include "Player.h"

Player::Player(
    ResourceManager& resourceManager,
    Vector2 initialPosition,
    Vector2 size,
    ShapeType shape,
    const std::string& texturePath,
    const std::string& bounceSoundPath,
    bool collidable
) : Sprite(initialPosition, size, 0.0, { 0, 0 }, shape, collidable),
texturePath(texturePath),
bounceSoundPath(bounceSoundPath),
resourceManager(resourceManager)
{
    texture = resourceManager.GetTexture(texturePath, size.x, size.y);
    bounceSound = resourceManager.GetSound(bounceSoundPath);
}

void Player::Update(float deltaTime, int screenWidth, int screenHeight) {
    Move(deltaTime);
    ConstrainToBounds(screenWidth, screenHeight);
}

void Player::Move(float deltaTime) {
    if (IsKeyDown(KEY_W)) velocity.y -= ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_S)) velocity.y += ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_A)) velocity.x -= ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_D)) velocity.x += ACCELERATION * deltaTime;

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    Vector2 mousePosition = GetMousePosition();
    rotation = atan2f(mousePosition.y - position.y, mousePosition.x - position.x) * RAD2DEG + ROTATION_OFFSET;
}

void Player::ConstrainToBounds(int screenWidth, int screenHeight) {
    float halfWidth = size.x / 2.0f;
    float halfHeight = size.y / 2.0f;

    if (position.x - halfWidth < 0) {
        position.x = halfWidth;
        velocity.x = -velocity.x;
        PlaySound(bounceSound);
    }
    if (position.x + halfWidth > screenWidth) {
        position.x = screenWidth - halfWidth;
        velocity.x = -velocity.x;
        PlaySound(bounceSound);
    }
    if (position.y - halfHeight < 0) {
        position.y = halfHeight;
        velocity.y = -velocity.y;
        PlaySound(bounceSound);
    }
    if (position.y + halfHeight > screenHeight) {
        position.y = screenHeight - halfHeight;
        velocity.y = -velocity.y;
        PlaySound(bounceSound);
    }
}

void Player::Draw() const {
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle destination = { position.x, position.y, size.x, size.y };
    Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

    DrawTexturePro(texture, source, destination, origin, rotation, WHITE);
}

void Player::Save(std::ofstream& file) const {
    Sprite::Save(file);
    resourceManager.SaveResourceKey(file, texturePath);
    resourceManager.SaveResourceKey(file, bounceSoundPath);
}

void Player::Load(std::ifstream& file) {
    Sprite::Load(file);
    texturePath = resourceManager.LoadResourceKey(file);
    bounceSoundPath = resourceManager.LoadResourceKey(file);

    texture = resourceManager.GetTexture(texturePath, size.x, size.y);
    bounceSound = resourceManager.GetSound(bounceSoundPath);
}
