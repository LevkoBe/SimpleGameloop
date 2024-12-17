#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <filesystem>

class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> sounds;
    Sound defaultSound = LoadSoundFromWave({0});

public:
    Texture2D GetTexture(const std::string& path, int width = 100, int height = 100) {
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

    Sound GetSound(const std::string& path) {
        if (sounds.find(path) == sounds.end()) {
            if (std::filesystem::exists(path)) sounds[path] = LoadSound(path.c_str());
            else sounds[path] = defaultSound;
        }
        return sounds[path];
    }

    void SaveResourceKey(std::ofstream& file, const std::string& path) {
        size_t length = path.length();
        file.write((char*)&length, sizeof(length));
        file.write(path.c_str(), length);
    }

    std::string LoadResourceKey(std::ifstream& file) {
        size_t length;
        file.read((char*)&length, sizeof(length));
        char* buffer = new char[length + 1];
        file.read(buffer, length);
        buffer[length] = '\0';
        std::string path(buffer);
        delete[] buffer;
        return path;
    }

    void UnloadAll() {
        for (auto& [key, texture] : textures) UnloadTexture(texture);
        for (auto& [key, sound] : sounds) UnloadSound(sound);
        textures.clear();
        sounds.clear();
    }

    ~ResourceManager() {
        UnloadAll();
    }
};
