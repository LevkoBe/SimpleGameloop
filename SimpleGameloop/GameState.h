#pragma once
#include <vector>
#include <fstream>
#include "Sprite.h"

class GameState : public Saveable {
private:
    std::vector<Sprite*> sprites;

public:
    void Register(Sprite* sprite) { sprites.push_back(sprite); }

    void Update(float deltaTime) {
        for (auto* sprite : sprites) sprite->Update(deltaTime);
    }

    void Draw() const {
        for (const auto* sprite : sprites) sprite->Draw();
    }

    void Save(std::ofstream& file) const override {
        size_t count = sprites.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (const auto* sprite : sprites) sprite->Save(file);
    }

    void Load(std::ifstream& file) override {
        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        for (size_t i = 0; i < count; ++i)
            for (auto* sprite : sprites) sprite->Load(file);
    }
};
