#include "GameState.h"
#include <stdexcept>

GameState::GameState(ResourceManager& resourceManager) : resourceManager(resourceManager) {}

void GameState::Register(std::unique_ptr<Sprite> sprite) {
    sprites.push_back(std::move(sprite));
}

void GameState::Update(float deltaTime, int screenWidth, int screenHeight) {
    for (const auto& sprite : sprites) sprite->Update(deltaTime, screenWidth, screenHeight);
}

void GameState::Draw() const {
    for (const auto& sprite : sprites) sprite->Draw();
}

void GameState::Save(std::ofstream& file) const {
    size_t count = sprites.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& sprite : sprites) {
        if (dynamic_cast<Player*>(sprite.get())) {
            SpriteType type = SpriteType::Player;
            file.write(reinterpret_cast<const char*>(&type), sizeof(type));
        }
        else if (dynamic_cast<Background*>(sprite.get())) {
            SpriteType type = SpriteType::Background;
            file.write(reinterpret_cast<const char*>(&type), sizeof(type));
        }
        else throw std::runtime_error("Unknown sprite type during saving");

        sprite->Save(file);
    }
}

void GameState::Load(std::ifstream& file) {
    sprites.clear();

    size_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        SpriteType type;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));

        std::unique_ptr<Sprite> sprite;

        switch (type) {
        case SpriteType::Player:
            sprite = std::make_unique<Player>(
                Vector2{ 0, 0 }, "", Vector2{ 0, 0 }, "", resourceManager);
            break;
        case SpriteType::Background:
            sprite = std::make_unique<Background>("", resourceManager);
            break;
        default:
            throw std::runtime_error("Unknown sprite type during loading");
        }

        if (sprite) {
            sprite->Load(file);
            sprites.push_back(std::move(sprite));
        }
    }
}
