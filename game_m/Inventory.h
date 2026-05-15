#ifndef INVENTORY_H
#define INVENTORY_H

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <iostream>

struct PlayerStats {
    float health = 100.f;
    float satiety = 100.f;
};

class Inventory {
public:
    std::map<std::string, int> items;
    std::map<std::string, sf::Texture> textures;

    inline void loadItemTexture(std::string name, std::string path) {
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            tex.setSmooth(false);
            textures[name] = tex;
        }
        else {
            std::cout << "Error loading texture for: " << name << std::endl;
        }
    }

    inline void addItem(std::string name, int count) {
        items[name] += count;
    }

    inline int getItemCount(std::string name) {
        if (items.find(name) == items.end()) {
            return 0;
        }
        return items[name];
    }


    inline void drawUI(sf::RenderWindow& window, sf::Font& font) {

        sf::RectangleShape inventoryBox(sf::Vector2f(360.f, 180.f));
        inventoryBox.setFillColor(sf::Color(20, 20, 20, 220));
        inventoryBox.setOutlineThickness(2.f);
        inventoryBox.setOutlineColor(sf::Color(255, 200, 50, 200));
        inventoryBox.setPosition(220.f, 110.f);
        window.draw(inventoryBox);


        sf::Text invTitle(L"ÈÍÂÅÍÒÀÐÜ ÅÂÛ", font, 13);
        invTitle.setFillColor(sf::Color(255, 215, 0));
        invTitle.setOrigin(invTitle.getLocalBounds().width / 2.f, 0);
        invTitle.setPosition(400.f, 125.f);
        window.draw(invTitle);

        float startSlotX = 250.f;
        float slotY = 165.f;


        for (auto it = items.begin(); it != items.end(); ++it) {
            std::string itemName = it->first;
            int itemCount = it->second;

            if (itemCount > 0) {

                sf::RectangleShape slotBorder(sf::Vector2f(42.f, 42.f));
                slotBorder.setFillColor(sf::Color(10, 10, 10, 200));
                slotBorder.setOutlineThickness(1.f);
                slotBorder.setOutlineColor(sf::Color(100, 100, 100));
                slotBorder.setPosition(startSlotX, slotY);
                window.draw(slotBorder);


                if (textures.count(itemName) > 0) {
                    sf::Sprite itemIcon;
                    itemIcon.setTexture(textures[itemName]);

                    float iconSize = 32.f;
                    itemIcon.setScale(
                        iconSize / itemIcon.getLocalBounds().width,
                        iconSize / itemIcon.getLocalBounds().height
                    );
                    itemIcon.setPosition(startSlotX + 5.f, slotY + 5.f);
                    window.draw(itemIcon);
                }


                std::wstring countWStr = std::to_wstring(static_cast<long>(itemCount));
                sf::Text countText(countWStr, font, 10);
                countText.setFillColor(sf::Color::White);
                countText.setPosition(startSlotX + 28.f, slotY + 26.f);
                window.draw(countText);

                startSlotX += 55.f;
            }
        }
    }
};

#endif
