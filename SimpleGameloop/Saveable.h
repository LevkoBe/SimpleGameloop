#pragma once
#include <fstream>

struct Saveable {
    virtual void Save(std::ofstream& file) const = 0;
    virtual void Load(std::ifstream& file) = 0;
    virtual ~Saveable() = default;
};