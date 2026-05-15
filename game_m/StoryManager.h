#ifndef STORY_MANAGER_H
#define STORY_MANAGER_H

#pragma once
#include <string>
#include "Config.h" 

class StoryManager {
public:
    int currentScene;
    Difficulty difficulty;

    // --- СЮЖЕТНЫЕ ПЕРЕМЕННЫЕ АКТА I (КВАРТИРА И ПОДЪЕЗД) ---
    bool noteRead;               // Прочитана ли записка Марка на кухонном столе
    bool readLaptopEmail;        // Взломан ли стационарный ПК
    bool talkedToMarkStart;      // Разговор с Марком состоялся
    bool markMovingToExit;       // Марк идёт в прихожую к шкафу за Глоком

    bool hallwayIntroPlayed;     // Диалог в подъезде  прочитан


    StoryManager(Difficulty diff = NORMAL) {
        currentScene = 1;
        difficulty = diff;

        noteRead = false;
        readLaptopEmail = false;
        talkedToMarkStart = false;
        markMovingToExit = false;

        hallwayIntroPlayed = false;
    }


    std::wstring getCurrentQuestText() {
        if (currentScene == 1) {
            if (!noteRead) {
                return L"Задание: Исследовать кухню и осмотреть обеденный стол";
            }
            if (!readLaptopEmail) {
                return L"Задание: Взломать брандмауэр и проверить сеть на ПК";
            }
            if (!talkedToMarkStart) {
                return L"Задание: Рассказать Марку о трагедии с родителями";
            }
            return L"Задание: Ввести код, открыть шкаф в прихожей и забрать Глок-17";
        }
        else if (currentScene == 2) {
            if (!hallwayIntroPlayed) {
                return L"Задание: Осмотреться в темном коридоре подъезда";
            }
            return L"Задание: Уничтожить дядю Толю и забрать связку ключей";
        }

        return L"Задание: Найти выход из жилого комплекса 'Фидес'";
    }
};

#endif
