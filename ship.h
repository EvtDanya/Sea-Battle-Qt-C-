#pragma once
#include "cell.h"
#include <QVector>

extern int cell_size;
extern double shipScale;

namespace Ui
{
 class Ship;
}

enum class Direction
{
    Down, Right
};

class Ship
{
public:
    Ship();
    Ship(int x, int y, int size, Direction dir);
    ~Ship();

    QVector<Cell*> cells;

    void setTexture(std::string str);
    void setPosition(sf::Vector2i newCoords);
    void setColor(sf::Color color);
    void setDirection(Direction dir);


    void resetCells(sf::Vector2i cell);
    void resetPosition();
    void resetPositionWithOffset(int offset);
    void resetWithOffsetAllDirections(int offset);

    void rotate();
    void rotateSprite();

    void placeShipNearCursor(int x, int y);

    sf::Sprite getSprite() const;
    int getSize() const;
    Direction getDirection() const;
    sf::Vector2i getPosition() const;

    void setAlive(bool _alive);
    bool isAlive() const;
private:
    bool alive;
    Direction direction;
    sf::Texture texture;
    sf::Sprite sprite;
};

