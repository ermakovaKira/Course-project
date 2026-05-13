#ifndef APARTMENT_SCENE_H
#define APARTMENT_SCENE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
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
        : mark("npc_sprite.png", sf::Vector2f(550, 195)),
        pcStation("laptop_sprite.png", sf::Vector2f(330, 245), "pc", 40.f, 30.f),
        familyPhoto("laptop_sprite.png", sf::Vector2f(650, 260), "photo", 15.f, 20.f),
        deskDrawer("laptop_sprite.png", sf::Vector2f(440, 265), "drawer", 30.f, 20.f)
    {
        if (!roomTex.loadFromFile("room.png")) {
            std::cout << "Error: room.png not found!" << std::endl;
        }
        roomTex.setSmooth(false);
        roomSprite.setTexture(roomTex);
        roomSprite.setScale(800.f / roomSprite.getLocalBounds().width, 400.f / roomSprite.getLocalBounds().height);

        nearMark = nearPc = nearPhoto = nearDrawer = false;
    }

    void update(float time, Player& hero, StoryManager& story) {
        // Логика перемещения Марка к левой двери (к выходу)
        if (story.markMovingToExit) {
            float targetExitX = 60.f;
            float currentMarkX = mark.sprite.getPosition().x;

            if (currentMarkX > targetExitX) {
                mark.sprite.move(-0.08f * time, 0.f);

                static float markFrame = 0.f;
                markFrame += 0.01f * time;
                if (markFrame >= 4.f) markFrame = 0.f;

                mark.sprite.setTextureRect(sf::IntRect(int(markFrame) * 150 + 150, 0, -150, (int)mark.sprite.getLocalBounds().height));
            }
            else {
                mark.sprite.setPosition(targetExitX, 195);
                mark.sprite.setTextureRect(sf::IntRect(0, 0, 150, (int)mark.sprite.getLocalBounds().height));
            }
        }

        // Обновляем дистанции для подсказок взаимодействия
        updateDistances(hero.sprite.getPosition().x, false, hero.inventory.items["Ammo"]);
    }

    // ПОЛНОСТЬЮ ИСПРАВЛЕННЫЙ МЕТОД: Подсказка над ПК теперь горит идеально стабильно!
    void updateDistances(float playerX, bool dialogueIsOpen, int ammoCount) {
        nearMark = (std::abs(playerX - mark.sprite.getPosition().x) < 60.f);
        mark.showHint = (nearMark && !dialogueIsOpen);

        // ПОЛНОСТЬЮ ИСПРАВЛЕННАЯ СТРОКА ДЛЯ НОУТБУКА:
        nearPc = (std::abs(playerX - pcStation.sprite.getPosition().x) < 50.f);
        pcStation.showHint = (nearPc && !dialogueIsOpen); // Подсказка горит, если Ева просто стоит рядом

        nearPhoto = (std::abs(playerX - familyPhoto.sprite.getPosition().x) < 40.f);
        familyPhoto.showHint = (nearPhoto && !dialogueIsOpen);

        nearDrawer = (std::abs(playerX - deskDrawer.sprite.getPosition().x) < 40.f);
        deskDrawer.showHint = (nearDrawer && !dialogueIsOpen && ammoCount == 0);
    }

    void handleInteraction(Player& hero, StoryManager& story, DialogueSystem& dialogue, DialogueDatabase& dialogueDb) {
        // Взаимодействие со стационарным ПК (Взлом брандмауэра)
        if (nearPc && !story.readLaptopEmail) {
            dialogue.startDialogue(dialogueDb.getDialogue("apartment_pc"));
            story.readLaptopEmail = true;
            return;
        }

        // Взаимодействие с Марком
        if (nearMark) {
            if (!story.readLaptopEmail) {
                dialogue.startDialogue(dialogueDb.getDialogue("apartment_mark_before_pc"));
            }
            else {
                dialogue.startDialogue(dialogueDb.getDialogue("apartment_mark_after_pc"));
                if (!story.talkedToMarkStart) {
                    story.talkedToMarkStart = true;
                    hero.inventory.addItem("Medkit", 1);
                    // УДАЛЕНО: hero.showMessage(...); (перенесли в GameManager)
                }
            }
            return;
        }

        if (nearDrawer) {
            if (!story.talkedToMarkStart) {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_drawer_locked"));
            }
            else if (story.talkedToMarkStart && hero.inventory.items["Ammo"] == 0) {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_drawer_open"));
                hero.inventory.addItem("Ammo", 15);
                // УДАЛЕНО: hero.showMessage(...); и story.markMovingToExit = true; (перенесли в GameManager)
            }
            return;
        }

        // Взаимодействие с фотографией семьи
        if (nearPhoto) {
            if (!story.readLaptopEmail) {
                std::vector<DialogueLine> prePhotoLines = {
                    { L"ЕВА: Фотография моей семьи... Сейчас не время ностальгировать, мне нужно сначала взломать городской узел связи.", nullptr }
                };
                dialogue.startDialogue(prePhotoLines);
            }
            else {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_photo"));
            }
            return;
        }

        // Взаимодействие с выдвижным ящиком стола (Глок-17)
        if (nearDrawer) {
            if (!story.talkedToMarkStart) {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_drawer_locked"));
            }
            else if (story.talkedToMarkStart && hero.inventory.items["Ammo"] == 0) {
                dialogue.startDialogue(dialogueDb.getDialogue("inspect_drawer_open"));
                hero.inventory.addItem("Ammo", 15);
                hero.showMessage(L"ПОЛУЧЕНО: ГЛОК-17 (15 патронов)", sf::Color::Green);

                // Марк срывается со своего места и идет караулить Еву у выхода
                story.markMovingToExit = true;
            }
            return;
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
