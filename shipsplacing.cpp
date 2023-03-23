#include "shipsplacing.h"
#include "ui_shipsplacing.h"
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QPixmap>
#include <QPalette>

FieldPlacing::FieldPlacing(QWidget *parent, const QPoint &position, const QSize &size):
     SFMLWidget(parent, position, size),sizeOfNewShip(0), max(4), previousDirection(Direction::Right), move(M_State::Nothing)
{
    this->spriteShipyard.setPosition(574,0);
    this->spriteShipyard.setScale(0.835,0.835);
    this->color = sf::Color::White;
    this->fleet = new Fleet();
    this->cursor.loadFromSystem(sf::Cursor::SizeAll);
    this->defaultCursor.loadFromSystem(sf::Cursor::Arrow);
}

FieldPlacing::~FieldPlacing()
{
    for (auto ship : ships)
      {
        delete ship;
      }
    ships.clear();
    delete fleet;
}

 void FieldPlacing::mousePressEvent(QMouseEvent *event)
 {

     int x = sf::Mouse::getPosition().x - RenderWindow::getPosition().x;
     int y = sf::Mouse::getPosition().y - RenderWindow::getPosition().y;

     if (x > 1 && x < (10*cell_size+1) && y > 1 && y < (10*cell_size+1))//if in main field
     {
         int border_thickness = 1;
         int h_plates_count = 10;
         int v_plates_count = 10;

         for (int i = 0; i < v_plates_count; ++i)
             for (int j = 0; j < h_plates_count; ++j)
             {


                 if (((x > (border_thickness + j * cell_size)) && (y > (border_thickness + i * cell_size))) &&
                         ((x < (border_thickness + j * cell_size+cell_size)) && (y < (border_thickness + i * cell_size+cell_size))))
                 {
                     sf::Vector2f coords(border_thickness + j * cell_size, border_thickness + i * cell_size);

                     if(event->button()==Qt::RightButton)
                     {
                         switch (move) {
                         case M_State::Nothing://Rotation
                         {
                             for (auto ind = 0 ; ind <  fleet->ships.size(); ind++)
                             {
                                 if (fleet->ships[ind]->getSprite().getGlobalBounds().contains(x, y))
                                 {
                                     if (fleet->canRotate(ind))
                                     {
                                         fleet->ships[ind]->rotate();
                                         fleet->resetCells();
                                     }
                                     else
                                     {
                                         fleet->clearField();
                                         fleet->resetCells();
                                     }
                                     break;
                                 }
                             }
                             break;
                         }
                         case M_State::AddShipToField:
                         {
                             if (this->ships[indOfShip]->getDirection() != Direction::Right)
                             {
                                 this->ships[indOfShip]->setDirection(Direction::Right);
                                 this->ships[indOfShip]->rotateSprite();
                             }

                             this->ships[indOfShip]->setColor(sf::Color::White);
                             this->ships[indOfShip]->resetPositionWithOffset(cell_size);

                             move = M_State::Nothing;
                             sf::Window::setMouseCursor(this->defaultCursor);
                             this->deleteRect();
                             emit sendData(true);
                             break;
                         }
                         case M_State::MoveInsideField:
                         {
                             if (this->fleet->ships[indOfShip]->getDirection() != this->previousDirection)
                             {
                                 this->fleet->ships[indOfShip]->setDirection(this->previousDirection);
                                 this->fleet->ships[indOfShip]->rotateSprite();
                             }

                             this->fleet->ships[indOfShip]->setColor(sf::Color::White);
                             this->fleet->ships[indOfShip]->resetPosition();

                             this->move = M_State::Nothing;
                             sf::Window::setMouseCursor(this->defaultCursor);
                             this->deleteRect();
                             this->fleet->resetCells();
                             emit sendData(true);
                             break;
                         }
                         default:
                             break;
                         }
                     }
                     else
                     {
                         switch (move) {
                         case M_State::Nothing:
                         {
                             for (auto ind = 0 ; ind < fleet->ships.size(); ind++)
                             {
                                 if (this->fleet->ships[ind]->getSprite().getGlobalBounds().contains(x, y))
                                 {
                                     this->indOfShip = ind;

                                     this->previousDirection = this->fleet->ships[ind]->getDirection();
                                     this->move = M_State::MoveInsideField;

                                     this->fleet->ships[ind]->setColor(sf::Color::Green);
                                     sf::Window::setMouseCursor(this->cursor);

                                     this->fleet->clearField();
                                     this->fleet->raiseShipNResetCells(indOfShip);
                                     this->fillRed();

                                     emit sendData(false);
                                     break;
                                 }

                             }
                             break;
                         }

                         case  M_State::AddShipToField:
                         {
                             if (fleet->canPlaceShip(this->ships[indOfShip]->getDirection(),this->ships[indOfShip]->getSize(),j,i))
                             {
                                 this->deleteRect();
                                 this->ships[indOfShip]->setColor(sf::Color::White);
                                 this->move = M_State::Nothing;
                                 sf::Window::setMouseCursor(this->defaultCursor);

                                 this->fleet->addShip(new Ship(j,i,this->ships[indOfShip]->getSize(),this->ships[indOfShip]->getDirection()));
                                 this->fleet->ships[fleet->ships.size()-1]->resetPosition();

                                 delete this->ships[indOfShip];
                                 this->ships.erase(ships.begin()+indOfShip);

                                 emit sendData(true);
                             }
                             break;
                         }
                         case  M_State::MoveInsideField:
                         {
                             if (fleet->canPlaceShip(this->fleet->ships[indOfShip]->getDirection(),fleet->ships[indOfShip]->getSize(),j,i))
                             {
                                 this->deleteRect();
                                 fleet->ships[indOfShip]->setColor(sf::Color::White);
                                  sf::Window::setMouseCursor(this->defaultCursor);

                                 fleet->ships[indOfShip]->resetCells(sf::Vector2i(j,i));
                                 fleet->ships[indOfShip]->resetPosition();

                                 fleet->clearField();
                                 fleet->resetCells();

                                 move = M_State::Nothing;
                                 emit sendData(true);
                             }
                             break;
                         }
                         default:
                             break;
                         }
                     }
                     i=10;j=10;//break
                 }
             }
     }
     else if(event->button()==Qt::LeftButton) //if we wanna move ship
     {
         if (move == M_State::Nothing)
             for (auto ind = 0 ; ind < ships.size(); ind++)
             {
                 if (ships[ind]->getSprite().getGlobalBounds().contains(x, y))
                 {
                     indOfShip = ind;
                     move = M_State::AddShipToField;

                     ships[ind]->setColor(sf::Color::Green);
                     sf::Window::setMouseCursor(this->cursor);

                     this->fillRed();
                     emit sendData(false);
                     break;
                 }
             }
     }
     else if(event->button()==Qt::RightButton && move != M_State::Nothing)
     {
         switch (move) {
         case M_State::MoveInsideField:
         {
             fleet->ships[indOfShip]->setColor(sf::Color::White);
             fleet->ships[indOfShip]->resetPosition();
             break;
         }
         case M_State::AddShipToField:
         {
             if (this->ships[indOfShip]->getDirection() != Direction::Right)
             {
                 this->ships[indOfShip]->setDirection(Direction::Right);
                 this->ships[indOfShip]->rotateSprite();
             }
             this->ships[indOfShip]->setColor(sf::Color::White);
             this->ships[indOfShip]->resetPositionWithOffset(cell_size);
             break;
         }
         default:
             break;
         }
        move = M_State::Nothing;
        sf::Window::setMouseCursor(this->defaultCursor);
        this->deleteRect();
        emit sendData(true);
     }
     QWidget::update();
 }

 void FieldPlacing::mouseMoveEvent(QMouseEvent *)
 {
     int x = sf::Mouse::getPosition().x - RenderWindow::getPosition().x;
     int y = sf::Mouse::getPosition().y - RenderWindow::getPosition().y;

     switch (move) {
     case M_State::MoveInsideField:
     {  
         this->fleet->ships[indOfShip]->placeShipNearCursor(x,y);
         break;
     }
     case M_State::AddShipToField:
     {        
         this->ships[indOfShip]->placeShipNearCursor(x,y);
         break;
     }
     default:
         break;
     }

 }

 void FieldPlacing::wheelEvent(QWheelEvent *event)
 {
     int x = sf::Mouse::getPosition().x - RenderWindow::getPosition().x;
     int y = sf::Mouse::getPosition().y - RenderWindow::getPosition().y;

     QPoint numDegrees = event->angleDelta() / 8;

     switch (move) {
     case M_State::MoveInsideField:
     {
         if (numDegrees.y() > 0 && this->fleet->ships[indOfShip]->getDirection() != Direction::Right)
         {
             this->fleet->ships[indOfShip]->setDirection(Direction::Right);
             this->fleet->ships[indOfShip]->rotateSprite();
         }

         else if (numDegrees.y() < 0 && this->fleet->ships[indOfShip]->getDirection() != Direction::Down)
         {
             this->fleet->ships[indOfShip]->setDirection(Direction::Down);
             this->fleet->ships[indOfShip]->rotateSprite();
         }

         this->fleet->ships[indOfShip]->placeShipNearCursor(x,y);
         break;
     }
     case M_State::AddShipToField:
     {
         if (numDegrees.y() > 0 && this->ships[indOfShip]->getDirection() != Direction::Right)
         {
             this->ships[indOfShip]->setDirection(Direction::Right);
             this->ships[indOfShip]->rotateSprite();
         }

         else if (numDegrees.y() < 0 && this->ships[indOfShip]->getDirection() != Direction::Down)
         {
             this->ships[indOfShip]->setDirection(Direction::Down);
             this->ships[indOfShip]->rotateSprite();
         }

         this->ships[indOfShip]->placeShipNearCursor(x,y);
         break;
     }
     default:
         break;
     }
 }

 void FieldPlacing::deleteShips()
 {
     for (auto ship : this->ships)
       {
         delete ship;
       }
    this->ships.clear();
 }

 void FieldPlacing::update()
 {
     for (auto i = 0; i < 8; i++)
     {
         for (auto j = 0; j < 5; j++)
         {
             RenderWindow::draw(this->spriteBack);
             this->spriteBack.setPosition(i*2*cell_size,j*2*cell_size);
         }
     }

     for (auto i = 0; i < 10; i++)
          {
              for (auto j = 0; j < 10; j++)
              {
                  spriteCell.setPosition(i*cell_size+1, j*cell_size+1);

                  RenderWindow::draw(spriteCell);
              }
          }

     for (auto &elem : rectangles) {
         RenderWindow::draw(*elem);
     }

     RenderWindow::draw(spriteShipyard);

     if ((move == M_State::MoveInsideField || move == M_State::AddShipToField) && fleet->ships.size() > indOfShip)
     {
         for (auto i = 0; i < fleet->ships.size(); i++)
         {
             if (i != indOfShip)
                 RenderWindow::draw(fleet->ships[i]->getSprite());
         }
         RenderWindow::draw(fleet->ships[indOfShip]->getSprite());

     }
     else
          for (auto elem : fleet->ships) {
              RenderWindow::draw(elem->getSprite());
          }

    if ((move == M_State::MoveInsideField || move == M_State::AddShipToField) && this->ships.size() > indOfShip)
    {
        for (auto i = 0; i < this->ships.size(); i++)
        {
            if (i != indOfShip)
                RenderWindow::draw(this->ships[i]->getSprite());
        }
        RenderWindow::draw(this->ships[indOfShip]->getSprite());

    }
    else
         for (auto elem : this->ships) {
             RenderWindow::draw(elem->getSprite());
         }
 }


 void FieldPlacing::initialize()
 {
     this->textureCell.loadFromFile("images\\frame.png");
     this->spriteCell.setTexture(this->textureCell);

     this->textureShipyard.loadFromFile("images\\shipyard.png");
     this->spriteShipyard.setTexture(this->textureShipyard);

     this->textureBack.loadFromFile("images\\sea.jpg");
     this->spriteBack.setTexture(this->textureBack);
     this->spriteBack.setScale(0.09,0.09);

     this->fleet->clearField();

     auto max = 4;
     auto line = 0;
     for (auto i =1 ; i <= 4 ; i++)
     {
         for (auto j = 0; j < max; j++)
         {
             ships.push_back(new Ship((max+9),line,i,Direction::Right));
             line++;
         }
         max--;
     }
 }

 int FieldPlacing::getCountOfShips() const
 {
     return this->ships.size();
 }

 int FieldPlacing::getSizeOfFleet() const
 {
     return this->fleet->ships.size();
 }

 void FieldPlacing::fillRed()
 {
     int border_thickness = 1;
     for (auto i = 0; i < 10; i++)
         for (auto j = 0; j < 10; j++)
         {
             if(fleet->field[i][j] == State::ShipNear || fleet->field[i][j] == State::Ship)
             {
                 sf::RectangleShape *rect = new sf::RectangleShape(sf::Vector2f(cell_size, cell_size));
                 sf::Vector2f coords(border_thickness + i * cell_size,border_thickness + j * cell_size);
                 rect->setPosition(coords);
                 rect->setFillColor(sf::Color (196,30,58,180));
                 rectangles.push_back(rect);
             }
         }
 }

 void FieldPlacing::randomLocation()
 {
     this->deleteFleet();
     this->deleteShips();
     this->fleet->randomLocation();
 }

 void FieldPlacing::restore()
 {
     this->deleteShips();
     this->deleteFleet();
     this->initialize();
 }

 void FieldPlacing::deleteFleet()
 {
     this->fleet->deleteShips();
     this->fleet->clearField();
 }

 Fleet* FieldPlacing::getFleet() const
 {
    return this->fleet;
 }

