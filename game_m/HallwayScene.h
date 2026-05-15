#ifndef HALLWAY_SCENE_H
#define HALLWAY_SCENE_H

#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
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
    bool isLoaded;

    NPC zombie;
    NPC mark;

    Interactable doorCloset;
    Interactable apartment40;
    Interactable apartment40Note;

    bool nearZombie, nearCloset, nearApartment, nearNote, nearMark;
    bool noteRead;

    HallwayScene()
        : doorCloset("laptop_sprite.png", sf::Vector2f(200.f, 200.f), "closet_door", 60.f, 90.f),
        apartment40("laptop_sprite.png", sf::Vector2f(1200.f, 200.f), "door40", 70.f, 90.f),
        apartment40Note("laptop_sprite.png", sf::Vector2f(1250.f, 250.f), "note40", 30.f, 20.f)
    {
        isLoaded = false;
        nearZombie = nearCloset = nearApartment = nearNote = nearMark = false;
        noteRead = false;
    }

    void init() {
        if (isLoaded) return;

        if (!bgTex.loadFromFile("hallway.png")) {
            std::cout << "CRITICAL ERROR: hallway.png not found!" << std::endl;
        }
        bgTex.setSmooth(false);
        bgSprite.setTexture(bgTex);
        bgSprite.setScale(2400.f / bgTex.getSize().x, 400.f / bgTex.getSize().y);

        zombie.init("zombie_static.png", "zombie_static.png", sf::Vector2f(400.f, 370.f));
        zombie.health = 60.f;

        mark.init("npc_sprite.png", "mark_move.png", sf::Vector2f(2360.f, 370.f));

        isLoaded = true;
    }

    void update(float time, Player& hero, StoryManager& story, DialogueSystem& dialogue) {
        if (!isLoaded) return;

        float playerX = hero.sprite.getPosition().x;

        if (story.hallwayIntroPlayed && zombie.health > 0 && !dialogue.isOpen) {
            float zombieX = zombie.getPosition().x;

            static float zombieFrame = 0.f;
            zombieFrame += 0.005f * time;
            if (zombieFrame >= 4.f) zombieFrame = 0.f;

            if (zombieX < playerX - 10.f) {
                zombie.move(0.04f * time, 0.f);
                zombie.setState(true, static_cast<int>(zombieFrame));
            }
            else if (zombieX > playerX + 10.f) {
                zombie.move(-0.04f * time, 0.f);
                zombie.setState(true, static_cast<int>(zombieFrame));
            }

            if (std::abs(playerX - zombieX) < 35.f) {
                hero.stats.health -= 0.2f * time;
                if (hero.stats.health < 0.f) hero.stats.health = 0.f;
            }
        }
        else if (zombie.health <= 0) {
            zombie.setState(false);
            zombie.staticSprite.setColor(sf::Color(100, 100, 100, 150));
        }

        if (!dialogue.isOpen) {
            float markX = mark.getPosition().x;
            float targetMarkX = playerX + (hero.faceRight ? -50.f : 50.f);

            static float markFrame = 0.f;
            if (std::abs(markX - targetMarkX) > 15.f) {
                markFrame += 0.0025f * time;
                if (markFrame >= 4.f) markFrame = 0.f;

                if (markX < targetMarkX) {
                    mark.move(0.09f * time, 0.f);
                    mark.setState(true, static_cast<int>(markFrame));
                }
                else {
                    mark.move(-0.09f * time, 0.f);
                    mark.setState(true, static_cast<int>(markFrame));
                }
            }
            else {
                mark.setState(false);
            }
        }

        updateDistances(playerX, dialogue.isOpen, story, hero.inventory.items["Keys"]);
    }

    void updateDistances(float playerX, bool dialogueIsOpen, StoryManager& story, int hasKeys) {
        nearZombie = (std::abs(playerX - zombie.getPosition().x) < 50.f);
        zombie.showHint = (nearZombie && zombie.health <= 0 && hasKeys == 0 && !dialogueIsOpen);

        nearCloset = (std::abs(playerX - doorCloset.sprite.getPosition().x) < 50.f);
        doorCloset.showHint = (nearCloset && !dialogueIsOpen);

        nearApartment = (std::abs(playerX - apartment40.sprite.getPosition().x) < 50.f);
        apartment40.showHint = (nearApartment && !dialogueIsOpen);

        nearNote = (std::abs(playerX - apartment40Note.sprite.getPosition().x) < 40.f);
        apartment40Note.showHint = (nearNote && !dialogueIsOpen && !noteRead);
    }

    void handleInteraction(Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        if (nearZombie && zombie.health <= 0 && hero.inventory.items["Keys"] == 0) {
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_find_keys"));
            hero.inventory.addItem("Keys", 1);
            return;
        }

        if (nearApartment) {
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_inspect_apartment_40"));
            return;
        }

        if (nearNote && !noteRead) {
            noteRead = true;
            dialogue.startDialogue(dialogueDb.getDialogue("hallway_note_uncle_tolya"));
            return;
        }

        if (nearCloset) {
            if (hero.inventory.items["Keys"] == 0) {
                dialogue.startDialogue(dialogueDb.getDialogue("hallway_door_locked"));
            }
            else if (hero.inventory.items["Ammo"] <= 15) {
                dialogue.startDialogue(dialogueDb.getDialogue("hallway_door_open"));
                hero.inventory.addItem("Ammo", 30);
            }
            return;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(bgSprite);
        doorCloset.draw(window);
        apartment40.draw(window);
        apartment40Note.draw(window);
        zombie.draw(window);
        mark.draw(window);

        if (zombie.showHint) {
            sf::Font font;
            if (font.loadFromFile("PixeloidSans.ttf")) {
                sf::Text hint(L"Нажмите E (Обыскать)", font, 12);
                hint.setFillColor(sf::Color::Yellow);
                hint.setOutlineColor(sf::Color::Black);
                hint.setOutlineThickness(1.f);
                hint.setPosition(zombie.getPosition().x + 10.f, zombie.getPosition().y - 20.f);
                window.draw(hint);
            }
        }
    }
};

#endif
