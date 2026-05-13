#ifndef HALLWAY_SCENE_H
#define HALLWAY_SCENE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "Enemy.h"
#include "NPC.h" // Подключили класс Марка
#include "Interactable.h"
#include "Player.h"
#include "StoryManager.h"
#include "DialogueSystem.h"
#include "DialogueDatabase.h"

class HallwayScene {
public:
    sf::Texture bgTex;
    sf::Sprite bgSprite;

    Enemy zombie;
    NPC mark; // Марк теперь присутствует в подъезде!
    Interactable vendingMachine;
    Interactable utilityDoor;

    bool nearMachine, nearDoor;
    bool keysDropped;
    bool keysPicked;
    bool isLoaded;

    HallwayScene()
        : zombie("zombie_walk.png", "zombie_attack.png", sf::Vector2f(-1000, -1000), 150, 236),
        mark("npc_sprite.png", sf::Vector2f(-1000, -1000)), // Прячем до загрузки уровня
        vendingMachine("laptop_sprite.png", sf::Vector2f(-1000, -1000), "vending", 40.f, 50.f),
        utilityDoor("laptop_sprite.png", sf::Vector2f(-1000, -1000), "utility_door", 45.f, 90.f)
    {
        nearMachine = nearDoor = false;
        keysDropped = false;
        keysPicked = false;
        isLoaded = false;
    }

    void init() {
        if (isLoaded) return;

        if (!bgTex.loadFromFile("hallway.png")) {
            std::cout << "CRITICAL ERROR: hallway.png not found!" << std::endl;
        }
        bgTex.setSmooth(false);
        bgSprite.setTexture(bgTex);
        bgSprite.setScale(800.f / bgSprite.getLocalBounds().width, 400.f / bgSprite.getLocalBounds().height);

        // Спавним мистера Грина слева
        zombie.sprite.setPosition(180, 210);
        zombie.health = 50.f;

        // Спавним Марка РЯДОМ С ЕВОЙ (чуть правее неё, у косяка двери)
        mark.sprite.setPosition(770, 195);
        // Синхронизируем рост Марка
        float currentScale = 195.0f / mark.sprite.getLocalBounds().height;
        mark.sprite.setScale(currentScale, currentScale);

        // Настройка скрытых коллизий автомата и подсобки
        vendingMachine.texture.loadFromFile("laptop_sprite.png");
        vendingMachine.sprite.setTexture(vendingMachine.texture, true);
        vendingMachine.sprite.setPosition(420, 210);
        vendingMachine.sprite.setScale(800.f / vendingMachine.sprite.getLocalBounds().width * 0.1f, 400.f / vendingMachine.sprite.getLocalBounds().height * 0.35f);

        utilityDoor.texture.loadFromFile("laptop_sprite.png");
        utilityDoor.sprite.setTexture(utilityDoor.texture, true);
        utilityDoor.sprite.setPosition(650, 180);
        utilityDoor.sprite.setScale(70.f / utilityDoor.sprite.getLocalBounds().width, 170.f / utilityDoor.sprite.getLocalBounds().height);

        isLoaded = true;
        std::cout << "SUCCESS: HallwayScene loaded with Mark!" << std::endl;
    }

    void update(float time, Player& hero, StoryManager& story, DialogueSystem& dialogue) {
        if (!isLoaded) return;

        // ВРАГ НАЧИНАЕТ ХОДИТЬ ТОЛЬКО ПОСЛЕ ТОГО, КАК ПРОИГРАЛСЯ СЮЖЕТНЫЙ ДИАЛОГ!
        if (story.hallwayIntroPlayed && !dialogue.isOpen) {
            zombie.update(time, hero);
        }

        if (zombie.health <= 0 && !keysDropped) {
            keysDropped = true;
            hero.showMessage(L"Мистер Грин упал. На полу что-то блестит.", sf::Color::Red);
        }

        float pX = hero.sprite.getPosition().x;
        nearMachine = (std::abs(pX - vendingMachine.sprite.getPosition().x) < 50.f);
        vendingMachine.showHint = (nearMachine && !dialogue.isOpen && hero.inventory.items["Flash"] == 0);

        nearDoor = (std::abs(pX - utilityDoor.sprite.getPosition().x) < 50.f);
        utilityDoor.showHint = (nearDoor && !dialogue.isOpen);
    }

    void handleInteraction(Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        if (!isLoaded) return;

        if (keysDropped && !keysPicked && std::abs(hero.sprite.getPosition().x - zombie.sprite.getPosition().x) < 60.f) {
            keysPicked = true;
            hero.inventory.addItem("Keys", 1);
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_find_keys"));
            hero.showMessage(L"ПОЛУЧЕНЫ: СВЯЗКА КЛЮЧЕЙ", sf::Color::Green);
            return;
        }

        if (nearMachine && hero.inventory.items["Flash"] == 0) {
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_vending"));
            hero.inventory.addItem("Flash", 1);
            hero.showMessage(L"ПОЛУЧЕН: ЭНЕРГЕТИК FLASH", sf::Color::Cyan);
        }

        if (nearDoor) {
            if (!keysPicked) {
                dialogue.startDialogue(dialogueDb.getDialogue("hallway_door_locked"));
            }
            else if (keysPicked && hero.inventory.items["Ammo"] == 15) {
                dialogue.startDialogue(dialogueDb.getDialogue("hallway_door_open"));
                hero.inventory.addItem("Ammo", 30);
                hero.showMessage(L"ПОПОЛНЕНИЕ: +30 ПАТРОНОВ", sf::Color::Green);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        if (!isLoaded) return;

        window.draw(bgSprite);

        if (vendingMachine.showHint) vendingMachine.draw(window);
        if (utilityDoor.showHint) utilityDoor.draw(window);

        mark.draw(window); // Рисуем Марка в подъезде

        if (zombie.health > 0) {
            zombie.draw(window);
        }
        else if (keysDropped && !keysPicked) {
            sf::RectangleShape keyDraw(sf::Vector2f(12, 8));
            keyDraw.setFillColor(sf::Color::Yellow);
            keyDraw.setPosition(zombie.sprite.getPosition().x + 20, 365);
            window.draw(keyDraw);
        }
    }
};

#endif

