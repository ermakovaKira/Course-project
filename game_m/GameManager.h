#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Config.h"
#include "Player.h"
#include "ItemDatabase.h"
#include "StoryManager.h"
#include "DialogueSystem.h"
#include "DialogueDatabase.h"
#include "ApartmentScene.h"
#include "HallwayScene.h" 

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

    // Скрытые буферные переменные для задержки вывода уведомлений о вещах
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

        hero.sprite.setPosition(120, 210);
        hero.db = &gameDb;

        hero.inventory.loadItemTexture("Laptop", "icons/laptop.png");
        hero.inventory.loadItemTexture("Medkit", "icons/medkit.png");
        hero.inventory.loadItemTexture("Ammo", "icons/ammo.png");
        hero.inventory.addItem("Laptop", 1);

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
        questBoxSprite.setScale(
            targetWidth / questBoxSprite.getLocalBounds().width,
            targetHeight / questBoxSprite.getLocalBounds().height
        );

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

            if (!dialogue.isOpen) {
                hero.handleInput(event);
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                if (dialogue.isOpen) {
                    if (dialogue.isPrinting()) {
                        dialogue.forceComplete();
                    }
                    else {
                        dialogue.nextLine();

                        // --- УМНЫЙ МОМЕНТ ЗАКРЫТИЯ ДИАЛОГА ---
                        if (!dialogue.isOpen) {
                            // 1. Если дочитали диалог в подъезде — только теперь активируем зомби
                            if (story.currentScene == 2 && !story.hallwayIntroPlayed) {
                                story.hallwayIntroPlayed = true;
                            }

                            // 2. Если дочитали диалог Марка после взлома — выводим сообщение об аптечке
                            if (story.currentScene == 1 && story.talkedToMarkStart && pendingMedkitMessage) {
                                hero.showMessage(L"ПОЛУЧЕНО: АПТЕЧКА МАРКА", sf::Color::Green);
                                pendingMedkitMessage = false;
                            }

                            // 3. Если дочитали осмотр ящика — выводим сообщение о Глоке и запускаем ходьбу Марка к выходу
                            if (story.currentScene == 1 && hero.inventory.items["Ammo"] > 0 && pendingGlockMessage) {
                                hero.showMessage(L"ПОЛУЧЕНО: ГЛОК-17 (15 патронов)", sf::Color::Green);
                                story.markMovingToExit = true;
                                pendingGlockMessage = false;
                            }
                        }
                    }
                }
                else {
                    // Перехватываем установку отложенных сообщений перед запуском диалога
                    if (story.currentScene == 1) {
                        if (apartmentScene.nearMark && story.readLaptopEmail && !story.talkedToMarkStart) {
                            pendingMedkitMessage = true; // Запомним, что надо показать аптечку ПОСЛЕ разговора
                        }
                        if (apartmentScene.nearDrawer && story.talkedToMarkStart && hero.inventory.items["Ammo"] == 0) {
                            pendingGlockMessage = true; // Запомним, что надо показать Глок ПОСЛЕ осмотра ящика
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

    void update(float time) {
        if (!dialogue.isOpen) {
            hero.update(time);
        }
        dialogue.update(time);

        static bool bulletHitRegistered = false;

        if (story.currentScene == 1) {
            apartmentScene.update(time, hero, story);

            if (hero.sprite.getPosition().x < 15.f && story.talkedToMarkStart && hero.inventory.items["Ammo"] > 0) {
                hallwayScene.init();
                story.currentScene = 2;
                hero.sprite.setPosition(730, 210);
                dialogue.startDialogue(dialogueDb.getDialogue("hallway_intro"));
            }
        }
        else if (story.currentScene == 2) {
            if (!hallwayScene.isLoaded) {
                hallwayScene.init();
            }

            hallwayScene.update(time, hero, story, dialogue);

            if (!dialogue.isOpen && hero.isShooting && hallwayScene.zombie.health > 0) {
                if (int(hero.currentFrame) == 0 && !bulletHitRegistered) {
                    float playerX = hero.sprite.getPosition().x;
                    float zombieX = hallwayScene.zombie.sprite.getPosition().x;

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

        // --- УПРАВЛЕНИЕ КВЕСТАМИ БЕЗ СПОЙЛЕРОВ ---
        // Если идет диалог — на плашке висит ТЕКУЩЕЕ (старое) задание, а новое не спойлерится заранее!
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
                float zX = hallwayScene.zombie.sprite.getPosition().x + 40.f;
                float zY = hallwayScene.zombie.sprite.getPosition().y - 15.f;
                zombieHPBar.update(hallwayScene.zombie.health, 50.f, sf::Vector2f(zX, zY));
                zombieHPBar.draw(window);
            }
        }

        hero.draw(window);

        window.setView(uiView);

        playerHPBar.update(hero.stats.health, 100.f, sf::Vector2f(20.f, 20.f));
        playerHPBar.draw(window);

        window.draw(questBoxSprite);
        window.draw(questText);

        if (dialogue.isOpen) {
            dialogue.draw(window);
        }

        window.display();
    }
};

#endif
