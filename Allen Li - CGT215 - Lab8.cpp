#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "duck hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "Lab08Images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "Lab08Images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(790, 300));
    right.setStatic(false);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "Lab08Images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };


    Font fnt;
    if (!fnt.loadFromFile("arial1.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    long duckMS(0);
    long creationinterval = 1000;
    int space = 50;
    long creationtime = creationinterval;
    int currentspace = space;
    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        duckMS = duckMS + deltaMS;
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            ducks.DoRemovals();
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }
            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            scoreText.setPosition(Vector2f(0 + GetTextSize(scoreText).x, 550));
            window.draw(scoreText);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            arrowCountText.setPosition(Vector2f(790 - GetTextSize(arrowCountText).x, 550));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();

        }
        if (duckMS - creationtime >= creationinterval) {
            duckMS = 0;
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(redTex);
                Vector2f sz = duck.getSize();
                duck.setCenter(Vector2f(50, 20 + (sz.y / 2)));
                duck.setVelocity(Vector2f(0.5, 0));
                world.AddPhysicsBody(duck);
                duck.onCollision =
                    [&drawingArrow, &world, &arrow, &duck, &ducks, &score, &right]
                    (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                    if (result.object2 == right) {
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                    }
                    };
                creationtime = duckMS;

                creationtime += currentspace;
        }
    }
    window.display(); // this is needed to see the last frame
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (true);

}
