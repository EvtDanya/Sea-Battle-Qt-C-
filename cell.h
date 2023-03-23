#pragma once
#include <SFML/Graphics.hpp>

namespace Ui
{
 class Cell;
}

class Cell
{
public:
    Cell();
    Cell(int x0, int y0);

    void setX(int x0);
    void setY(int y0);

    int getX() const;
    int getY() const;

private:
    int x;
    int y;
};

