#ifndef STORY_MANAGER_H
#define STORY_MANAGER_H

#include "Config.h" // <--- ÏÎÄÊËÞ×ÀÅÌ ÒÓÒ

class StoryManager {
public:
    Difficulty difficulty;
    int currentScene;
    bool readLaptopEmail;
    bool talkedToMarkStart;
    bool studentSaved;
    bool markIsWounded;
    int dataFragmentsCollected;

    StoryManager(Difficulty diff = NORMAL) {
        difficulty = diff;
        currentScene = 1;
        readLaptopEmail = false;
        talkedToMarkStart = false;
        studentSaved = false;
        markIsWounded = false;
        dataFragmentsCollected = 0;
    }

    bool checkGoodEndingConditions() {
        int requiredFragments = (difficulty == EASY) ? 2 : 3;
        return (dataFragmentsCollected >= requiredFragments && !markIsWounded);
    }
};

#endif
