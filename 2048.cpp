#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>

using namespace sf;
using namespace std;

enum class GameState {
    StartScreen,
    Playing,
    Win,
    GameOver
};

using Grid = array<array<int, 4>, 4>;

// grid utilities

int squash_row(array<int, 4>& row) {
    array<int, 4> temp = {0, 0, 0, 0};
    int idx = 0;
    int scoreGained = 0;

    // compact
    for (int i = 0; i < 4; ++i) {
        if (row[i] != 0) {
            temp[idx++] = row[i];
        }
    }

    // merge
    for (int i = 0; i < 3; ++i) {
        if (temp[i] != 0 && temp[i] == temp[i + 1]) {
            temp[i] *= 2;
            scoreGained += temp[i]; // <-- Add to score
            temp[i + 1] = 0;
        }
    }

    // recompact
    idx = 0;
    for (int i = 0; i < 4; ++i) {
        if (temp[i] != 0) {
            row[idx++] = temp[i];
        }
    }

    // zero-fill
    while (idx < 4) {
        row[idx++] = 0;
    }

    return scoreGained;
}



void rotate_clockwise(Grid& grid) {
    Grid temp = grid;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            grid[x][3 - y] = temp[y][x];
}

void rotate_counterclockwise(Grid& grid) {
    Grid temp = grid;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            grid[3 - x][y] = temp[y][x];
}

void rotate_180(Grid& grid) {
    rotate_clockwise(grid);
    rotate_clockwise(grid);
}

int squash(Grid& grid, Keyboard::Key key) {
    int totalPoints = 0;

    switch (key) {
        case Keyboard::Key::Up:
            for (int x = 0; x < 4; ++x) {
                array<int, 4> col;
                for (int y = 0; y < 4; ++y)
                    col[y] = grid[y][x];

                totalPoints += squash_row(col);

                for (int y = 0; y < 4; ++y)
                    grid[y][x] = col[y];
            }
            break;

        case Keyboard::Key::Down:
            for (int x = 0; x < 4; ++x) {
                array<int, 4> col;
                for (int y = 0; y < 4; ++y)
                    col[y] = grid[3 - y][x];

                totalPoints += squash_row(col);

                for (int y = 0; y < 4; ++y)
                    grid[3 - y][x] = col[y];
            }
            break;

        case Keyboard::Key::Left:
            for (int y = 0; y < 4; ++y)
                totalPoints += squash_row(grid[y]);
            break;

        case Keyboard::Key::Right:
            for (int y = 0; y < 4; ++y) {
                array<int, 4> row = grid[y];
                std::reverse(row.begin(), row.end());

                totalPoints += squash_row(row);

                std::reverse(row.begin(), row.end());
                grid[y] = row;
            }
            break;

        default:
            break;
    }

    return totalPoints;
}



bool has_won(const Grid& grid) {
    for (const auto& row : grid)
        for (int val : row)
            if (val == 2048)
                return true;
    return false;
}

bool has_moves(const Grid& grid) {
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (grid[y][x] == 0) return true;
            if (x < 3 && grid[y][x] == grid[y][x + 1]) return true;
            if (y < 3 && grid[y][x] == grid[y + 1][x]) return true;
        }
    }
    return false;
}

void add_random_tile(Grid& grid) {
    vector<pair<int, int>> empty;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            if (grid[y][x] == 0)
                empty.emplace_back(y, x);

    if (!empty.empty()) {
        auto [y, x] = empty[rand() % empty.size()];
        grid[y][x] = (rand() % 10 == 0) ? 4 : 2;
    }
}
    class GameStats {
        public:
            virtual void displayStats() const = 0; // Pure virtual function (makes this an abstract class)
        };
        class HighScore : public GameStats {
            private:
                int highScore;
            protected:
                int currentScore;    
            public:
                HighScore() : highScore(0), currentScore(0) {}
                void resetScore() {
                    currentScore = 0;
                }
                void addPoints(int points) {
                    currentScore += points;
                    if (currentScore > highScore) {
                        highScore = currentScore;
                    }
                }
                // Overloaded version: add with bonus multiplier
                void addPoints(int points, float multiplier) {
                    int total = static_cast<int>(points * multiplier);
                    addPoints(total);  // reuse original
                }
                int getCurrentScore() const {
                    return currentScore;
                }
                int getHighScore() const {
                    return highScore;
                }
                void displayScores() const {
                    cout << "Current Score: " << currentScore << ", High Score: " << highScore << endl;
                }
            
                // Implementation of pure virtual function
                void displayStats() const override {
                    displayScores();
                }
            };

            class MoveCounter : public GameStats {
                private:
                    int moveCount;       
                public:
                    MoveCounter() : moveCount(0) {}
                    void increment() {
                        ++moveCount;
                    }
                    void reset() {
                        moveCount = 0;
                    }
                    int getMoves() const {
                        return moveCount;
                    }
                    // override pure virtual function
                    void displayStats() const override {
                        cout << "Moves: " << moveCount << endl;
                    }
                };
