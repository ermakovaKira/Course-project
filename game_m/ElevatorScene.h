#ifndef ELEVATOR_SCENE_H
#define ELEVATOR_SCENE_H

#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include "NPC.h" 
#include "Interactable.h"
#include "Player.h"
#include "StoryManager.h"
#include "DialogueSystem.h"
#include "DialogueDatabase.h"

class ElevatorScene {
public:
    sf::Texture bgTex;
    sf::Sprite bgSprite;
    bool isLoaded;

    NPC mark;

    sf::RectangleShape hackWindow;
    sf::RectangleShape targetPoints[4]; // ИСПРАВЛЕНО: Явный статический массив из 4 элементов
    sf::RectangleShape waveMarker;

    float markerX;
    float markerSpeed;
    int successfulHits;
    bool waveDirectionRight;
    bool pointHitTracked[4]; // ИСПРАВЛЕНО: Явный статический массив из 4 элементов
    bool wasSpacePressedLastFrame; // Флаг защиты от залипания Пробела

    bool nearPanel, nearElevator;
    bool hackStarted;
    bool hackSuccess;
    bool isMinigameActive;

    Interactable securityPanel;
    Interactable offlineElevator;

public:
    ElevatorScene()
        : securityPanel("laptop_sprite.png", sf::Vector2f(80.f, 210.f), "panel_hack", 45.f, 60.f),
        offlineElevator("laptop_sprite.png", sf::Vector2f(800.f, 210.f), "elevator_shaft", 120.f, 110.f)
    {
        isLoaded = false;
        nearPanel = nearElevator = false;
        hackStarted = false;
        hackSuccess = false;
        isMinigameActive = false;
        wasSpacePressedLastFrame = false;

        markerX = 0.f;
        markerSpeed = 0.08f;
        successfulHits = 0;
        waveDirectionRight = true;

        hackWindow.setSize(sf::Vector2f(300.f, 120.f));
        hackWindow.setFillColor(sf::Color(0, 15, 0, 240));
        hackWindow.setOutlineThickness(2.f);
        hackWindow.setOutlineColor(sf::Color(0, 255, 0));
        hackWindow.setPosition(250.f, 140.f);

        waveMarker.setSize(sf::Vector2f(8.f, 8.f));
        waveMarker.setFillColor(sf::Color::Green);
        waveMarker.setOrigin(4.f, 4.f);
        waveMarker.setOutlineThickness(1.f);
        waveMarker.setOutlineColor(sf::Color::Black);

        for (int i = 0; i < 4; i++) {
            targetPoints[i].setSize(sf::Vector2f(10.f, 10.f));
            targetPoints[i].setFillColor(sf::Color::Red);
            targetPoints[i].setOrigin(5.f, 5.f);
            targetPoints[i].setOutlineThickness(1.f);
            targetPoints[i].setOutlineColor(sf::Color::White);
            pointHitTracked[i] = false;
        }
    }

    void init() {
        if (!bgTex.loadFromFile("elevator_hall.png")) {
            std::cout << "CRITICAL ERROR: elevator_hall.png not found!" << std::endl;
        }
        bgTex.setSmooth(false);
        bgSprite.setTexture(bgTex, true);
        bgSprite.setScale(1600.f / bgTex.getSize().x, 400.f / bgTex.getSize().y);

        mark.init("npc_sprite.png", "mark_move.png", sf::Vector2f(1350.f, 385.f));

        isLoaded = true;
    }

    void resetMinigame() {
        successfulHits = 0;
        markerX = 0.f;
        waveDirectionRight = true;
        wasSpacePressedLastFrame = false;
        for (int i = 0; i < 4; i++) {
            targetPoints[i].setFillColor(sf::Color::Red);
            pointHitTracked[i] = false;
        }
    }

    void update(float time, Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        if (!isLoaded) return;

        float playerX = hero.sprite.getPosition().x;
        updateDistances(playerX, dialogue.isOpen);

        if (isMinigameActive) {
            if (!nearPanel) {
                isMinigameActive = false;
                hero.showMessage(L"СОЕДИНЕНИЕ ПОТЕРЯНО! ВЕРНИТЕСЬ К ПАНЕЛИ!", sf::Color::Red);
                return;
            }

            float startX = hackWindow.getPosition().x + 30.f;
            float endX = hackWindow.getPosition().x + 270.f;
            float totalWidth = endX - startX;
            float centerY = hackWindow.getPosition().y + 60.f;

            for (int i = 0; i < 4; i++) {
                float targetMarkerX = totalWidth * (0.2f * (i + 1));
                float pX = startX + targetMarkerX;
                float pY = centerY + std::sin((targetMarkerX / totalWidth * 5.f) * 1.5f) * 30.f;
                targetPoints[i].setPosition(pX, pY);
            }

            if (waveDirectionRight) {
                markerX += markerSpeed * time;
                if (startX + markerX >= endX) waveDirectionRight = false;
            }
            else {
                markerX -= markerSpeed * time;
                if (markerX <= 0.f) waveDirectionRight = true;
            }

            float currentMarkerGlobalX = startX + markerX;
            float currentMarkerGlobalY = centerY + std::sin((markerX / totalWidth * 5.f) * 1.5f) * 30.f;
            waveMarker.setPosition(currentMarkerGlobalX, currentMarkerGlobalY);

            // ИСПРАВЛЕНО: Строгая покадровая проверка нажатия без залипания
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                if (!wasSpacePressedLastFrame) {
                    wasSpacePressedLastFrame = true;
                    bool pointHit = false;

                    for (int i = 0; i < 4; i++) {
                        if (!pointHitTracked[i]) {
                            float dist = std::abs(currentMarkerGlobalX - targetPoints[i].getPosition().x);
                            if (dist < 22.f) {
                                targetPoints[i].setFillColor(sf::Color::Green);
                                pointHitTracked[i] = true;
                                successfulHits++;
                                hero.showMessage(L"РЕЗОНАНС СИНХРОНИЗИРОВАН!", sf::Color::Green);
                                pointHit = true;
                                break;
                            }
                        }
                    }

                    if (!pointHit) {
                        hero.showMessage(L"СБОЙ ЧАСТОТЫ!", sf::Color::Red);
                    }

                    if (successfulHits >= 4) {
                        isMinigameActive = false;
                        hackSuccess = true;
                        hero.showMessage(L"СИСТЕМА ПЕРЕЗАПУЩЕНА! ЛИФТ ОТКРЫТ!", sf::Color::Green);
                    }
                }
            }
            else {
                wasSpacePressedLastFrame = false;
            }
            return;
        }

