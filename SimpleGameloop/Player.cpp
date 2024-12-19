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
    if (IsKeyDown(KEY_W)) velocity.y -= ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_S)) velocity.y += ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_A)) velocity.x -= ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_D)) velocity.x += ACCELERATION * deltaTime;

    Vector2 mousePosition = GetMousePosition();
    rotation = atan2f(mousePosition.y - position.y, mousePosition.x - position.x) * RAD2DEG + ROTATION_OFFSET;
}

void Player::OnCollision() const {
    PlaySound(bounceSound);
}

void Player::Draw(int global_x, int global_y) const {
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle destination = { global_x, global_y, size.x, size.y };
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
