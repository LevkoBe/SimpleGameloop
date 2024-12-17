#pragma once
#include "Saveable.h"
#include "raylib.h"

class Sprite : public Saveable {
protected:
    Vector2 position;
    float rotation;
    Vector2 velocity;

public:
    Sprite(Vector2 initialPosition = { 0, 0 }, float initialRotation = 0.0f, Vector2 initialVelocity = { 0, 0 })
        : position(initialPosition), rotation(initialRotation), velocity(initialVelocity) {}

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() const = 0;

    void Save(std::ofstream& file) const override {
        file.write((char*)&position, sizeof(position));
        file.write((char*)&rotation, sizeof(rotation));
        file.write((char*)&velocity, sizeof(velocity));
    }

    void Load(std::ifstream& file) override {
        file.read((char*)&position, sizeof(position));
        file.read((char*)&rotation, sizeof(rotation));
        file.read((char*)&velocity, sizeof(velocity));
    }
};
