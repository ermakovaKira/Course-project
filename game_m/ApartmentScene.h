#ifndef APARTMENT_SCENE_H
#define APARTMENT_SCENE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "NPC.h"
#include "Interactable.h"
#include "Player.h"
#include "StoryManager.h"
#include "DialogueSystem.h"
#include "DialogueDatabase.h"

class ApartmentScene {
public:
    sf::Texture roomTex;
    sf::Sprite roomSprite;

    NPC mark;
    Interactable pcStation;
    Interactable familyPhoto;
    Interactable deskDrawer;

    bool nearMark, nearPc, nearPhoto, nearDrawer;

    ApartmentScene()
        : mark("npc_sprite.png", sf::Vector2f(550, 195)), // Рост Марка 195, позиция Y=195
        pcStation("laptop_sprite.png", sf::Vector2f(330, 245), "pc", 40.f, 30.f),
        familyPhoto("laptop_sprite.png", sf::Vector2f(650, 260), "photo", 15.f, 20.f),
        deskDrawer("laptop_sprite.png", sf::Vector2f(440, 265), "drawer", 30.f, 20.f)
    {
        roomTex.loadFromFile("room.png");
        roomTex.setSmooth(false);
        roomSprite.setTexture(roomTex);
        roomSprite.setScale(800.f / roomSprite.getLocalBounds().width, 400.f / roomSprite.getLocalBounds().height);

        nearMark = nearPc = nearPhoto = nearDrawer = false;
    }

    void updateDistances(float playerX, bool dialogueIsOpen, int ammoCount) {
        nearMark = (std::abs(playerX - mark.sprite.getPosition().x) < 60.f);
        mark.showHint = (nearMark && !dialogueIsOpen);

        nearPc = (std::abs(playerX - pcStation.sprite.getPosition().x) < 50.f);
        pcStation.showHint = (nearPc && !dialogueIsOpen && !nearPc);

        nearPhoto = (std::abs(playerX - familyPhoto.sprite.getPosition().x) < 40.f);
        familyPhoto.showHint = (nearPhoto && !dialogueIsOpen);

        nearDrawer = (std::abs(playerX - deskDrawer.sprite.getPosition().x) < 40.f);
        deskDrawer.showHint = (nearDrawer && !dialogueIsOpen && ammoCount == 0);
    }

    void handleInteraction(Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        if (nearPc && !story.readLaptopEmail) {
            dialogue.startDialogue(dialogueDb.getDialogue("apartment_pc"));
            story.readLaptopEmail = true;
        }
        else if (nearMark) {
            if (!story.readLaptopEmail) {
                dialogue.startDialogue(dialogueDb.getDialogue("apartment_mark_before_pc"));
            }
            else {
                dialogue.startDialogue(dialogueDb.getDialogue("apartment_mark_after_pc"));
                story.talkedToMarkStart = true;
                hero.inventory.addItem("Medkit", 1);
            }
        }
        else if (nearPhoto) {
            dialogue.startDialogue(dialogueDb.getDialogue("inspect_photo"));
            hero.stats.health = std::min(100.f, hero.stats.health + 15.f);
            hero.showMessage(L"+15 HP: Воспоминания", sf::Color::Cyan);
        }
        else if (nearDrawer) {
            if (!story.talkedToMarkStart) {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_drawer_locked"));
            }
            else if (story.talkedToMarkStart && hero.inventory.items["Ammo"] == 0) {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_drawer_open"));
                hero.inventory.addItem("Ammo", 15);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(roomSprite);
        pcStation.draw(window);
        familyPhoto.draw(window);
        deskDrawer.draw(window);
        mark.draw(window);
    }
};

#endif

