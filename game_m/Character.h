#ifndef CHARACTER_H
#define CHARACTER_H
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Character {
protected:
    sf::Sprite staticSprite;
    sf::Sprite moveSprite;
    sf::Texture staticTex;
    sf::Texture moveTex;
    bool isMoveTexLoaded;
    float targetHeight;

public:
    float health;
    bool isMovingNow;
    bool faceRight;

    Character() {
        isMoveTexLoaded = false;
        health = 100.f;
        isMovingNow = false;
        faceRight = true;
        targetHeight = 189.f;
    }

    virtual ~Character() {}

    virtual void init(std::string staticFile, std::string moveFile, sf::Vector2f startPos) {
        if (!staticTex.loadFromFile(staticFile)) {
            std::cout << "Error: " << staticFile << " not found!" << std::endl;
        }
        staticTex.setSmooth(false);

        if (!moveTex.loadFromFile(moveFile)) {
            std::cout << "Error: " << moveFile << " not found!" << std::endl;
            isMoveTexLoaded = false;
        }
        else {
            moveTex.setSmooth(false);
            isMoveTexLoaded = true;
        }

        staticSprite.setTexture(staticTex);
        int staticW = static_cast<int>(staticTex.getSize().x);
        int staticH = static_cast<int>(staticTex.getSize().y);
        staticSprite.setTextureRect(sf::IntRect(0, 0, staticW, staticH));
        staticSprite.setOrigin(static_cast<float>(staticW) / 2.f, static_cast<float>(staticH));

        float scaleFactorStatic = targetHeight / staticSprite.getLocalBounds().height;
        staticSprite.setScale(scaleFactorStatic, scaleFactorStatic);
        staticSprite.setPosition(startPos);

        if (isMoveTexLoaded) {
            moveSprite.setTexture(moveTex);
            moveSprite.setTextureRect(sf::IntRect(0, 0, 150, 234));
            moveSprite.setOrigin(75.f, 234.f);
            float scaleFactorMove = targetHeight / 234.f;
            moveSprite.setScale(scaleFactorMove, scaleFactorMove);
        }
        moveSprite.setPosition(startPos);
    }

    virtual void setState(bool isMoving, int frameIndex = 0) {
        isMovingNow = isMoving;
        if (isMoving && isMoveTexLoaded) {
            moveSprite.setTextureRect(sf::IntRect(frameIndex * 150, 0, 150, 234));
        }
    }

    virtual void setPosition(float x, float y) {
        staticSprite.setPosition(x, y);
        moveSprite.setPosition(x, y);
    }

    virtual sf::Vector2f getPosition() const {
        return isMovingNow ? moveSprite.getPosition() : staticSprite.getPosition();
    }

    virtual void move(float offsetX, float offsetY) {
        staticSprite.move(offsetX, offsetY);
        moveSprite.move(offsetX, offsetY);
    }

    virtual void setFacing(bool right) {
        faceRight = right;
        float currentScaleX = std::abs(staticSprite.getScale().x);
        float currentScaleY = staticSprite.getScale().y;
        float factor = faceRight ? 1.f : -1.f;

        staticSprite.setScale(currentScaleX * factor, currentScaleY);
        if (isMoveTexLoaded) {
            moveSprite.setScale(currentScaleX * factor, currentScaleY);
        }
    }

    virtual void draw(sf::RenderWindow& window) {
        if (isMovingNow && isMoveTexLoaded) {
            window.draw(moveSprite);
        }
        else {
            window.draw(staticSprite);
        }
    }
};

#endif

