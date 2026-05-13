#include "Enemy.h"
#include <cmath>
#include <string> 

using namespace sf;
Enemy::Enemy(std::string pathWalk, std::string pathAttack, sf::Vector2f pos, int width, int height)
    : hpBar(60.f, 5.f, sf::Color::Red)
{
    // 1. Загрузка графики
    textureWalk.loadFromFile(pathWalk);
    textureAttack.loadFromFile(pathAttack);
    textureWalk.setSmooth(false);
    textureAttack.setSmooth(false);

    sprite.setTexture(textureWalk);
    sprite.setPosition(pos);

    // 2. Настройка размеров
    w = width; h = height;

    // ФИКСИРУЕМ РАЗМЕР ВРАГА (75x118)
    float targetW = 75.0f;
    float targetH = 118.0f;
    sprite.setScale(targetW / (float)w, targetH / (float)h);

    sprite.setTextureRect(sf::IntRect(0, 0, w, h));

    // 3. Параметры
    currentFrame = 0;
    state = WALKING;
    speed = 0.07f;
    health = 100.f;
    damage = 15.f;
    attackCooldown = 1000.f;
    timer = 0;
}
void Enemy::update(float time, Player& hero) {
    if (health <= 0) return;

    float playerX = hero.sprite.getPosition().x;
    float zombieX = sprite.getPosition().x;
    float dist = std::abs(playerX - zombieX);

    // 1. Выбор состояния
    if (dist < 40.f) {
        if (state != ATTACKING) {
            state = ATTACKING;
            currentFrame = 0;
        }
    }
    else {
        state = WALKING;
    }

    // 2. Логика анимации и перемещения
    if (state == ATTACKING) {
        sprite.setTexture(textureAttack);
        currentFrame += 0.005f * time;

        // Проверка нанесения урона (в конце анимации)
        if (currentFrame >= 3.8f) { // Почти конец анимации
            if (timer <= 0) {
                hero.stats.health -= damage;
                std::wstring damageMsg = L"-" + std::to_wstring((int)damage) + L" HP";
                hero.showMessage(damageMsg, Color::Red);
                timer = attackCooldown;
            }
            if (currentFrame >= 4.0f) currentFrame = 0; // Зацикливаем атаку
        }
    }
    else {
        sprite.setTexture(textureWalk);
        float directionX = (playerX > zombieX) ? 1.f : -1.f;
        sprite.move(directionX * speed * time, 0);

        currentFrame += 0.004f * time;
        if (currentFrame >= 4.0f) currentFrame = 0;
    }

    if (timer > 0) timer -= time;

    // 3. Отрисовка кадра и поворот
    int frame = int(currentFrame);
    if (frame > 3) frame = 3;

    if (playerX > sprite.getPosition().x) {
        sprite.setTextureRect(IntRect(w * frame, 0, w, h));
    }
    else {
        sprite.setTextureRect(IntRect(w * frame + w, 0, -w, h));
    }

    // Обновляем позицию полоски ХП над головой зомби
    hpBar.update(health, 100.f, Vector2f(sprite.getPosition().x, sprite.getPosition().y - 10));
}

void Enemy::draw(RenderWindow& window) {
    if (health > 0) {
        window.draw(sprite);
        hpBar.draw(window); // ТЕПЕРЬ ПОЛОСКА БУДЕТ ВИДНА
    }
}
