#include "gamealgorythm.h"
#include "presets.cpp"

GameAlgorythm::GameAlgorythm()
{
    this->choosenPreset = GameAlgorythm::random(0,1);
    this->clearAll();
}

GameAlgorythm::~GameAlgorythm()
{

}

void GameAlgorythm::setDifficulty(Difficulty difficulty)
{
    this->difficulty = difficulty;
}

Difficulty GameAlgorythm::getDifficulty() const
{
    return this->difficulty;
}

void GameAlgorythm::clearField()
{
    for (auto i = 0; i < 10; i++)
        for (auto j = 0; j < 10; j++)
            this->field[i][j] = State::Free;
}

void GameAlgorythm::clearAttackMatrix()
{
    this->attackMatrix = clearMatrix;
}

void GameAlgorythm::clearAll()
{
    this->clearAttackMatrix();
    this->clearField();

    this->algorythm = Algorythm::Diagonal;
    this->directionOfAttackingShip = DirectionOfAttackingShip::Undefined;
    this->previousStep = sf::Vector2i(-1,-1);
    this->lastHit = sf::Vector2i(-1,-1);
    this->finishOff = false;
    this->mode = BotMode::SearchFourDeckShip;
    this->fillMatrix();
}

sf::Vector2i GameAlgorythm::makeMove()
{
    sf::Vector2i move(0,0);

    switch (this->difficulty) {
    case Difficulty::Easy:
    {
        move = this->randomShoot();
        break;
    }
    case Difficulty::Medium:
    {

        if (this->previousStep != sf::Vector2i(-1,-1))
        {
            if (this->field[this->previousStep.x][this->previousStep.y] == State::Hit)
            {

                if (this->previousStep!=this->lastHit && this->lastHit != sf::Vector2i(-1,-1))
                {
                    if (this->previousStep.x == this->lastHit.x)
                        this->directionOfAttackingShip = DirectionOfAttackingShip::Down;

                    else if (this->previousStep.y == this->lastHit.y)
                        this->directionOfAttackingShip = DirectionOfAttackingShip::Right;
                }

                this->lastHit = this->previousStep;
                this->finishOff = true;
            }

            if (this->field[this->previousStep.x][this->previousStep.y] == State::Dead)
            {
                this->finishOff = false;
                this->directionOfAttackingShip = DirectionOfAttackingShip::Undefined;
                this->lastHit = sf::Vector2i(-1,-1);
            }
        }

        if (this->finishOff)
        {
            if (this->directionOfAttackingShip == DirectionOfAttackingShip::Undefined)
            {
                    QVector<QVector<int>> counts;

                    if (this->lastHit.x-1 >= 0)
                        if (this->field[this->lastHit.x-1][this->lastHit.y] == State::Free)
                            counts.push_back({this->lastHit.x-1,this->lastHit.y,this->countOfFreeCells(this->lastHit.x-1,this->lastHit.y)});

                    if (this->lastHit.y-1 >= 0)
                        if (this->field[this->lastHit.x][this->lastHit.y-1] == State::Free)
                            counts.push_back({this->lastHit.x,this->lastHit.y-1,this->countOfFreeCells(this->lastHit.x,this->lastHit.y-1)});

                    if (this->lastHit.x+1 < 10)
                        if (this->field[this->lastHit.x+1][this->lastHit.y] == State::Free)
                            counts.push_back({this->lastHit.x+1,this->lastHit.y,this->countOfFreeCells(this->lastHit.x+1,this->lastHit.y)});

                    if (this->lastHit.y+1 < 10)
                        if (this->field[this->lastHit.x][this->lastHit.y+1] == State::Free)
                            counts.push_back({this->lastHit.x,this->lastHit.y+1,this->countOfFreeCells(this->lastHit.x,this->lastHit.y+1)});

                    if (this->lastHit.x == 0 || this->lastHit.x == 9 || this->lastHit.y == 0 || this->lastHit.y == 9)
                    {
                          int rand = this->random(0, counts.size()-1);
                          this->previousStep = sf::Vector2i(counts.at(rand).at(0),counts.at(rand).at(1));
                          move = sf::Vector2i(counts.at(rand).at(0),counts.at(rand).at(1));
                    }
                    else
                    {
                        int max = 0;

                        for (auto i = 0; i < counts.size(); i++)
                        {
                            if (counts.at(i).at(2) > counts.at(max).at(2))
                                max = i;
                            else if (counts.at(i).at(2) == counts.at(max).at(2))
                            {
                                if (random(0,1) == 0)  max = i;
                            }
                        }
                        this->previousStep = sf::Vector2i(counts.at(max).at(0),counts.at(max).at(1));
                        move = sf::Vector2i(counts.at(max).at(0),counts.at(max).at(1));
                    }

            }
            else
            {
                if (this->directionOfAttackingShip == DirectionOfAttackingShip::Right)
                {
                    QVector<QVector<int>> counts;
                    for (auto i = this->lastHit.x - 1; i >= 0; i--)
                    {
                        if (this->field[i][this->lastHit.y] == State::Free)
                        {
                            counts.push_back({i,this->lastHit.y,this->countOfFreeCells(i,this->lastHit.y)});
                            break;
                        }
                        if (this->field[i][this->lastHit.y] == State::Miss)
                            break;
                    }

                    for (auto i = this->lastHit.x + 1; i < 10; i++)
                    {
                        if (this->field[i][this->lastHit.y] == State::Free)
                        {
                            counts.push_back({i,this->lastHit.y,this->countOfFreeCells(i,this->lastHit.y)});
                            break;
                        }
                        if (this->field[i][this->lastHit.y] == State::Miss)
                            break;
                    }

                    if (counts.size() == 1)
                        move = sf::Vector2i(counts.at(0).at(0), counts.at(0).at(1));

                    else
                    {
                        if (counts.at(0).at(2) == counts.at(1).at(2))
                        {
                            if (random(0,1) == 0)
                                move = sf::Vector2i(counts.at(0).at(0), counts.at(0).at(1));

                            else
                                move = sf::Vector2i(counts.at(1).at(0), counts.at(1).at(1));
                        }
                        else if (counts.at(0).at(2) > counts.at(1).at(2))
                            move = sf::Vector2i(counts.at(0).at(0), counts.at(0).at(1));

                        else
                            move = sf::Vector2i(counts.at(1).at(0), counts.at(1).at(1));
                    }
                     this->previousStep = move;
                }
                else
                {
                    QVector<QVector<int>> counts;
                    for (auto j = this->lastHit.y - 1; j >= 0; j--)
                    {
                        if (this->field[this->lastHit.x][j] == State::Free)
                        {
                            counts.push_back({this->lastHit.x,j,this->countOfFreeCells(this->lastHit.x,j)});
                            break;
                        }
                        if (this->field[this->lastHit.x][j] == State::Miss)
                            break;
                    }

                    for (auto j = this->lastHit.y + 1; j < 10; j++)
                    {
                        if (this->field[this->lastHit.x][j] == State::Free)
                        {
                            counts.push_back({this->lastHit.x,j,this->countOfFreeCells(this->lastHit.x,j)});
                            break;
                        }
                        if (this->field[this->lastHit.x][j] == State::Miss)
                            break;
                    }

                    if (counts.size() == 1)
                    {

                        move = sf::Vector2i(counts.at(0).at(0), counts.at(0).at(1));
                    }
                    else
                    {
                        if (counts.at(0).at(2) == counts.at(1).at(2))
                        {
                            if (random(0,1) == 0)
                                move = sf::Vector2i(counts.at(0).at(0), counts.at(0).at(1));

                            else
                                move = sf::Vector2i(counts.at(1).at(0), counts.at(1).at(1));
                        }
                        else if (counts.at(0).at(2) > counts.at(1).at(2))
                            move = sf::Vector2i(counts.at(0).at(0), counts.at(0).at(1));

                        else
                            move = sf::Vector2i(counts.at(1).at(0), counts.at(1).at(1));
                    }
                    this->previousStep = move;
                }
            }

        }
        else
        {
            this->checkMatrix();
            if (this->canSwitchMode())
            {
                if (this->mode == BotMode::SearchFourDeckShip)
                {
                    this->mode = BotMode::SearchThreeTwoDeckShip;
                    this->fillMatrix();
                    this->checkMatrix();
                }

                else if (this->mode == BotMode::SearchThreeTwoDeckShip)
                    this->mode = BotMode::SearchOneDeckShip;
            }
            switch (this->mode)
            {
            case BotMode::SearchFourDeckShip: case BotMode::SearchThreeTwoDeckShip:
            {  
                while (1)
                {
                    int i = this->random(0,9);
                    int j = this->random(0,9);

                    if (this->attackMatrix[i][j] == 0)
                        continue;

                    this->attackMatrix[i][j] = 0;

                    this->previousStep = sf::Vector2i(i,j);
                    move = sf::Vector2i(i,j);
                    break;
                }
                break;
            }          
            case BotMode::SearchOneDeckShip:
            {
                move = this->randomShoot();
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    default:
        break;
    }
    return move;
}

void GameAlgorythm::setCellState(int i, int j, State state)
{
    this->field[i][j] = state;
}

bool GameAlgorythm::canShoot(int i, int j) const
{
    if (this->field[i][j] == State::Free)
        return true;

    return false;
}

void GameAlgorythm::checkMatrix()
{
    for (auto i = 0; i < 10; i++)
        for (auto j = 0; j < 10; j++)
        {
            if (this->field[i][j] == State::Hit || this->field[i][j] == State::Miss || this->field[i][j] == State::Dead)
            {
                this->attackMatrix[i][j] = 0;
            }
        }
}

bool GameAlgorythm::canSwitchMode() const
{
    for (auto i = 0; i < 10; i++)
        for (auto j = 0; j < 10; j++)
        {
            if (this->attackMatrix[i][j]!=0)
                return false;
        }
    return true;
}

int GameAlgorythm::countOfFreeCells(int i, int j) const
{
    int count = 0;
    if (j-1 >= 0)
        if (this->field[i][j-1] == State::Hit)
        {
            for (auto x = -1; x < 2; x++)
                for (auto y = 0; y < 2; y++)
                {
                    if (x+i >= 0 && x+i<10 && y+j >= 0 && y+j < 10)
                        if ((x != 0 || y != 0) && this->field[i+x][j+y] == State::Free)
                            count++;
                }
            return count;
        }
    if (i-1 >= 0)
        if (this->field[i-1][j] == State::Hit)
        {
            for (auto x = 0; x < 2; x++)
                for (auto y = -1; y < 2; y++)
                {
                    if (x+i >= 0 && x+i<10 && y+j >= 0 && y+j < 10)
                        if ((x != 0 || y != 0) && this->field[i+x][j+y] == State::Free)
                            count++;
                }
            return count;
        }
    if (i+1 < 10)
        if (this->field[i+1][j] == State::Hit)
        {
            for (auto x = -1; x < 1; x++)
                for (auto y = -1; y < 2; y++)
                {
                    if (x+i >= 0 && x+i<10 && y+j >= 0 && y+j < 10)
                        if ((x != 0 || y != 0) && this->field[i+x][j+y] == State::Free)
                            count++;
                }
            return count;
        }
    if (j+1 < 10)
        if (this->field[i][j+1] == State::Hit)
        {
            for (auto x = -1; x < 2; x++)
                for (auto y = -1; y < 1; y++)
                {
                    if (x+i >= 0 && x+i<10 && y+j >= 0 && y+j < 10)
                        if ((x != 0 || y != 0) && this->field[i+x][j+y] == State::Free)
                            count++;
                }
            return count;
        }
    return count;
}

sf::Vector2i GameAlgorythm::randomShoot()
{
    sf::Vector2i move;
    while (true)
    {
        int i = random(0,9);
        int j = random(0,9);
        if (this->canShoot(i,j))
        {
            move = sf::Vector2i(i,j);
            break;
        }
    }
    return move;
}

int GameAlgorythm::random(int min, int max)
{
    static std::mt19937 gen(time(NULL));
    std::uniform_int_distribution<> uid(min, max);
    return uid(gen);
}

void GameAlgorythm::fillMatrix()
{
    switch (this->algorythm) {
    case Algorythm::Diagonal:
    {
        switch (mode) {
        case BotMode::SearchFourDeckShip:
        {
            switch (this->choosenPreset) {
            case 0:
            {
                this->attackMatrix = diagonalFirstStepFirstOpt;
                break;
            }
            case 1:
            {
                this->attackMatrix = diagonalFirstStepSecondOpt;
                break;
            }
            default:
                break;
            }
            break;
        }
        case BotMode::SearchThreeTwoDeckShip:
        {
            switch (this->choosenPreset) {
            case 0:
            {
                this->attackMatrix = diagonalSecondStepFirstOpt;
                break;
            }
            case 1:
            {
                this->attackMatrix = diagonalSecondStepSecondOpt;
                break;
            }
            default:
                break;
            }

            break;
        }
        default:
            break;
        }

        break;
    }
    default:
        break;
    }
}