ShipsPlacing::ShipsPlacing(QWidget *parent) :
    QWidget(parent),gameMode(GameMode::Bot),
    ui(new Ui::ShipsPlacing)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    this->setMouseTracking(true);

    QPixmap bkgnd("images\\background5.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    ui->CancelBtn->setStyleSheet("QPushButton#CancelBtn {"
                               "background-color: red;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");

    ui->ConfirmBtn->setStyleSheet("QPushButton#ConfirmBtn {"
                               "background-color: green;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");

    ui->RandomBtn->setStyleSheet("QPushButton#RandomBtn {"
                               "background-color: rgb(168,228,160);"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");

    ui->RestoreBtn->setStyleSheet("QPushButton#RestoreBtn {"
                               "background-color: rgb(106,90,205);"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");
    field = new FieldPlacing(this, QPoint(this->size().width()/2 - 682/2,this->size().height()/2 - 401*0.55), QSize(682,401));

    field->show();
    field->setMouseTracking(true);

    connect(field, &FieldPlacing::sendData,this, &ShipsPlacing::receiveData);

    dialog = new BotSetup();
    dialog->setItems(QStringList({"Easy", "Medium"}));

    this->mb = new QMessageBox(this);
    this->mb->setAttribute(Qt::WA_DeleteOnClose, true);
    this->mb->setWindowTitle("Waiting");
    this->mb->setText("Waiting for your opponent and a response from the server!");
    this->mb->setIcon(QMessageBox::Information);
}


ShipsPlacing::~ShipsPlacing()
{
    delete ui;
    delete field;
    delete dialog;
}

void ShipsPlacing::closeEvent(QCloseEvent *event)
{
    emit saveStatToFile();
    emit saveConfigToFile();
    event->accept();
}

void ShipsPlacing::hideEvent(QHideEvent *event)
{
    this->mb->close();
    QWidget::hideEvent(event);
}

void ShipsPlacing::shipsPlaced()
{
    this->hide();
    emit sendFleet(this->field->getFleet());
    field->deleteShips();
    emit firstWindow();
    this->ui->ConfirmBtn->setEnabled(true);
    this->mb->close();
}

void ShipsPlacing::clearFleetInShipsPlacing()
{
    field->deleteShips();
    field->deleteFleet();
}

void ShipsPlacing::on_CancelBtn_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Attention!");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setDefaultButton(QMessageBox::Ok);

    switch (this->gameMode) {
    case GameMode::Bot:
    {
        if (field->getSizeOfFleet() != 0)
        {
            msgBox.setText("Are you sure you want to go back to the main menu?\nAll spaced ships will reset!");
            int res = msgBox.exec();
            if (res == QMessageBox::Ok)
            {
                this->hide();
                field->deleteShips();
                field->deleteFleet();
                emit helloWindow();
            }
        }
        else
        {
            this->hide();
            field->deleteShips();
            emit helloWindow();
        }
        break;
    }
    case GameMode::Multiplayer:
    {
        msgBox.setText("Are you sure you want to go back to the main menu?\nYou will be disconnected from the server!");
        int res = msgBox.exec();
        if (res == QMessageBox::Ok)
        {
            this->hide();
            field->deleteShips();
            field->deleteFleet();
            emit helloWindow();
            emit closeConnection();
        }
        break;
    }
    default:
        break;
    }

}


