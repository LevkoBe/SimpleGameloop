#include "Background.h"

constexpr float B_ACCELERATION = 400.0f;

Background::Background(ResourceManager& resourceManager, const std::string& texturePath, float scrollSpeed)
    : Sprite({ 0, 0 }, { 0, 0 }, 0.0, { 0, 0 }, Rectangular, false), texturePath(texturePath), resourceManager(resourceManager), scrollSpeed(scrollSpeed) {
    texture = resourceManager.GetTexture(texturePath);
    size = Vector2{ static_cast<float>(texture.width), static_cast<float>(texture.height) };
}

void Background::Update(float deltaTime, int screenWidth, int screenHeight) {
    if (IsKeyDown(KEY_W)) velocity.y -= B_ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_S)) velocity.y += B_ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_A)) velocity.x -= B_ACCELERATION * deltaTime;
    if (IsKeyDown(KEY_D)) velocity.x += B_ACCELERATION * deltaTime;

    Vector2 scrollDelta = GetMouseWheelMoveV();

    position.x += scrollDelta.x * scrollSpeed;
    position.y += scrollDelta.y * scrollSpeed;

    if (position.x <= -texture.width) position.x += texture.width;
    if (position.x > 0) position.x -= texture.width;
    if (position.y <= -texture.height) position.y += texture.height;
    if (position.y > 0) position.y -= texture.height;
}

void Background::Draw(int global_x, int global_y) const {
    for (int x = static_cast<int>(global_x); x < GetScreenWidth(); x += texture.width)
        for (int y = static_cast<int>(global_y); y < GetScreenHeight(); y += texture.height)
            DrawTexture(texture, x, y, WHITE);
}

void Background::Save(std::ofstream& file) const {
    Sprite::Save(file);

    resourceManager.SaveResourceKey(file, texturePath);
    file.write(reinterpret_cast<const char*>(&scrollSpeed), sizeof(scrollSpeed));
}

void Background::Load(std::ifstream& file) {
    Sprite::Load(file);

    texturePath = resourceManager.LoadResourceKey(file);
    texture = resourceManager.GetTexture(texturePath);

    file.read(reinterpret_cast<char*>(&scrollSpeed), sizeof(scrollSpeed));
}
