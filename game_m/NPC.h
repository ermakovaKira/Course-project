#ifndef NPC_H
#define NPC_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "DialogueSystem.h" // Для поддержки структуры DialogueLine

class NPC {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    std::vector<DialogueLine> dialogueLines; // Теперь используем структуру из базы диалогов

    sf::Text hintText;
    sf::Font font;
    bool showHint;

    NPC(std::string texturePath, sf::Vector2f pos) {
        // --- ВНУТРИ КОНСТРУКТОРА NPC.H ---
        if (texture.loadFromFile(texturePath)) {
            texture.setSmooth(false);
            sprite.setTexture(texture);

            // МАРК ЧУТЬ ВЫШЕ ЕВЫ (195 пикселей вместо 180)
            float targetHeight = 195.0f;
            float currentScale = targetHeight / sprite.getLocalBounds().height;
            sprite.setScale(currentScale, currentScale);

            sprite.setPosition(pos);
        }

        showHint = false;

        // Настройка текста подсказки "Нажмите E" (оставляем как было)
        if (font.loadFromFile("PixeloidSans.ttf")) {
            hintText.setFont(font);
            hintText.setString(L"Нажмите E");
            hintText.setCharacterSize(14);
            hintText.setFillColor(sf::Color::Yellow);
            hintText.setOrigin(hintText.getLocalBounds().width / 2, 0);
        }
    }


    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        // Показываем текст только когда флаг активен
        if (showHint) {
            // Позиционируем над головой (75.f — фиксированная ширина)
            hintText.setPosition(sprite.getPosition().x + (75.0f / 2.0f), sprite.getPosition().y - 30);
            window.draw(hintText);
        }
    }
};

#endif
