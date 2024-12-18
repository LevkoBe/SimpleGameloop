#include "Sprite.h"

Sprite::Sprite(Vector2 initialPosition, float initialRotation, Vector2 initialVelocity)
    : position(initialPosition), rotation(initialRotation), velocity(initialVelocity) {}

void Sprite::Update(float deltaTime, int screenWidth, int screenHeight) {
    // Default Update: Do nothing
}

void Sprite::Draw() const {
    // Default Draw: Represent a blank sprite
}

void Sprite::Save(std::ofstream& file) const {
    file.write((char*)&position, sizeof(position));
    file.write((char*)&rotation, sizeof(rotation));
    file.write((char*)&velocity, sizeof(velocity));
}

void Sprite::Load(std::ifstream& file) {
    file.read((char*)&position, sizeof(position));
    file.read((char*)&rotation, sizeof(rotation));
    file.read((char*)&velocity, sizeof(velocity));
}
