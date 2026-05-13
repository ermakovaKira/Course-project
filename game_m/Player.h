#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <map>

// Утилитарная структура для инвентаря игрока
struct Inventory {
    std::map<std::string, int> items;
    std::map<std::string, sf::Texture> textures;

    void loadItemTexture(std::string name, std::string path) {
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            tex.setSmooth(false);
            textures[name] = tex;
        }
    }

    void addItem(std::string name, int count) {
        items[name] += count;
    }
};

// Структура для характеристик
struct PlayerStats {
    float health = 100.f;
    float satiety = 100.f; // Сытость (для сэндвичей)
};

class Player {
public:
    sf::Sprite sprite;
    sf::Texture textureIdle;
    sf::Texture textureShoot;

    int w, h; // Размеры кадра
    float currentFrame;
    float speed;
    bool isShooting;
    bool faceRight;
    sf::Clock animationClock;

    Inventory inventory;
    PlayerStats stats;
    class ItemDatabase* db; // Указатель на базу данных предметов

    Player(std::string pathIdle, std::string pathShoot, int width, int height);

    void handleInput(sf::Event& event);
    void update(float time);
    void draw(sf::RenderWindow& window);
    void showMessage(std::wstring message, sf::Color color);
};

#endif
