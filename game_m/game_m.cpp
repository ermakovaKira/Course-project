#include <SFML/Graphics.hpp>

using namespace sf;
void KeyBoard_Control(float time, Sprite& s, float& currentFrame, float speed, int w, int h);



int main(){

    RenderWindow window(VideoMode(800, 400), "Test");
    window.setFramerateLimit(60);

    Texture t;
    t.setSmooth(true);
    t.loadFromFile("sprite_main.png");
    if (!t.loadFromFile("sprite_main.png")) {
        return -1; 
    }

    Sprite s;
    s.setTexture(t);
    int w = 150;
    int h = 236;
    s.setTextureRect(IntRect(0, 0, w, h));
    s.setPosition(50, 100);
    s.setScale(0.5f, 0.5f);

    float currentFrame = 0;
    Clock clock;
    float speed = 0.1;
    while (window.isOpen())
    {
        float time = (clock.getElapsedTime().asMicroseconds()) / 700.0f;
        clock.restart();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        KeyBoard_Control(time, s, currentFrame, speed, w, h);

        window.clear(Color::White);
        window.draw(s);
        window.display();
    }

    return 0;
}

void KeyBoard_Control(float time, Sprite& s, float& currentFrame, float speed, int w, int h) {
    bool isMoving = false;
    float animSpeed = 0.004f;

    if (Keyboard::isKeyPressed(Keyboard::A)) {
        isMoving = true;
        s.move(-speed * time, 0);
        currentFrame += animSpeed * time;
        if (currentFrame >= 4) currentFrame = 0;
        s.setTextureRect(IntRect(w * int(currentFrame) + w, 0, -w, h));
    }
    else if (Keyboard::isKeyPressed(Keyboard::D)) {
        isMoving = true;
        s.move(speed * time, 0);
        currentFrame += animSpeed * time;
        if (currentFrame >= 4) currentFrame = 0;
        s.setTextureRect(IntRect(w * int(currentFrame), 0, w, h));
    }

    if (!isMoving) {

        if (currentFrame > 0) currentFrame -= 0.002f * time;
        if (currentFrame < 0) currentFrame = 0;
    }
}