void ShipsPlacing::on_ConfirmBtn_clicked()
{
    if (field->getCountOfShips() == 0)
    {
        switch (this->gameMode) {
        case GameMode::Bot:
        {
            if (dialog->exec() == QDialog::Accepted)
            {
                this->dialog->close();
                this->hide();
                emit sendFleet(this->field->getFleet());
                if (this->dialog->comboBox()->currentText() == "Easy")
                    emit sendDifficulty(Difficulty::Easy);
                else
                    emit sendDifficulty(Difficulty::Medium);

                field->deleteShips();
                emit firstWindow();
            }
            break;
        }
        case GameMode::Multiplayer:
        {
            emit sendCommandToServer("ShipsPlaced");
            this->ui->ConfirmBtn->setEnabled(false);
            this->mb->show();
            break;
        }
        default:
            break;
        }
    }
    else
    {
        QMessageBox::warning(this,"Attention","You haven't set up all the ships");
    }
}

void ShipsPlacing::on_RestoreBtn_clicked()
{
    field->restore();
}

void ShipsPlacing::receiveData(bool enable)
{
    this->ui->CancelBtn->setEnabled(enable);
    this->ui->ConfirmBtn->setEnabled(enable);
    this->ui->RestoreBtn->setEnabled(enable);
    this->ui->RandomBtn->setEnabled(enable);
}


