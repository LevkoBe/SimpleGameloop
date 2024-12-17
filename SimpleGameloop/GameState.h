#pragma once
#include <vector>
#include <fstream>
#include <memory>
#include "Sprite.h"

class GameState : public Saveable {
private:
    std::vector<std::unique_ptr<Sprite>> sprites;

public:
    // Register a sprite by moving a unique_ptr into the vector
    void Register(std::unique_ptr<Sprite> sprite) {
        sprites.push_back(std::move(sprite));
    }

    // Update all sprites
    void Update(float deltaTime) {
        for (const auto& sprite : sprites) {
            sprite->Update(deltaTime);
        }
    }

    // Draw all sprites
    void Draw() const {
        for (const auto& sprite : sprites) {
            sprite->Draw();
        }
    }

    // Save all sprites
    void Save(std::ofstream& file) const override {
        size_t count = sprites.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& sprite : sprites) {
            sprite->Save(file);
        }
    }

    // Load all sprites
    void Load(std::ifstream& file) override {
        // Clear existing sprites
        sprites.clear();

        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (size_t i = 0; i < count; ++i) {
            auto sprite = std::make_unique<Sprite>(); // Default Sprite instantiation
            sprite->Load(file);
            sprites.push_back(std::move(sprite));
        }
    }
};
