#pragma once // Защищает файлы от дублирования перечислений (Убирает ошибку C2011)

#include <string>
#include "Config.h"
// Возвращаем enum на место, теперь он объявлен правильно и безопасно!


class StoryManager {
public:
    int currentScene;
    Difficulty difficulty;

    // Сюжетные переменные Сцены 1 (Квартира)
    bool readLaptopEmail;
    bool talkedToMarkStart;
    bool markMovingToExit;

    // Сюжетные переменные Сцены 2 (Подъезд)
    bool hallwayIntroPlayed;

    StoryManager(Difficulty diff = NORMAL) {
        currentScene = 1;
        difficulty = diff;
        readLaptopEmail = false;
        talkedToMarkStart = false;
        markMovingToExit = false;
        hallwayIntroPlayed = false;
    }

    // ДЕТАЛИЗИРОВАННЫЙ КВЕСТ-ТРЕКЕР С КОНКРЕТНЫМИ ШАГАМИ ДЛЯ ПОЛЬЗОВАТЕЛЯ
    std::wstring getCurrentQuestText() {
        if (currentScene == 1) {
            if (!readLaptopEmail) {
                return L"Задание: Взломать брандмауэр и узел связи на ПК";
            }
            if (!talkedToMarkStart) {
                return L"Задание: Поговорить с Марком и сообщить о смерти семьи";
            }
            return L"Задание: Открыть скрытый ящик стола и забрать Глок-17";
        }
        else if (currentScene == 2) {
            if (!hallwayIntroPlayed) {
                return L"Задание: Осмотреться в коридоре и выслушать Марка";
            }
            return L"Задание: Уничтожить мистера Грина и забрать связку ключей";
        }
        return L"Задание: Найти выход из жилого комплекса";
    }
};
