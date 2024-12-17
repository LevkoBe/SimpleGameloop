#pragma once
#include <vector>
#include <fstream>
#include "Saveable.h"

class GameState : public Saveable {
private:
    std::vector<Saveable*> saveables;

public:
    void Register(Saveable* saveable) { saveables.push_back(saveable); }

    void Save(std::ofstream& file) const override {
        size_t count = saveables.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (const auto* saveable : saveables) saveable->Save(file);
    }

    void Load(std::ifstream& file) override {
        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        for (size_t i = 0; i < count; ++i) for (auto* saveable : saveables) saveable->Load(file);
    }
};
