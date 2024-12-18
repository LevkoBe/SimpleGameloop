#include "Background.h"

Background::Background(const std::string& texturePath, ResourceManager& resourceManager, float scrollSpeed)
    : texturePath(texturePath), resourceManager(resourceManager), scrollSpeed(scrollSpeed) {
    texture = resourceManager.GetTexture(texturePath);
    size = Vector2{ static_cast<float>(texture.width), static_cast<float>(texture.height) };
}

void Background::Update(float deltaTime, int screenWidth, int screenHeight) {
    Vector2 scrollDelta = GetMouseWheelMoveV();

    position.x += scrollDelta.x * scrollSpeed;
    position.y += scrollDelta.y * scrollSpeed;

    if (position.x <= -texture.width) position.x += texture.width;
    if (position.x > 0) position.x -= texture.width;
    if (position.y <= -texture.height) position.y += texture.height;
    if (position.y > 0) position.y -= texture.height;
}

void Background::Draw() const {
    for (int x = static_cast<int>(position.x); x < GetScreenWidth(); x += texture.width)
        for (int y = static_cast<int>(position.y); y < GetScreenHeight(); y += texture.height)
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
