#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "Player.h"
#include "ItemDatabase.h"
#include "StoryManager.h"
#include "DialogueSystem.h"
#include "DialogueDatabase.h"
#include "ApartmentScene.h"
// #include "HallwayScene.h" // Сюда мы позже подключим вторую сцену

class GameManager {
private:
    sf::RenderWindow window;
    sf::View gameView;
    sf::Clock clock;

    ItemDatabase gameDb;
    StoryManager story;
    DialogueDatabase dialogueDb;
    DialogueSystem dialogue;

    Player hero;
    ApartmentScene apartmentScene;
    // HallwayScene hallwayScene; // Переменная для подъезда

public:
    GameManager()
        : window(sf::VideoMode(800, 400), "Survival RPG - Fides: Point of No Return"),
        gameView(sf::FloatRect(0.f, 0.f, 800.f, 400.f)),
        gameDb(NORMAL),
        story(NORMAL),
        hero("sprite_main.png", "spr_streilba_m2.png", 150, 234)
    {
        window.setFramerateLimit(60);
        window.setView(gameView);

        // Начальная настройка Евы
        hero.sprite.setPosition(120, 210); // Рост 180, сидит идеально на полу
        hero.db = &gameDb;

        // Стартовый инвентарь Евы
        hero.inventory.loadItemTexture("Laptop", "icons/laptop.png");
        hero.inventory.loadItemTexture("Medkit", "icons/medkit.png");
        hero.inventory.loadItemTexture("Ammo", "icons/ammo.png");
        hero.inventory.addItem("Laptop", 1);
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

            // Обработка Letterbox-масштабирования при изменении размеров экрана
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
                window.setView(gameView);
            }

            // Передаем ввод игроку
            hero.handleInput(event);

            // Кнопка взаимодействия E (с эффектом печатной машинки)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                if (dialogue.isOpen) {
                    if (dialogue.isPrinting()) {
                        dialogue.forceComplete();
                    }
                    else {
                        dialogue.nextLine();
                    }
                }
                else {
                    // Менеджер сам знает, какой сцене передать клик Е
                    if (story.currentScene == 1) {
                        apartmentScene.handleInteraction(hero, story, dialogue, dialogueDb);
                    }
                    /* else if (story.currentScene == 2) {
                        hallwayScene.handleInteraction(hero, story, dialogue, dialogueDb);
                    } */
                }
            }
        }
    }

    void update(float time) {
        hero.update(time);
        dialogue.update(time); // Эффект печатной машинки обновляется здесь

        // Обновляем логику активной сцены в зависимости от StoryManager
        if (story.currentScene == 1) {
            apartmentScene.updateDistances(hero.sprite.getPosition().x, dialogue.isOpen, hero.inventory.items["Ammo"]);

            // Триггер перехода: Ева взяла ствол, поговорила с Марком и дошла до левого края
            if (hero.sprite.getPosition().x < 20.f && story.talkedToMarkStart && hero.inventory.items["Ammo"] > 0) {
                story.currentScene = 2; // Меняем сцену на Подъезд
                hero.sprite.setPosition(740, 210); // Переносим Еву на правый край новой локации
            }
        }
        /* else if (story.currentScene == 2) {
            hallwayScene.update(time, hero, story, dialogue, dialogueDb);
        } */
    }

    void render() {
        window.clear(sf::Color::Black);

        if (story.currentScene == 1) {
            apartmentScene.draw(window); // Рисует фон и объекты квартиры
        }

        // ВОЗВРАЩАЕМ ЕВУ (Теперь она будет одна!)
        hero.draw(window);

        if (dialogue.isOpen) {
            dialogue.draw(window);
        }

        window.display();
    }


};

#endif

