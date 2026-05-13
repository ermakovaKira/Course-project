#include "Player.h"
#include "ItemDatabase.h"
#include <iostream>

Player::Player(std::string pathIdle, std::string pathShoot, int width, int height) {
    textureIdle.loadFromFile(pathIdle);
    textureShoot.loadFromFile(pathShoot);

    // Бритвенная чёткость для HD-пикселей Евы
    textureIdle.setSmooth(false);
    textureShoot.setSmooth(false);

    w = width;
    h = height;

    currentFrame = 0.f;
    speed = 0.10f; // Слегка уменьшили физическую скорость, чтобы походка была реалистичной
    isShooting = false;
    faceRight = true;
    db = nullptr;

    sprite.setTexture(textureIdle);
    sprite.setTextureRect(sf::IntRect(0, 0, w - 1, h));

    float targetH = 180.0f;
    float currentScale = targetH / (float)h;
    sprite.setScale(currentScale, currentScale);
}

void Player::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1) {
        if (inventory.items["Medkit"] > 0) {
            inventory.items["Medkit"]--;
            float healAmount = 30.f;
            if (db != nullptr) {
                healAmount = db->data["Medkit"].effectValue;
            }
            stats.health = std::min(100.f, stats.health + healAmount);
            std::cout << "Ева использовала аптечку. HP: " << stats.health << std::endl;
        }
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        if (inventory.items["Ammo"] > 0) {
            inventory.items["Ammo"]--;
            isShooting = true;
            currentFrame = 0.f;
            sprite.setTexture(textureShoot);
            animationClock.restart();
            std::cout << "Выстрел! Патронов осталось: " << inventory.items["Ammo"] << std::endl;
        }
    }
}

void Player::update(float time) {
    if (!isShooting) {
        bool isMoving = false;
        sf::Vector2f position = sprite.getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            position.x -= speed * time;
            faceRight = false;
            isMoving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            position.x += speed * time;
            faceRight = true;
            isMoving = true;
        }

        if (position.x < 0.f) position.x = 0.f;
        if (position.x > 800.f - (w * sprite.getScale().x)) position.x = 800.f - (w * sprite.getScale().x);

        sprite.setPosition(position);

        // --- РАЗМЕРЕННЫЙ КИНЕМАТОГРАФИЧНЫЙ ТАЙМЕР ХОДЬБЫ ---
        if (isMoving) {
            sprite.setTexture(textureIdle);

            // Замедлили смену кадров до 160 мс — ноги будут переставляться плавно и неторопливо
            if (animationClock.getElapsedTime().asMilliseconds() > 160) {
                currentFrame += 1.f;
                if (currentFrame >= 4.f) currentFrame = 0.f;
                animationClock.restart();
            }

            int frameOffset = int(currentFrame) * w;

            if (faceRight) {
                sprite.setTextureRect(sf::IntRect(frameOffset, 0, w - 1, h));
            }
            else {
                sprite.setTextureRect(sf::IntRect(frameOffset + w - 1, 0, -(w - 1), h));
            }
        }
        else {
            // При остановке мгновенно и красиво возвращаем Еву в позу покоя
            sprite.setTexture(textureIdle);
            currentFrame = 0.f;
            if (faceRight) {
                sprite.setTextureRect(sf::IntRect(0, 0, w - 1, h));
            }
            else {
                sprite.setTextureRect(sf::IntRect(w - 1, 0, -(w - 1), h));
            }
        }
    }
    // Скорость вспышки выстрела оставляем четкой и быстрой (70 миллисекунд)
    else {
        sprite.setTexture(textureShoot);

        if (animationClock.getElapsedTime().asMilliseconds() > 70) {
            currentFrame += 1.f;
            if (currentFrame >= 4.f) {
                isShooting = false;
                currentFrame = 0.f;
                sprite.setTexture(textureIdle);
            }
            animationClock.restart();
        }

        if (isShooting) {
            int frameOffset = int(currentFrame) * w;
            if (faceRight) {
                sprite.setTextureRect(sf::IntRect(frameOffset, 0, w - 1, h));
            }
            else {
                sprite.setTextureRect(sf::IntRect(frameOffset + w - 1, 0, -(w - 1), h));
            }
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Player::showMessage(std::wstring message, sf::Color color) {
    std::wcout << L"[Оповещение]: " << message << std::endl;
}