        if (!dialogue.isOpen) {
            float markX = mark.getPosition().x;
            float targetMarkX = playerX + (hero.faceRight ? -55.f : 55.f);

            static float markFrame = 0.f;
            if (std::abs(markX - targetMarkX) > 15.f) {
                markFrame += 0.0025f * time;
                if (markFrame >= 4.f) markFrame = 0.f;

                if (markX < targetMarkX) {
                    mark.move(0.08f * time, 0.f);
                    mark.setState(true, static_cast<int>(markFrame));
                    mark.moveSprite.setScale(-std::abs(mark.moveSprite.getScale().x), mark.moveSprite.getScale().y);
                    mark.staticSprite.setScale(-std::abs(mark.staticSprite.getScale().x), mark.staticSprite.getScale().y);
                }
                else {
                    mark.move(-0.08f * time, 0.f);
                    mark.setState(true, static_cast<int>(markFrame));
                    mark.moveSprite.setScale(std::abs(mark.moveSprite.getScale().x), mark.moveSprite.getScale().y);
                    mark.staticSprite.setScale(std::abs(mark.staticSprite.getScale().x), mark.staticSprite.getScale().y);
                }
            }
            else {
                mark.setState(false);
                if (hero.faceRight) {
                    mark.staticSprite.setScale(-std::abs(mark.staticSprite.getScale().x), mark.staticSprite.getScale().y);
                }
                else {
                    mark.staticSprite.setScale(std::abs(mark.staticSprite.getScale().x), mark.staticSprite.getScale().y);
                }
            }
        }
    }

    void updateDistances(float playerX, bool dialogueIsOpen) {
        nearPanel = (std::abs(playerX - 80.f) < 65.f);
        securityPanel.showHint = (nearPanel && !dialogueIsOpen && !hackSuccess);

        nearElevator = (std::abs(playerX - 800.f) < 90.f);
        offlineElevator.showHint = (nearElevator && !dialogueIsOpen);
    }

    void handleInteraction(Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        if (nearPanel) {
            if (!hackStarted) {
                hackStarted = true;
                dialogue.startDialogue(dialogueDb.getDialogue("elevator_panel_hack_start"));
            }
            else if (!hackSuccess) {
                if (!isMinigameActive) {
                    resetMinigame();
                }
                isMinigameActive = true;
            }
            return;
        }

        if (nearElevator) {
            if (hackSuccess) {
                story.currentScene = 4;
            }
            else {
                dialogue.startDialogue(dialogueDb.getDialogue("elevator_stuck"));
            }
            return;
        }
    }

    void draw(sf::RenderWindow& window) {
        if (!isLoaded) return;

        bgSprite.setTexture(bgTex, false);
        bgSprite.setColor(sf::Color::White);
        window.draw(bgSprite);
        mark.draw(window);

        if (isMinigameActive) {
            window.draw(hackWindow);
            for (int i = 0; i < 4; i++) {
                window.draw(targetPoints[i]);
            }
            window.draw(waveMarker);
        }

        sf::Font font;
        if (!font.loadFromFile("PixeloidSans.ttf")) return;

        if (offlineElevator.showHint) {
            sf::Text hint(hackSuccess ? L"Нажмите E (Шагнуть в лифт)" : L"Нажмите E (Осмотр лифта)", font, 12);
            hint.setFillColor(sf::Color::Yellow);
            hint.setOutlineColor(sf::Color::Black);
            hint.setOutlineThickness(1.f);
            hint.setPosition(800.f - 75.f, 220.f);
            window.draw(hint);
        }

        if (securityPanel.showHint) {
            sf::Text hint(isMinigameActive ? L"ВЗЛОМ В ПРОЦЕССЕ..." : L"Нажмите E (Подключить ноутбук)", font, 12);
            hint.setFillColor(sf::Color::Yellow);
            hint.setOutlineColor(sf::Color::Black);
            hint.setOutlineThickness(1.f);
            hint.setPosition(80.f - 85.f, 220.f);
            window.draw(hint);
        }
    }
};

#endif