// main

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    try {
        RenderWindow window(sf::VideoMode(sf::Vector2u(800u, 800u)), std::string("2048"));
        
        Font font;
        if (!font.openFromFile("arial.ttf")) {
            throw runtime_error("Failed to load font!");
        }

    Grid grid = {};
    unique_ptr<HighScore> scoreManager = make_unique<HighScore>();
    unique_ptr<MoveCounter> moveCounter = make_unique<MoveCounter>();   

    add_random_tile(grid);
    add_random_tile(grid);

    scoreManager->addPoints(4); // Example: simulate score increase after tile merge
    scoreManager->displayScores(); // Optional: display in console for now
    GameState gameState = GameState::StartScreen;

    while (window.isOpen())
    {
        if (gameState == GameState::StartScreen) {
            window.clear(Color(250, 215, 215));
        
            sf::Text title(font, "2048", 72);  // Initialize with font, text, and size
            title.setFillColor(Color::Black);
            title.setPosition(sf::Vector2f(280.f, 200.f));
            
        
            sf::Text instruction(font, "Press Enter to Start", 32);  // Initialize with font, text, and size
            instruction.setFillColor(Color::Black);
            instruction.setPosition(sf::Vector2f(240.f, 400.f));
        
            window.draw(title);
            window.draw(instruction);  
            window.display();      
        
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == Keyboard::Key::Enter) {
                    gameState = GameState::Playing;  // Transition to Playing state
                    break;  // Exit the loop to start playing the game
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                window.close();
            }
        }
        continue;
        }
        if (gameState == GameState::Playing) {
            window.clear(Color(250, 215, 215)); // Clear screen for the game logic

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();

                else if (keyPressed->scancode == sf::Keyboard::Scancode::W || keyPressed->scancode == sf::Keyboard::Scancode::Up ||
                    keyPressed->scancode == sf::Keyboard::Scancode::A || keyPressed->scancode == sf::Keyboard::Scancode::Left ||
                    keyPressed->scancode == sf::Keyboard::Scancode::S || keyPressed->scancode == sf::Keyboard::Scancode::Down ||
                    keyPressed->scancode == sf::Keyboard::Scancode::D || keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                        sf::Keyboard::Key key = keyPressed->code;
                        
                        Grid old = grid;
                        int pointsEarned = squash(grid, key);
                        
                        if (grid != old) {
                            add_random_tile(grid);
                        
                            // Use bonus for higher merges (e.g., big merge → higher multiplier)
                            if (pointsEarned >= 64)
                                scoreManager.addPoints(pointsEarned, 1.5f); // bonus multiplier
                            else
                                scoreManager.addPoints(pointsEarned);       // normal scoring
                        
                            scoreManager.displayScores();
                        }
                         
                        
                    if (has_won(grid) && gameState == GameState::Playing) {
                        gameState = GameState::Win;
                    }                                   
                    }
                }
            }

        float tileSize = 180.f;
        float padding = 20.f;
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                RectangleShape tile(Vector2f(tileSize, tileSize));
                tile.setFillColor(Color(235, 183, 183));
                tile.setPosition(sf::Vector2f(
                    static_cast<float>(padding + x * (tileSize + padding)),
                    static_cast<float>(padding + y * (tileSize + padding))
                ));                
                window.draw(tile);

                if (grid[y][x] != 0) {
                    sf::Text numberText(font, std::to_string(grid[y][x]), 36);
                    numberText.setFillColor(Color::White);
                    // numberText.setString(to_string(grid[y][x]));

                    sf::FloatRect textRect = numberText.getLocalBounds();
                    sf::Vector2f origin = textRect.position + (textRect.size / 2.0f);
                    numberText.setOrigin(origin);                    
                    sf::Vector2f position = tile.getPosition() + sf::Vector2f(tileSize / 2.f, tileSize / 2.f);
                    numberText.setPosition(position);
                    window.draw(numberText);
                }
            }
        }
        // display scores
            sf::Text scoreText(font, "", 28);  // Initialize with font and size, empty string initially
            scoreText.setFillColor(sf::Color::White); // Set text color if needed
            std::string scoreString = 
                "Score: " + std::to_string(scoreManager.getCurrentScore()) + 
                "    High Score: " + std::to_string(scoreManager.getHighScore());
        
            scoreText.setString(scoreString);
            scoreText.setPosition(sf::Vector2f(20.f, 10.f)); // Top-left corner
        
            window.draw(scoreText);
       

        if (has_won(grid)) {
            cout << "You won!" << endl;
        }

        if (!has_moves(grid) && gameState == GameState::Playing) {
            gameState = GameState::GameOver;
        }
        
        if (gameState == GameState::GameOver) {
            sf::Text gameOverText(font, "Game Over!", 48);  // Initialize with font, text, and size
            gameOverText.setFillColor(sf::Color::Red);  // Optional: Set color            
            gameOverText.setFillColor(Color::Red);
            gameOverText.setPosition(sf::Vector2f(260.f, 350.f));
            window.draw(gameOverText);
        }

        if (gameState == GameState::Win) {
            sf::Text winText(font, "You Win!", 60);  // Initialize with font, text, and size
            winText.setFillColor(Color::Green);
            winText.setPosition(sf::Vector2f(260.f, 300.f));
            window.draw(winText);
        
            continue; // skipping rest of the loop while on win screen
        }
        window.display();
    }
    }
    
} catch (const exception& e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
}

    return 0;
}
