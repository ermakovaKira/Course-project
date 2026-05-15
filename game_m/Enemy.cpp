#include "Enemy.h"
#include <iostream>

Enemy::Enemy(std::string pathIdle, std::string pathAttack, sf::Vector2f pos, int width, int height)
    : hpBar(100.f, 10.f, sf::Color::Red)
{
    if (!textureIdle.loadFromFile(pathIdle)) {
        std::cout << "Error: " << pathIdle << " not found!" << std::endl;
    }
    textureAttack.loadFromFile(pathAttack);

    textureIdle.setSmooth(false);
    textureAttack.setSmooth(false);

    w = width;
    h = height;
    currentFrame = 0.f;
    health = 50.f;

    sprite.setTexture(textureIdle, true);
    sprite.setTextureRect(sf::IntRect(0, 0, w, h));
    sprite.setPosition(pos);

    float targetH = 185.0f;
    float currentScale = targetH / (float)h;
    sprite.setScale(currentScale, currentScale);
}

void Enemy::update(float time, Player& hero) {
    if (health <= 0) return;

    float enemyX = sprite.getPosition().x;
    float playerX = hero.sprite.getPosition().x;


    float distanceToPlayer = std::abs(enemyX - playerX);

    float zombieSpeed = 0.035f; 
    bool isMoving = false;


    if (distanceToPlayer > 60.f) {
        isMoving = true;
        if (enemyX < playerX) {
            sprite.move(zombieSpeed * time, 0);
        }
        else {
            sprite.move(-zombieSpeed * time, 0);
        }
    }

    if (isMoving) {
        sprite.setTexture(textureIdle);


        if (animClock.getElapsedTime().asMilliseconds() > 180) {
            currentFrame += 1.f;
            if (currentFrame >= 4.f) currentFrame = 0.f;
            animClock.restart();
        }

        int frameOffset = int(currentFrame) * w;
        if (enemyX < playerX) {
            sprite.setTextureRect(sf::IntRect(frameOffset, 0, w, h)); 
        }
        else {
            sprite.setTextureRect(sf::IntRect(frameOffset + w, 0, -w, h)); 
        }
    }

    else {
        sprite.setTexture(textureAttack);
        if (enemyX < playerX) sprite.setTextureRect(sf::IntRect(0, 0, w, h));
        else sprite.setTextureRect(sf::IntRect(w, 0, -w, h));
    }

}


void Enemy::draw(sf::RenderWindow& window) {
    if (health > 0) {
        window.draw(sprite); 
    }
}

