#include "Sprite.h"

Sprite::Sprite(Vector2 initialPosition, Vector2 size, float initialRotation, Vector2 initialVelocity, ShapeType shape, bool collidable)
    : position(initialPosition), rotation(initialRotation), velocity(initialVelocity), collidable(collidable), shape(shape), size(size) {}

void Sprite::Update(float deltaTime, int screenWidth, int screenHeight) {
    // Default Update: Do nothing
}
void Sprite::OnCollision() const {
    // Default Reaction: Absent
}

void Sprite::Draw(int global_x, int global_y) const {
    // Default Draw: Represent a blank sprite
}

void Sprite::Save(std::ofstream& file) const {
    file.write((char*)&position, sizeof(position));
    file.write((char*)&rotation, sizeof(rotation));
    file.write((char*)&velocity, sizeof(velocity));
    file.write((char*)&size, sizeof(size));
    file.write((char*)&shape, sizeof(shape));
    file.write((char*)&collidable, sizeof(collidable));
}

void Sprite::Load(std::ifstream& file) {
    file.read((char*)&position, sizeof(position));
    file.read((char*)&rotation, sizeof(rotation));
    file.read((char*)&velocity, sizeof(velocity));
    file.read((char*)&size, sizeof(size));
    file.read((char*)&shape, sizeof(shape));
    file.read((char*)&collidable, sizeof(collidable));
}

