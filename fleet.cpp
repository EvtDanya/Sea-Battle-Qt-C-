#include "fleet.h"

Fleet::Fleet():sizeOfFleet(10)
{
    this->clearField();
}

Fleet::Fleet(QVector<Ship *> _ships): ships(_ships), sizeOfFleet(10)
{

}

Fleet::~Fleet()
{
    this->deleteShips();
}

void Fleet::addShip(Ship *ship)
{
    this->ships.push_back(ship);
    this->resetCells();
}

void Fleet::deleteShips()
{
   for (auto ship : ships)
   {
        delete ship;
   }
   this->ships.clear();
}

void Fleet::resetCells()
{
    for (auto ship : ships)
    {
        for (auto cell : ship->cells)
        {
            this->field[cell->getX()][cell->getY()] = State::Ship;
            for (auto i = -1; i < 2; i++)
                for (auto j = -1; j < 2; j++)
                {
                    if (cell->getX() + i >= 0 && cell->getX() + i < 10 &&
                            cell->getY() + j >= 0 && cell->getY() + j < 10)
                        if (field[cell->getX() + i][cell->getY() + j]!=State::Ship
                                && field[cell->getX() + i][cell->getY() + j]!=State::Miss)
                            field[cell->getX() + i][cell->getY() + j]=State::ShipNear;
                }
        }
    }
}

void Fleet::raiseShipNResetCells(int ind)
{
    for (auto n = 0; n < ships.size(); n++)
    {
        if (n == ind)
            continue;

        for (auto cell : ships[n]->cells)
        {
            this->field[cell->getX()][cell->getY()] = State::Ship;
            for (auto i = -1; i < 2; i++)
                for (auto j = -1; j < 2; j++)
                {
                    if (cell->getX() + i >= 0 && cell->getX() + i < 10 &&
                            cell->getY() + j >= 0 && cell->getY() + j < 10)
                        if (field[cell->getX() + i][cell->getY() + j]!=State::Ship)
                            field[cell->getX() + i][cell->getY() + j]=State::ShipNear;
                }
        }
    }
}

void Fleet::rotateShip(int ind)
{
    ships[ind]->rotate();
}

bool Fleet::canPlaceShip(Direction direction, int size, int i, int j)
{
    switch (direction) {
    case Direction::Down:
    {
        for (auto ind = 0, step = 0; ind < size; ind++, step++)
        {
            if (j+step > 9)
                return false;

            if (field[i][j + step] == State::Ship || field[i][j + step] == State::ShipNear)
                return false;
        }
        break;
    }
    case Direction::Right:
    {
        for (auto ind = 0, step = 0; ind < size; ind++, step++)
        {
            if (i + step > 9)
                return false;

            if (field[i + step][j] == State::Ship || field[i + step][j] == State::ShipNear)
                return false;
        }
        break;
    }
    default:
        break;
    }

    return true;
}

bool Fleet::canRotate(int n)
{
    this->clearField();
    this->raiseShipNResetCells(n);

    switch (ships[n]->getDirection()) {
    case Direction::Down:
    {
        for (auto ind = 0, step = 0; ind < ships[n]->cells.size(); ind++, step++)
        {
            if (ships[n]->cells[0]->getX() + step > 9)
                return false;

            if (field[ships[n]->cells[0]->getX() + step][ships[n]->cells[0]->getY()] == State::Ship ||
                    field[ships[n]->cells[0]->getX() + step][ships[n]->cells[0]->getY()] == State::ShipNear)
                return false;
        }
        break;
    }
    case Direction::Right:
    {

        for (auto ind = 0, step = 0; ind < ships[n]->cells.size(); ind++, step++)
        {
            if (ships[n]->cells[0]->getY() + step > 9)
                return false;

            if (field[ships[n]->cells[0]->getX()][ships[n]->cells[0]->getY() + step] == State::Ship ||
                    field[ships[n]->cells[0]->getX()][ships[n]->cells[0]->getY() + step] == State::ShipNear)
                return false;
        }
        break;
    }
    default:
        break;
    }
    return true;
}

void Fleet::checkShip(int ind)
{
    for (auto cell : this->ships[ind]->cells)
        if (field[cell->getX()][cell->getY()] == State::Ship)
            return ;
    this->ships[ind]->setAlive(false);
}

int Fleet::indOfShip(int i, int j) const
{
    for (auto ind = 0; ind < ships.size(); ind++)
        for (auto cell : ships[ind]->cells)
            if (cell->getX() == i && cell->getY() == j)
                return ind;
    return -1;
}

void Fleet::randomLocation()
{
        for (auto count = 1; count < 5; count++)
            for (auto n = 0; n < count; n++)
                while (1)
                {
                    int i = GameAlgorythm::random(0,9);
                    int j = GameAlgorythm::random(0,9);
                    Direction dir;
                    int d = GameAlgorythm::random(0,1);
                    if (d == 0)
                        dir = Direction::Right;
                    else
                        dir = Direction::Down;

                    if (this->canPlaceShip(dir,5-count,i,j))
                    {
                        this->addShip(new Ship(i,j,5-count,dir));
                        this->ships[this->ships.size()-1]->resetPosition();

                        break;
                    }
                }
}

QVector<sf::Vector2i> Fleet::fillMissAroundShip(int ind)
{
    QVector<sf::Vector2i> redCells;
    for (auto cell : this->ships[ind]->cells)
    {
        for (auto i = -1; i < 2; i++)
            for (auto j = -1; j < 2; j++)
            {
                if (cell->getX() + i >= 0 && cell->getX() + i < 10 &&
                        cell->getY() + j >= 0 && cell->getY() + j < 10)
                    if (field[cell->getX() + i][cell->getY() + j]!=State::Miss &&
                            field[cell->getX() + i][cell->getY() + j]!=State::Ship &&
                                field[cell->getX() + i][cell->getY() + j]!=State::Hit)
                    {
                        field[cell->getX() + i][cell->getY() + j]=State::Miss;
                        redCells.push_back(sf::Vector2i(cell->getX() + i,cell->getY() + j));
                    }
            }
    }
    return redCells;
}

QVector<sf::Vector2i> Fleet::getCoordsOfShipCells(int ind)
{
    QVector<sf::Vector2i> cellsCoords;
    for (auto cell : this->ships[ind]->cells)
    {
        cellsCoords.push_back(sf::Vector2i(cell->getX(), cell->getY()));
    }
    return cellsCoords;
}

void Fleet::clearField()
{
    for (auto i = 0; i < 10; i++)
        for (auto j = 0; j< 10; j++)
        {
            field[i][j] = State::Free;
        }
}

