#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <string>
#include <vector>

using namespace sf;

struct Point {
    int x, y;
    bool isMoving;  // Whether the platform/enemy/power-up is moving
    float speed;    // Speed for moving platforms or enemies
};

int main() {
    srand(time(0));

    // Create the main game window
    RenderWindow app(VideoMode(400, 533), "Doodle Game!");
    app.setFramerateLimit(60);

    // Load textures
    Texture t1, t2, t3, t4, t5;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/platform.png");
    t3.loadFromFile("images/doodle.png");
    t4.loadFromFile("images/powerup.png");
    t5.loadFromFile("images/enemy.png");

    // Create sprites
    Sprite sBackground(t1), sPlat(t2), sPers(t3), sPowerUp(t4), sEnemy(t5);

    // Load font for text
    Font font;
    font.loadFromFile("fonts/arial.ttf");

    // Text elements for score, game over, etc.
    Text scoreText, gameOverText, restartText, levelText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::Red);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setString("GAME OVER");
    gameOverText.setPosition(100, 200);

    restartText.setFont(font);
    restartText.setCharacterSize(20);
    restartText.setFillColor(Color::Red);
    restartText.setString("Press R to Restart");
    restartText.setPosition(120, 300);

    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(Color::Red);
    levelText.setPosition(10, 40);

    // Load sounds
    SoundBuffer jumpBuffer, gameOverBuffer;
    jumpBuffer.loadFromFile("sounds/jump.wav");
    gameOverBuffer.loadFromFile("sounds/gameover.wav");

    Sound jumpSound(jumpBuffer);
    Sound gameOverSound(gameOverBuffer);

    // Initialize platforms
    std::vector<Point> plat(10);
    for (int i = 0; i < 10; i++) {
        plat[i].x = rand() % 400;
        plat[i].y = rand() % 533;
        plat[i].isMoving = rand() % 2;
        plat[i].speed = (rand() % 2 ? 1.5f : -1.5f);
    }

    // Initialize power-ups
    std::vector<Point> powerUps(3);
    for (auto &p : powerUps) {
        p.x = rand() % 400;
        p.y = rand() % 533;
    }

    // Initialize enemies
    std::vector<Point> enemies(2);
    for (auto &e : enemies) {
        e.x = rand() % 400;
        e.y = rand() % 533;
        e.speed = (rand() % 2 ? 2.0f : -2.0f);
    }

    // Player position and game state
    int x = 100, y = 100, h = 200, score = 0, level = 1;
    float dx = 0, dy = 0;
    bool isGameOver = false;

    while (app.isOpen()) {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
        }

        // Game over handling
        if (isGameOver) {
            app.clear();
            app.draw(sBackground);
            app.draw(gameOverText);
            app.draw(restartText);
            app.display();

            if (Keyboard::isKeyPressed(Keyboard::R)) {
                // Reset game state
                x = 100; y = 100; score = 0; level = 1; dy = 0; isGameOver = false;
                for (auto &p : plat) {
                    p.x = rand() % 400;
                    p.y = rand() % 533;
                    p.isMoving = rand() % 2;
                    p.speed = (rand() % 2 ? 1.5f : -1.5f);
                }
            }
            continue;
        }

        // Player movement
        if (Keyboard::isKeyPressed(Keyboard::Right)) x += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left)) x -= 3;

        // Gravity
        dy += 0.2;
        y += dy;

        // Check if player falls
        if (y > 533) {
            isGameOver = true;
            gameOverSound.play();
            continue;
        }

        // Update platforms and score
        if (y < h) {
            y = h;
            for (auto &p : plat) {
                p.y -= dy;
                if (p.y > 533) {
                    p.y = 0;
                    p.x = rand() % 400;
                    p.isMoving = rand() % 2;
                    p.speed = (rand() % 2 ? 1.5f : -1.5f);
                    score++;
                }
            }
            for (auto &p : powerUps) {
                p.y -= dy;
                if (p.y > 533) {
                    p.y = rand() % 533;
                    p.x = rand() % 400;
                }
            }
            for (auto &e : enemies) {
                e.y -= dy;
                if (e.y > 533) {
                    e.y = rand() % 533;
                    e.x = rand() % 400;
                }
            }
        }

        // Update level
        level = score / 10 + 1;
        levelText.setString("Level: " + std::to_string(level));

        // Platform collision
        for (auto &p : plat) {
            if ((x + 50 > p.x) && (x + 20 < p.x + 68)
                && (y + 70 > p.y) && (y + 70 < p.y + 14) && (dy > 0)) {
                dy = -10;
                jumpSound.play();
            }
            if (p.isMoving) {
                p.x += p.speed;
                if (p.x < 0 || p.x > 332) p.speed *= -1;
            }
        }

        // Power-up collision
        for (auto &p : powerUps) {
            if ((x + 50 > p.x) && (x + 20 < p.x + 32)
                && (y + 70 > p.y) && (y + 70 < p.y + 32)) {
                score += 5; // Increase score
                p.y = -50;  // Move power-up off screen
            }
        }

        // Enemy collision
        for (auto &e : enemies) {
            e.x += e.speed;
            if (e.x < 0 || e.x > 368) e.speed *= -1;
            if ((x + 50 > e.x) && (x + 20 < e.x + 32)
                && (y + 70 > e.y) && (y + 70 < e.y + 32)) {
                isGameOver = true;
                gameOverSound.play();
            }
        }

        // Render background and sprites
        app.clear();
        app.draw(sBackground);
        sPers.setPosition(x, y);
        app.draw(sPers);

        // Render platforms
        for (const auto &p : plat) {
            sPlat.setPosition(p.x, p.y);
            app.draw(sPlat);
        }

        // Render power-ups
        for (const auto &p : powerUps) {
            sPowerUp.setPosition(p.x, p.y);
            app.draw(sPowerUp);
        }

        // Render enemies
        for (const auto &e : enemies) {
            sEnemy.setPosition(e.x, e.y);
            app.draw(sEnemy);
        }

        // Render score and level
        scoreText.setString("Score: " + std::to_string(score));
        app.draw(scoreText);
        app.draw(levelText);

        app.display();
    }

    return 0;
}
