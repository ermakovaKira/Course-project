#ifndef INVENTORY_H
#define INVENTORY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <map>

// Выносим структуры характеристик и инвентаря в свой собственный изолированный файл
struct PlayerStats {
    float health = 100.f;
    float satiety = 100.f;
};

class Inventory {
public:
    std::map<std::string, int> items;
    std::map<std::string, sf::Texture> textures;

    // Ключевое слово inline позволяет коду жить в заголовочном файле без создания .cpp
    inline void loadItemTexture(std::string name, std::string path) {
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            tex.setSmooth(false);
            textures[name] = tex;
        }
    }

    inline void addItem(std::string name, int count) {
        items[name] += count;
    }
};

#endif
