#ifndef HALLWAY_SCENE_H
#define HALLWAY_SCENE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "Enemy.h"
#include "NPC.h" 
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
    NPC mark;
    Interactable vendingMachine;
    Interactable utilityDoor;

    bool nearMachine, nearDoor;
    bool keysDropped;
    bool keysPicked;
    bool isLoaded;

    HallwayScene()
        : zombie("zombie_walk.png", "zombie_attack.png", sf::Vector2f(-1000, -1000), 150, 236),
        mark("npc_sprite.png", sf::Vector2f(-1000, -1000)),
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

        zombie.sprite.setPosition(180, 210);
        zombie.health = 50.f;

        mark.sprite.setPosition(770, 195);
        float currentScale = 195.0f / mark.sprite.getLocalBounds().height;
        mark.sprite.setScale(currentScale, currentScale);

        vendingMachine.texture.loadFromFile("laptop_sprite.png");
        vendingMachine.sprite.setTexture(vendingMachine.texture, true);
        vendingMachine.sprite.setPosition(420, 210);
        vendingMachine.sprite.setScale(800.f / vendingMachine.sprite.getLocalBounds().width * 0.1f, 400.f / vendingMachine.sprite.getLocalBounds().height * 0.35f);

        utilityDoor.texture.loadFromFile("laptop_sprite.png");
        utilityDoor.sprite.setTexture(utilityDoor.texture, true);
        utilityDoor.sprite.setPosition(650, 180);
        utilityDoor.sprite.setScale(70.f / utilityDoor.sprite.getLocalBounds().width, 170.f / utilityDoor.sprite.getLocalBounds().height);

        isLoaded = true;
    }

    void update(float time, Player& hero, StoryManager& story, DialogueSystem& dialogue) {
        if (!isLoaded) return;

        if (story.hallwayIntroPlayed && !dialogue.isOpen) {
            zombie.update(time, hero);
        }

        if (zombie.health <= 0 && !keysDropped) {
            keysDropped = true;
            hero.showMessage(L"Ìèñòåð Ãðèí óïàë. Íà ïîëó ÷òî-òî áëåñòèò.", sf::Color::Red);
        }

        float pX = hero.sprite.getPosition().x;

        // ÁÅÇÎÏÀÑÍÀß ÏÐÎÂÅÐÊÀ: Ïðîâåðÿåì, ñóùåñòâóåò ëè êëþ÷ "Flash" â ñëîâàðå ÷åðåç âñòðîåííûé ìåòîä count()
        bool hasFlash = (hero.inventory.items.count("Flash") > 0);
        nearMachine = (std::abs(pX - vendingMachine.sprite.getPosition().x) < 50.f);
        vendingMachine.showHint = (nearMachine && !dialogue.isOpen && !hasFlash);

        nearDoor = (std::abs(pX - utilityDoor.sprite.getPosition().x) < 50.f);
        utilityDoor.showHint = (nearDoor && !dialogue.isOpen);
    }

    void handleInteraction(Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        if (!isLoaded) return;

        if (keysDropped && !keysPicked && std::abs(hero.sprite.getPosition().x - zombie.sprite.getPosition().x) < 60.f) {
            keysPicked = true;
            hero.inventory.addItem("Keys", 1);
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_find_keys"));
            return;
        }

        // ÁÅÇÎÏÀÑÍÛÉ ÎÁÛÑÊ ÀÂÒÎÌÀÒÀ: Èñïîëüçóåì âñòðîåííûé ìåòîä count()
        bool hasFlash = (hero.inventory.items.count("Flash") > 0);
        if (nearMachine && !hasFlash) {
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_vending"));
            hero.inventory.addItem("Flash", 1);
            return;
        }

        // ÁÅÇÎÏÀÑÍÛÉ ÎÁÛÑÊ ÏÎÄÑÎÁÊÈ
        if (nearDoor) {
            if (!keysPicked) {
                dialogue.startDialogue(dialogueDb.getDialogue("hallway_door_locked"));
            }
            else if (keysPicked) {
                // Ïðîâåðÿåì êîëè÷åñòâî ïàòðîíîâ, åñëè îíè åñòü â ñëîâàðå
                int currentAmmo = 0;
                if (hero.inventory.items.count("Ammo") > 0) {
                    currentAmmo = hero.inventory.items["Ammo"];
                }

                if (currentAmmo <= 15) {
                    dialogue.startDialogue(dialogueDb.getDialogue("hallway_door_open"));
                    hero.inventory.addItem("Ammo", 30);
                }
            }
            return;
        }
    }

    void draw(sf::RenderWindow& window) {
        if (!isLoaded) return;

        window.draw(bgSprite);

        if (vendingMachine.showHint) vendingMachine.draw(window);
        if (utilityDoor.showHint) utilityDoor.draw(window);

        mark.draw(window);

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
