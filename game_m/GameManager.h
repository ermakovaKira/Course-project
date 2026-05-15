#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "Config.h"
#include "Player.h"
#include "ItemDatabase.h"
#include "StoryManager.h"
#include "DialogueSystem.h"
#include "DialogueDatabase.h"
#include "ApartmentScene.h"  
#include "HallwayScene.h"   
#include "HealthBar.h"

class GameManager {
private:
    sf::RenderWindow window;
    sf::View gameView;
    sf::View uiView;
    sf::Clock clock;

    ItemDatabase gameDb;
    StoryManager story;
    DialogueDatabase dialogueDb;
    DialogueSystem dialogue;

    Player hero;
    ApartmentScene apartmentScene;
    HallwayScene hallwayScene;

    HealthBar playerHPBar;
    HealthBar zombieHPBar;

    sf::Texture questBoxTex;
    sf::Sprite questBoxSprite;
    sf::Font questFont;
    sf::Text questText;

    bool showInventory;
    bool pendingMedkitMessage;
    bool pendingGlockMessage;

public:
    GameManager()
        : window(sf::VideoMode(800, 400), "Survival RPG - Fides: Point of No Return"),
        gameView(sf::FloatRect(0.f, 0.f, 800.f, 400.f)),
        uiView(sf::FloatRect(0.f, 0.f, 800.f, 400.f)),
        gameDb(NORMAL), story(NORMAL),
        hero("sprite_main.png", "spr_streilba_m2.png", 150, 234),
        playerHPBar(150.f, 12.f, sf::Color::Green),
        zombieHPBar(60.f, 7.f, sf::Color::Red)
    {
        window.setFramerateLimit(60);
        window.setView(gameView);

        hero.sprite.setPosition(1150.f, 210.f);
        hero.db = &gameDb;

        hero.inventory.loadItemTexture("Laptop", "icons/laptop.png");
        hero.inventory.loadItemTexture("Medkit", "icons/medkit.png");
        hero.inventory.loadItemTexture("Ammo", "icons/ammo.png");
        hero.inventory.addItem("Laptop", 1);

        showInventory = false;
        pendingMedkitMessage = false;
        pendingGlockMessage = false;

        playerHPBar.update(100.f, 100.f, sf::Vector2f(20.f, 20.f));

        if (!questBoxTex.loadFromFile("icons/quest_bg.png")) {
            std::cout << "Error: icons/quest_bg.png not found!" << std::endl;
        }
        questBoxTex.setSmooth(false);
        questBoxSprite.setTexture(questBoxTex);

        float targetWidth = 440.f;
        float targetHeight = 32.f;
        questBoxSprite.setScale(targetWidth / questBoxSprite.getLocalBounds().width, targetHeight / questBoxSprite.getLocalBounds().height);
        questBoxSprite.setColor(sf::Color(255, 255, 255, 160));
        questBoxSprite.setPosition(180.f, 15.f);

        questFont.loadFromFile("PixeloidSans.ttf");
        questText.setFont(questFont);
        questText.setCharacterSize(11);
        questText.setFillColor(sf::Color(255, 215, 0));
    }

    void run() {
        while (window.isOpen()) {
            float time = clock.getElapsedTime().asMicroseconds() / 700.0f;
            clock.restart();

            processEvents();
            update(time);
            render();
        }
    }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                float windowRatio = (float)event.size.width / (float)event.size.height;
                float viewRatio = 800.f / 400.f;
                sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

