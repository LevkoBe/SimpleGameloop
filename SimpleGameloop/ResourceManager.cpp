#include "ResourceManager.h"

ResourceManager::ResourceManager() : defaultSound(LoadSoundFromWave({ 0 })) {}

Texture2D ResourceManager::GetTexture(const std::string& path, int width, int height) {
    if (textures.find(path) == textures.end()) {
        Texture2D texture = LoadTexture(path.c_str());
        if (texture.id == 0) {
            Image img = GenImageColor(width, height, Color{
                static_cast<unsigned char>(rand() % 256),
                static_cast<unsigned char>(rand() % 256),
                static_cast<unsigned char>(rand() % 256),
                255
                });
            texture = LoadTextureFromImage(img);
            UnloadImage(img);
        }
        textures[path] = texture;
    }
    return textures[path];
}

Sound ResourceManager::GetSound(const std::string& path) {
    if (sounds.find(path) == sounds.end()) {
        if (std::filesystem::exists(path)) sounds[path] = LoadSound(path.c_str());
        else sounds[path] = defaultSound;
    }
    return sounds[path];
}

void ResourceManager::SaveResourceKey(std::ofstream& file, const std::string& path) {
    size_t length = path.length();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(path.c_str(), length);
}

std::string ResourceManager::LoadResourceKey(std::ifstream& file) {
    size_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    char* buffer = new char[length + 1];
    file.read(buffer, length);
    buffer[length] = '\0';
    std::string path(buffer);
    delete[] buffer;
    return path;
}

void ResourceManager::UnloadAll() {
    for (auto& [key, texture] : textures) UnloadTexture(texture);
    for (auto& [key, sound] : sounds) UnloadSound(sound);
    textures.clear();
    sounds.clear();
}

ResourceManager::~ResourceManager() {
    UnloadAll();
}
