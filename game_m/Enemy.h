#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "HealthBar.h"

class Enemy {
public:
    enum State { WALKING, ATTACKING } state;

    sf::Sprite sprite;
    sf::Texture textureWalk;
    sf::Texture textureAttack;
    HealthBar hpBar;

    int w, h;
    float currentFrame;
    float speed;
    float health;
    float damage;
    float attackCooldown;
    float timer;

    Enemy(std::string pathWalk, std::string pathAttack, sf::Vector2f pos, int width, int height);
    void update(float time, Player& hero);
    void draw(sf::RenderWindow& window);
};

#endif