                if (windowRatio > viewRatio) {
                    float width = viewRatio / windowRatio;
                    viewport.left = (1.f - width) / 2.f;
                    viewport.width = width;
                }
                else {
                    float height = windowRatio / viewRatio;
                    viewport.top = (1.f - height) / 2.f;
                    viewport.height = height;
                }
                gameView.setViewport(viewport);
                uiView.setViewport(viewport);
                window.setView(gameView);
            }

            if (!dialogue.isOpen && !showInventory) {
                hero.handleInput(event);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::I && !dialogue.isOpen) {
                    showInventory = !showInventory;
                }

                if (event.key.code == sf::Keyboard::E) {
                    if (dialogue.isOpen) {
                        if (dialogue.isPrinting()) {
                            dialogue.forceComplete();
                        }
                        else {
                            dialogue.nextLine();

                            if (!dialogue.isOpen) {
                                if (story.currentScene == 1 && story.talkedToMarkStart && pendingMedkitMessage) {
                                    hero.showMessage(L"ПОЛУЧЕНО: AПТЕЧКА МАРКА", sf::Color::Green);
                                    story.markMovingToExit = true;
                                    pendingMedkitMessage = false;
                                }
                                if (story.currentScene == 1 && hero.inventory.items["Ammo"] > 0 && pendingGlockMessage) {
                                    hero.showMessage(L"ПОЛУЧЕНО: ГЛОК-17 (15 патронов)", sf::Color::Green);
                                    pendingGlockMessage = false;
                                }
                                if (story.currentScene == 2 && !story.hallwayIntroPlayed) {
                                    story.hallwayIntroPlayed = true;
                                }
                            }
                        }
                    }
                    else if (!showInventory) {
                        if (story.currentScene == 1) {
                            if (apartmentScene.nearMark && story.readLaptopEmail && !story.talkedToMarkStart) {
                                pendingMedkitMessage = true;
                            }
                            if (apartmentScene.nearCloset && story.talkedToMarkStart && hero.inventory.items["Ammo"] == 0) {
                                pendingGlockMessage = true;
                            }
                            apartmentScene.handleInteraction(hero, story, dialogue, dialogueDb);
                        }
                        else if (story.currentScene == 2) {
                            hallwayScene.handleInteraction(hero, story, dialogue, dialogueDb);
                        }
                    }
                }
            }
        }
    }

    void update(float time) {
        if (!dialogue.isOpen && !showInventory) {
            hero.update(time);
        }
        dialogue.update(time);

        static bool bulletHitRegistered = false;

        if (story.currentScene == 1) {
            apartmentScene.update(time, hero, story);

            float playerX = hero.sprite.getPosition().x;
            float cameraX = std::max(400.f, std::min(1184.f, playerX));
            gameView.setCenter(cameraX, 200.f);

            if (playerX < 15.f) {
                if (story.talkedToMarkStart && hero.inventory.items["Ammo"] > 0) {
                    hallwayScene.init();
                    story.currentScene = 2;
                    hero.sprite.setPosition(2300.f, 210.f);
                    gameView.setCenter(2000.f, 200.f);
                    window.setView(gameView);

                    questText.setString(story.getCurrentQuestText());
                    sf::FloatRect textBounds = questText.getLocalBounds();
                    questText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
                    questText.setPosition(400.f, 31.f);

                    dialogue.startDialogue(dialogueDb.getDialogue("hallway_intro"));
                }
                else {
                    hero.sprite.setPosition(25.f, 210.f);
                    if (!dialogue.isOpen) {
                        hero.showMessage(L"Я не выйду туда без оружия. Нужно открыть шкаф Марка.", sf::Color::Red);
                    }
                }
            }
        }
        else if (story.currentScene == 2) {
            if (!hallwayScene.isLoaded) {
                hallwayScene.init();
            }

            hallwayScene.update(time, hero, story, dialogue);

            float playerX = hero.sprite.getPosition().x;
            float cameraX = std::max(400.f, std::min(2000.f, playerX));
            gameView.setCenter(cameraX, 200.f);

            if (!dialogue.isOpen && !showInventory && hero.isShooting && hallwayScene.zombie.health > 0) {
                if (int(hero.currentFrame) == 0 && !bulletHitRegistered) {
                    float zombieX = hallwayScene.zombie.getPosition().x;

                    if (!hero.faceRight && playerX > zombieX) {
                        hallwayScene.zombie.health -= 20.f;
                        bulletHitRegistered = true;
                        hero.showMessage(L"ПОПАДАНИЕ!", sf::Color::Yellow);
                    }
                }
            }
            if (!hero.isShooting) {
                bulletHitRegistered = false;
            }
        }

        if (!dialogue.isOpen) {
            questText.setString(story.getCurrentQuestText());
            sf::FloatRect textBounds = questText.getLocalBounds();
            questText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
            questText.setPosition(400.f, 31.f);
        }
    }

    void render() {
        window.clear(sf::Color::Black);

        window.setView(gameView);
        if (story.currentScene == 1) {
            apartmentScene.draw(window);
        }
        else if (story.currentScene == 2) {
            hallwayScene.draw(window);

            if (hallwayScene.zombie.health > 0 && story.hallwayIntroPlayed) {
                float zX = hallwayScene.zombie.getPosition().x + 40.f;
                float zY = hallwayScene.zombie.getPosition().y - 15.f;
                zombieHPBar.update(hallwayScene.zombie.health, 50.f, sf::Vector2f(zX, zY));
                zombieHPBar.draw(window);
            }
        }

        hero.draw(window);
        hero.drawMessage(window);

        window.setView(uiView);

        playerHPBar.update(hero.stats.health, 100.f, sf::Vector2f(20.f, 20.f));
        playerHPBar.draw(window);

        window.draw(questBoxSprite);
        window.draw(questText);

        if (showInventory) {
            hero.inventory.drawUI(window, questFont);
        }

        if (dialogue.isOpen) {
            dialogue.draw(window);
        }

        window.display();
    }
};

#endif
