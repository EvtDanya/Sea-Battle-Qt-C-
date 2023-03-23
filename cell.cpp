#include "cell.h"

Cell::Cell()
{

}
Cell::Cell(int x0, int y0): x(x0),y(y0)
{
}
void Cell::setX(int x0)
{
    x = x0;
}
void Cell::setY(int y0)
{
    y = y0;
}
int Cell::getX() const
{
    return x;
}
int Cell::getY() const
{
    return y;
}
