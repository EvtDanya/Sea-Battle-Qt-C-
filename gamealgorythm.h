#pragma once
#include "SFML/System/Vector2.hpp"

#include <random>
#include <ctime>
#include <QVector>

enum class Difficulty
{
    Easy, Medium
};

enum class State
{
    Free, Ship, ShipNear, Hit, Miss, Dead
};

enum class Algorythm
{
    Diagonal
};

enum class BotMode
{
    SearchFourDeckShip, SearchThreeTwoDeckShip, SearchOneDeckShip
};

class GameAlgorythm
{  
public:
    GameAlgorythm();
    ~GameAlgorythm();

    void setDifficulty(Difficulty difficulty);
    Difficulty getDifficulty() const;

    void clearField();
    void clearAttackMatrix();
    void clearAll();

    sf::Vector2i makeMove();

    void setCellState(int i, int j, State state);

    static int random(int min, int max);

private:
    Difficulty difficulty;
    enum State field[10][10];

    bool finishOff;
    BotMode mode;

    sf::Vector2i previousStep;
    sf::Vector2i lastHit;

    Algorythm algorythm;
    QVector<QVector<int>> attackMatrix;
    int choosenPreset;

    enum class DirectionOfAttackingShip
    {
        Right, Down, Undefined
    };

    DirectionOfAttackingShip directionOfAttackingShip;

    void fillMatrix();

    bool canShoot(int i, int j) const;
    void checkMatrix();
    bool canSwitchMode() const;

    int countOfFreeCells(int i, int j) const;

    sf::Vector2i randomShoot();

};

