#ifndef DIALOGUE_DATABASE_H
#define DIALOGUE_DATABASE_H

#include "DialogueSystem.h"
#include <map>
#include <string>
#include <vector>

class DialogueDatabase {
private:
    sf::Texture evaPortrait;
    sf::Texture markPortrait;

public:
    DialogueDatabase() {
        evaPortrait.loadFromFile("eva_face.png");
        markPortrait.loadFromFile("mark_face.png");
    }

    std::vector<DialogueLine> getDialogue(std::string dialogueID) {
        std::vector<DialogueLine> lines;

        if (dialogueID == "apartment_pc") {
            lines = {
                { L"СИСТЕМА: Доступ к городскому узлу связи получен. Дешифровка логов...", nullptr },
                { L"ЕВА: Так... Сектор 7, улица Ленина... Нет. Нет-нет-нет!", &evaPortrait },
                { L"СИСТЕМА: Сектор зачищен. Статус гражданского населения: ЛИКВИДИРОВАНЫ.", nullptr },
                { L"ЕВА: Мама... Папа... Их больше нет. Это был не вирус. Их просто казнили...", &evaPortrait }
            };
        }
        else if (dialogueID == "apartment_mark_before_pc") {
            lines = {
                { L"МАРК: Ева, ты смогла пробиться через их брандмауэр? Узнай, что с родителями.", &markPortrait }
            };
        }
        else if (dialogueID == "apartment_mark_after_pc") {
            lines = {
                { L"МАРК: Ева... Ты вся дрожишь. Что там?", &markPortrait },
                { L"ЕВА: Они убили их, Марк. Всех. Весь сектор.", &evaPortrait },
                { L"МАРК: Боже... Мне жаль, Ева. Но не смотри на экран. Мы должны уйти сейчас, или останемся в этом склепе навсегда.", &markPortrait },
                { L"МАРК: Я собрал свою медицинскую сумку. Возьми Глок из ящика стола, проверишь фото на тумбочке — и уходим через левую дверь.", &markPortrait }
            };
        }
        else if (dialogueID == "inspect_photo") {
            lines = {
                { L"ЕВА: Мой последний день рождения... Вы улыбаетесь. Я заставлю эту группировку ответить за каждую каплю крови.", &evaPortrait }
            };
        }
        else if (dialogueID == "inspect_drawer_locked") {
            lines = {
                { L"ЕВА: Ящик заклинило. Нужно сначала успокоиться и поговорить с Марком.", &evaPortrait }
            };
        }
        else if (dialogueID == "inspect_drawer_open") {
            lines = {
                { L"ЕВА: Старый отцовский Глок-17. Тяжелый. Надеюсь, мне хватит духу нажать на курок.", &evaPortrait }
            };
        }

        return lines;
    }
};

#endif
