#include "ship.h"

int cell_size = 40;
double shipScale = 1.5;

Ship::Ship():alive(true)
{

}

Ship::Ship(int x, int y, int _size, Direction dir):alive(true)
{
    switch(_size)
    {
    case 1:
        this->texture.loadFromFile("images\\ship1.png");
        break;
    case 2:
        this->texture.loadFromFile("images\\ship2.png");
        break;
    case 3:
        this->texture.loadFromFile("images\\ship3.png");
        break;
    case 4:
        this->texture.loadFromFile("images\\ship4.png");
        break;
    default:
        break;
    }
    this->sprite.setTexture(texture);
    this->sprite.setScale(shipScale,shipScale);
    this->sprite.setPosition(cell_size*x+cell_size+1,cell_size*y+1);
    direction = dir;
    switch (dir) {
    case Direction::Down:
    {
        for (auto j = 0; _size > 0; _size--, j++)
        {
            this->cells.push_back(new Cell(x,y+j));
        }
        this->rotateSprite();
        break;
    }
    case Direction::Right:
    {
        for (auto i = 0; _size > 0; _size--, i++)
        {
            this->cells.push_back(new Cell(x+i,y));
        }
        break;
    }
    default:
        break;
    }
}

Ship::~Ship()
{
    for (auto cell : cells)
      {
        delete cell;
      }
      cells.clear();
}

void Ship::setTexture(std::string str)
{
    this->texture.loadFromFile(str);
    this->sprite.setTexture(texture);
}

sf::Sprite Ship::getSprite() const
{
    return this->sprite;
}

void Ship::setPosition(sf::Vector2i newCoords)
{
    this->sprite.setPosition(newCoords.x, newCoords.y);
}

void Ship::setColor(sf::Color color)
{
    sprite.setColor(color);
}

void Ship::setDirection(Direction dir)
{
    this->direction = dir;
}

void Ship::resetCells(sf::Vector2i cell)
{
    switch (direction) {
    case Direction::Down:
    {
        for (auto j = 0, ind = 0; ind < this->cells.size(); ind++, j++)
        {
            delete cells[ind];
            this->cells[ind] = new Cell(cell.x,cell.y+j);
        }
        break;
    }
    case Direction::Right:
    {
        for (auto i = 0, ind = 0; ind < this->cells.size(); ind++, i++)
        {
            delete cells[ind];
            this->cells[ind] = new Cell(cell.x+i,cell.y);
        }
        break;
    }
    default:
        break;
    }
}

void Ship::resetPosition()
{
    switch (this->direction) {
    case Direction::Down:
        this->sprite.setPosition((cells[0]->getX()+1)*cell_size+1,cells[0]->getY()*cell_size+1);
        break;
    case Direction::Right:
        this->sprite.setPosition(cells[0]->getX()*cell_size+1,cells[0]->getY()*cell_size+1);
        break;
    default:
        break;
    }
}

int Ship::getSize() const
{
    return this->cells.size();
}

Direction Ship::getDirection() const
{
    return direction;
}

sf::Vector2i Ship::getPosition() const
{
    return sf::Vector2i(this->cells.at(0)->getX(),this->cells.at(0)->getY());
}

void Ship::setAlive(bool _alive)
{
   this->alive = _alive;
}

bool Ship::isAlive() const
{
    return alive;
}

void Ship::rotate()
{
    switch (this->direction) {
    case Direction::Down:
    {
        this->direction = Direction::Right;

        this->rotateSprite();

        this->resetCells(sf::Vector2i(cells[0]->getX(),cells[0]->getY()));
        break;
    }
    case Direction::Right:
    {
        this->direction = Direction::Down;

        this->rotateSprite();

        this->resetCells(sf::Vector2i(cells[0]->getX(),cells[0]->getY()));
        break;
    }
    default:
        break;
    }

}

void Ship::rotateSprite()
{
    switch (this->direction) {
    case Direction::Right:
    {
        this->sprite.setRotation(0.f);
        //this->sprite.setPosition(cells[0]->getX()*cell_size+1,cells[0]->getY()*cell_size+1);//correct position of sprite
        this->sprite.setPosition(this->sprite.getPosition().x-cell_size,this->sprite.getPosition().y);
        break;
    }
    case Direction::Down:
    {
        this->sprite.setRotation(90.f);
        //this->sprite.setPosition((cells[0]->getX()+1)*cell_size+1,cells[0]->getY()*cell_size+1);//correct position of sprite
        this->sprite.setPosition(this->sprite.getPosition().x+cell_size,this->sprite.getPosition().y);
        break;
    }
    default:
        break;
    }
}

void Ship::placeShipNearCursor(int x, int y)
{
    switch (this->direction) {
    case Direction::Down:
    {
        this->setPosition(sf::Vector2i(x+18,y-15));
        break;
    }
    case Direction::Right:
    {
        this->setPosition(sf::Vector2i(x-15,y-18));
        break;
    }
    default:
        break;
    }
}

void Ship::resetPositionWithOffset(int offset)
{
    switch (this->direction) {
    case Direction::Down:
        this->sprite.setPosition((cells[0]->getX()+1)*cell_size+1+offset,cells[0]->getY()*cell_size+1);
        break;
    case Direction::Right:
        this->sprite.setPosition(cells[0]->getX()*cell_size+1+offset,cells[0]->getY()*cell_size+1);
        break;
    default:
        break;
    }
}

void Ship::resetWithOffsetAllDirections(int offset)
{
    switch (this->direction) {
    case Direction::Down:
        this->sprite.setPosition((cells[0]->getX()+1)*cell_size+1+offset,cells[0]->getY()*cell_size+1+offset);
        break;
    case Direction::Right:
        this->sprite.setPosition(cells[0]->getX()*cell_size+1+offset,cells[0]->getY()*cell_size+1+offset);
        break;
    default:
        break;
    }
}
