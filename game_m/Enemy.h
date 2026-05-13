#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"
#include "HealthBar.h"

// Простая структура или класс HealthBar, если он у тебя вынесен отдельно
// Если класс HealthBar объявлен в другом файле, убедись, что он подключен
class Enemy {
public:
    sf::Clock animClock;
    sf::Sprite sprite;
    sf::Texture textureIdle;
    sf::Texture textureAttack;

    int w, h;            // Размеры кадра анимации
    float currentFrame;  // Текущий кадр
    float health;        // Здоровье зомби

    // Объявляем полоску здоровья для зомби
    HealthBar hpBar;

    // Объявляем полноценный конструктор класса
    Enemy(std::string pathIdle, std::string pathAttack, sf::Vector2f pos, int width, int height);

    void update(float time, Player& hero);
    void draw(sf::RenderWindow& window);
};

#endif
