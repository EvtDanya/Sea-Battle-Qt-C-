#pragma once

#include <QVector>

#include "ship.h"
#include "gamealgorythm.h"

namespace Ui
{
 class Fleet;
 enum class State;
}
class Fleet
{
public:
    Fleet();
    Fleet(QVector<Ship*> _ships);
    ~Fleet();

    void addShip(Ship* ship);
    void deleteShips();
    void clearField();

    void resetCells();
    void raiseShipNResetCells(int ind);
    void rotateShip(int ind);

    bool canPlaceShip(Direction direction, int size, int i, int j);
    bool canRotate(int n);
    void checkShip(int ind);

    int indOfShip(int i, int j) const;

    void randomLocation();

    QVector<sf::Vector2i> fillMissAroundShip(int ind);
    QVector<sf::Vector2i> getCoordsOfShipCells(int ind);
    QVector<Ship*> ships;
    enum State field[10][10];
    int  sizeOfFleet;
private:

};
