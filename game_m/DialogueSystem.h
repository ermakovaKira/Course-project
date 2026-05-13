#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct DialogueLine {
    std::wstring text;
    sf::Texture* portraitTex;
};

class DialogueSystem {
public:
    sf::RectangleShape box;
    sf::Font font;
    sf::Text uiText;
    sf::Sprite portraitSprite;
    bool isOpen;

    std::vector<DialogueLine> lines;
    int currentLine;

    // Переменные для эффекта плавного бега букв
    std::wstring fullText;
    std::wstring displayedText;
    float textTimer;
    float textSpeed;
    size_t charIndex;

    DialogueSystem() {
        isOpen = false;
        currentLine = 0;
        textTimer = 0.f;
        textSpeed = 30.f; // Скорость бега букв (меньше — быстрее)
        charIndex = 0;

        box.setSize(sf::Vector2f(630, 100));
        box.setFillColor(sf::Color(20, 20, 20, 230));
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color::White);
        box.setPosition(160, 290);

        font.loadFromFile("PixeloidSans.ttf");
        uiText.setFont(font);
        uiText.setCharacterSize(16);
        uiText.setFillColor(sf::Color::White);
        uiText.setPosition(180, 305);

        portraitSprite.setPosition(10, 250);
    }

    void startDialogue(std::vector<DialogueLine> newLines) {
        lines = newLines;
        currentLine = 0;
        isOpen = true;
        applyLine();
    }

    void nextLine() {
        currentLine++;
        if (currentLine < lines.size()) {
            applyLine();
        }
        else {
            isOpen = false;
        }
    }

    // Проверяем, бегут ли ещё буквы на экране прямо сейчас
    bool isPrinting() {
        return charIndex < fullText.length();
    }

    // Мгновенно отобразить всю строку (если игрок нажал Е во время печати)
    void forceComplete() {
        displayedText = fullText;
        uiText.setString(displayedText);
        charIndex = fullText.length();
    }

    // Метод обновления таймера букв (вызывается в GameManager.h)
    void update(float time) {
        if (!isOpen) return;

        if (charIndex < fullText.length()) {
            textTimer += time;
            if (textTimer >= textSpeed) {
                displayedText += fullText[charIndex];
                uiText.setString(displayedText);
                charIndex++;
                textTimer = 0.f;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        if (isOpen) {
            window.draw(box);
            window.draw(uiText);
            if (portraitSprite.getTexture() != nullptr) {
                window.draw(portraitSprite);
            }
        }
    }

private:
    void wrapText(sf::Text& text, float maxWidth) {
        std::wstring source = text.getString();
        std::wstring wrapped = L"";
        std::wstring line = L"";

        std::size_t start = 0;
        std::size_t end = source.find(L' ');

        while (end != std::wstring::npos) {
            std::wstring word = source.substr(start, end - start);
            text.setString(line + word + L" ");
            if (text.getGlobalBounds().width > maxWidth) {
                wrapped += line + L"\n";
                line = word + L" ";
            }
            else {
                line += word + L" ";
            }
            start = end + 1;
            end = source.find(L' ', start);
        }

        std::wstring lastWord = source.substr(start);
        text.setString(line + lastWord);
        if (text.getGlobalBounds().width > maxWidth) {
            wrapped += line + L"\n" + lastWord;
        }
        else {
            line += lastWord;
            wrapped += line;
        }
        text.setString(wrapped);
    }

    void applyLine() {
        fullText = lines[currentLine].text;

        // Прогоняем текст через автоперенос строк заранее
        sf::Text tempText = uiText;
        tempText.setString(fullText);
        wrapText(tempText, 580.f);
        fullText = tempText.getString();

        // Сбрасываем эффекты печати для новой фразы
        displayedText = L"";
        charIndex = 0;
        textTimer = 0.f;
        uiText.setString(displayedText);

        if (lines[currentLine].portraitTex != nullptr) {
            portraitSprite.setTexture(*lines[currentLine].portraitTex, true);
            float targetSize = 140.0f;
            portraitSprite.setScale(targetSize / portraitSprite.getLocalBounds().width, targetSize / portraitSprite.getLocalBounds().height);
        }
        else {
            portraitSprite.setTextureRect(sf::IntRect());
        }
    }
};

#endif