void ShipsPlacing::on_RandomBtn_clicked()
{
    field->randomLocation();
}

BotSetup::BotSetup(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle("Bot setup");
    this->setFixedSize(QSize(150,200));
    this->setLayout(new QVBoxLayout());

    QLabel *t_label = new QLabel("Choose bot difficult!");
    this->layout()->addWidget(t_label);
    this->layout()->setAlignment(t_label, Qt::AlignCenter);

    QLabel *i_label = new QLabel();
    i_label->setPixmap(QPixmap("images\\robot.png"));
    this->layout()->addWidget(i_label);
    this->layout()->setAlignment(i_label, Qt::AlignCenter);

    box = new QComboBox;
    this->layout()->addWidget(box);

    QPushButton *ok = new QPushButton("Continue");
    ok->setStyleSheet("background-color: #18A7B5;"
                      "border-style: outset;"
                      "border-width: 2px;"
                      "border-radius: 10px;"
                      "border-color: beige;"
                      "font: bold 20px;"
                      "padding: 6px;");

    this->layout()->addWidget(ok);
    connect(ok, &QPushButton::clicked, this, [this]()
    {
       accept();
    });
}

void BotSetup::setItems(QStringList strings)
{
    this->box->addItems(strings);
}

QComboBox* BotSetup::comboBox()
{
    return box;
}

