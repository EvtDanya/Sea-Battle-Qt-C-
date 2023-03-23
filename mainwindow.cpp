#include "mainwindow.h"
#include "ui_mainwindow.h"

int offset = 40;
int countOfMusicTracks = 9;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->setMinimumSize(1200,768);

    this->config = new Config();
    this->config->loadFromFile();

    this->soundSliderWindow = new SoundSliderWindow();
    this->soundSliderWindow->setVolume(this->config->volume);

    //Statisctics window
    this->statWindow = new StatWindow();

    /*Ships placing window*/
    this->shipsPlacing = new ShipsPlacing();
    connect(shipsPlacing, &ShipsPlacing::firstWindow, this, &QMainWindow::show);
    connect(this, &MainWindow::clearFleetInShipsPlacing, shipsPlacing, &ShipsPlacing::clearFleetInShipsPlacing);
    this->shipsPlacing->setMouseTracking(true);

    /*Hello window*/
    this->helloWindow = new HelloWindow();

    if (this->config->volume == 0)
        this->helloWindow->isVolumeOn = false;
    else
        this->helloWindow->isVolumeOn = true;

    //Music
    this->music = new Music(this);
    this->music->setVolume(this->config->volume);
    this->music->setCurrentMusic(GameAlgorythm::random(0,countOfMusicTracks-1));
    this->music->start();

    connect(helloWindow, &HelloWindow::shipsPlacingWindow, shipsPlacing, &QWidget::show);
    connect(shipsPlacing, &ShipsPlacing::helloWindow, helloWindow, &QWidget::show);
    connect(helloWindow, &HelloWindow::statWindow, statWindow, &QWidget::show);
    connect(statWindow, &StatWindow::helloWindow, helloWindow, &QWidget::show);

    //Check rank (for stat)
    connect(statWindow, &StatWindow::sendRankCheckedInfo, helloWindow, &HelloWindow::receiveRankCheckedInfo);
    connect(helloWindow, &HelloWindow::checkRank, statWindow, &StatWindow::checkRankDelay);

    /*Connection window*/
    this->localGameWindow = new LocalGameWindow();
    connect(localGameWindow->client, &Client::hideConnectionWindow, localGameWindow, &LocalGameWindow::hide);
    connect(localGameWindow->client, &Client::hideConnectionWindow, localGameWindow->mb, &QMessageBox::close);
    connect(localGameWindow->client, &Client::gameWindow, shipsPlacing, &ShipsPlacing::shipsPlaced);

    connect(helloWindow, &HelloWindow::localGameWindow, localGameWindow, &QWidget::show);
    connect(localGameWindow, &LocalGameWindow::helloWindow, helloWindow, &QWidget::show);
    connect(localGameWindow->client, &Client::shipsPlacing, shipsPlacing, &QWidget::show);

    //Chat
    connect(this->localGameWindow->client, &Client::addTextChat, this, &MainWindow::addTextChat);

    this->helloWindow->show();

    this->playerField = new PlayerField(this, QPoint(this->size().width()/2-481-64,this->size().height()/2 - 481*0.7), QSize(401+80,401+80));
    this->enemyField = new EnemyField(this, QPoint(this->size().width()/2+64,this->size().height()/2 - 481*0.7), QSize(401+80,401+80));
    this->arrow = new ArrowWidget(this, QPoint(this->size().width()/2 - 64 *0.5, this->size().height()/2 - 72*2), QSize(64,72));

    //Server-client
    connect(shipsPlacing, &ShipsPlacing::sendCommandToServer, localGameWindow->client, &Client::sendCommandToServer);
    connect(this->enemyField, &EnemyField::sendCommandToServer, localGameWindow->client, &Client::sendCommandToServer);
    connect(this->playerField, &PlayerField::sendCommandToServer, localGameWindow->client, &Client::sendCommandToServer);
    connect(this, &MainWindow::closeConnection, localGameWindow->client, &Client::disconnect);
    connect(shipsPlacing, &ShipsPlacing::closeConnection, localGameWindow->client, &Client::disconnect);
    connect(this->localGameWindow->client, &Client::secondPlayer, this, &MainWindow::secondPlayer);
    connect(this->localGameWindow->client, &Client::shoot, this->playerField, &PlayerField::enemyShoot);
    connect(this->localGameWindow->client, &Client::shotResult, this->enemyField, &EnemyField::shotResult);
    connect(this->localGameWindow->client, &Client::playerCanShoot,enemyField,&EnemyField::canShoot);
    connect(this, &MainWindow::messageAboutDisconnect,helloWindow,&HelloWindow::messageAboutDisconnect);
    connect(this->localGameWindow->client, &Client::serverDisconnected, this, &MainWindow::serverDisconnected);
    connect(this, &MainWindow::setMessageType, helloWindow, &HelloWindow::setMessage);
    connect(this->localGameWindow->client, &Client::setMessageType, helloWindow, &HelloWindow::setMessage);
    connect(this->localGameWindow, &LocalGameWindow::setMessageType, helloWindow, &HelloWindow::setMessage);

    this->enemyField->show();
    this->arrow->show();
    this->playerField->show();

    //Start game(for statistics)
    connect(enemyField, &EnemyField::gameStartedStat, this, &MainWindow::startGame);

    //Save stat
    connect(localGameWindow, &LocalGameWindow::saveStatToFile, statWindow, &StatWindow::saveStatToFile);
    connect(helloWindow, &HelloWindow::saveStatToFile, statWindow, &StatWindow::saveStatToFile);
    connect(shipsPlacing, &ShipsPlacing::saveStatToFile, statWindow, &StatWindow::saveStatToFile);

    //Config save
    connect(helloWindow, &HelloWindow::saveConfigToFile, this, &MainWindow::saveConfigToFile);
    connect(localGameWindow, &LocalGameWindow::saveConfigToFile, this, &MainWindow::saveConfigToFile);
    connect(shipsPlacing, &ShipsPlacing::saveConfigToFile, this, &MainWindow::saveConfigToFile);
    connect(statWindow, &StatWindow::saveConfigToFile, this, &MainWindow::saveConfigToFile);

    //Sound
    connect(soundSliderWindow, &SoundSliderWindow::sliderMoved, this, &MainWindow::volumeChanged);
    connect(helloWindow, &HelloWindow::volumeChanged, this, &MainWindow::volumeChanged);
    connect(helloWindow, &HelloWindow::moveSlider, this->soundSliderWindow, &SoundSliderWindow::setVolume);
    connect(soundSliderWindow, &SoundSliderWindow::setVolumeBtnIcon, helloWindow, &HelloWindow::sliderMoved);

    //Win/lose windows
    connect(playerField, &PlayerField::endGame,this,&MainWindow::loseWindow);
    connect(enemyField, &EnemyField::endGame,this,&MainWindow::winWindow);

    //Send data
    connect(shipsPlacing, &ShipsPlacing::sendDifficulty,this,&MainWindow::receiveDifficulty);
    connect(shipsPlacing, &ShipsPlacing::sendFleet,this,&MainWindow::receiveFleet);
    connect(helloWindow, &HelloWindow::isMultiplayer,this,&MainWindow::isMultiplayer);

    //Pass moves
    connect(enemyField, &EnemyField::passMove,playerField,&PlayerField::startMove);
    connect(playerField, &PlayerField::playerCanShoot,enemyField,&EnemyField::canShoot);

    //Arrow rotation
    connect(enemyField, &EnemyField::rotateArrow, arrow, &ArrowWidget::rotate);
    connect(playerField, &PlayerField::rotateArrow, arrow, &ArrowWidget::rotate);

    //Menu bar
     connect(playerField, &PlayerField::setEnableMenuBar, this, &MainWindow::setEnableMenuBar);
     connect(enemyField, &EnemyField::setEnableMenuBar, this, &MainWindow::setEnableMenuBar);

     QPixmap bkgnd("images\\background7.jpg");
     bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
     QPalette palette;
     palette.setBrush(QPalette::Window, bkgnd);
     this->setPalette(palette);

     ui->sendBtn->setStyleSheet("QPushButton#sendBtn {"
                                "background-color: #006633;"
                                "border-style: outset;"
                                "border-width: 2px;"
                                "border-radius: 10px;"
                                "border-color: beige;"
                                "font: bold 18px;"
                                "padding: 6px;""}");

     ui->chatHistory->setStyleSheet("QTextBrowser#chatHistory {"
                                "background-color: #FDF4E3;"
                                "border-style: outset;"
                                "border-width: 2px;"
                                "border-radius: 10px;"
                                "border-color: beige;"
                                "font: bold 18px;"
                                "padding: 6px;""}");

     ui->lineEdit->setStyleSheet("QLineEdit#lineEdit {"
                                "background-color: #FDF4E3;"
                                "border-style: outset;"
                                "border-width: 2px;"
                                "border-radius: 10px;"
                                "border-color: beige;"
                                "font: bold 18px;"
                                "padding: 6px;""}");

}

MainWindow::~MainWindow()
{
    delete this->ui;
    delete this->shipsPlacing;
    delete this->helloWindow;
    delete this->localGameWindow;
    delete this->statWindow;
    delete this->config;
    delete this->soundSliderWindow;
    delete this->music;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
     QPixmap bkgnd("images\\background7.jpg");
     bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
     QPalette palette;
     palette.setBrush(QPalette::Window, bkgnd);
     this->setPalette(palette);

    this->arrow->move(this->size().width()/2 - 64 *0.5, this->size().height()/2 - 72*2);
    this->playerField->move(this->size().width()/2-481-64,this->size().height()/2 - 481*0.7);
    this->enemyField->move(this->size().width()/2+64,this->size().height()/2 - 481*0.7);

     QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->statWindow->stat->saveToFile();
    this->saveConfigToFile();
    event->accept();
}

void MainWindow::showEvent(QShowEvent *event)
{
    this->ui->chatHistory->clear();
    switch (this->gameMode) {
    case GameMode::Bot:
    {
        this->ui->lineEdit->setEnabled(false);
        this->ui->sendBtn->setEnabled(false);
        break;
    }
    case GameMode::Multiplayer:
    {
        this->ui->lineEdit->setEnabled(true);
        this->ui->sendBtn->setEnabled(true);
        break;
    }
    default:
        break;
    }
    QMainWindow::showEvent(event);
}

void MainWindow::updateStatDefeat()
{
    if (this->enemyField->gameStarted)
        this->statWindow->stat->unfinishedGames--;
    switch (this->gameMode) {
    case GameMode::Bot:
    {
        switch (this->playerField->getDifficulty()) {
        case Difficulty::Easy:
        {
            this->statWindow->stat->defeatsEasyBot++;
            break;
        }
        case Difficulty::Medium:
        {
            this->statWindow->stat->defeatsMediumBot++;
            break;
        }
        default:
            break;
        }
        break;
    }
    case GameMode::Multiplayer:
    {
        this->statWindow->stat->defeatsMultiplayer++;
        break;
    }
    default:
        break;
    }
}

void MainWindow::updateStatWin()
{
    if (this->enemyField->gameStarted)
        this->statWindow->stat->unfinishedGames--;
    switch (this->gameMode) {
    case GameMode::Bot:
    {
        switch (this->playerField->getDifficulty()) {
        case Difficulty::Easy:
        {
            this->statWindow->stat->winsEasyBot++;
            break;
        }
        case Difficulty::Medium:
        {
            this->statWindow->stat->winsMediumBot++;
            break;
        }
        default:
            break;
        }
        break;
    }
    case GameMode::Multiplayer:
    {
        this->statWindow->stat->winsMultiplayer++;
        break;
    }
    default:
        break;
    }
}

void MainWindow::startGame()
{
    this->statWindow->stat->unfinishedGames++;
}

void MainWindow::volumeChanged(uint8_t volume)
{
    this->config->volume = volume;
    this->music->setVolume(volume);
}

void MainWindow::saveConfigToFile()
{
    this->config->saveToFile();
}

void MainWindow::isMultiplayer(bool isMultiplayer)
{
    if (isMultiplayer)
    {
        this->gameMode = GameMode::Multiplayer;
        this->shipsPlacing->gameMode = GameMode::Multiplayer;
        this->playerField->gameMode = GameMode::Multiplayer;
        this->enemyField->gameMode = GameMode::Multiplayer;
        this->ui->actionInterrupt->setEnabled(false);
    }
    else
    {
        this->gameMode = GameMode::Bot;
        this->shipsPlacing->gameMode = GameMode::Bot;
        this->playerField->gameMode = GameMode::Bot;
        this->enemyField->gameMode = GameMode::Bot;
        this->ui->actionInterrupt->setEnabled(true);
    }
}

void MainWindow::addTextChat(QString text)
{
    this->ui->chatHistory->append(text);
}

void MainWindow::secondPlayer()
{
    this->arrow->rotate();
    this->enemyField->canShoot(false);
}

void MainWindow::serverDisconnected()
{
    this->hide();
    this->playerField->deleteAll();
    this->enemyField->Field::deleteAll();
    this->enemyField->canShoot(true);

    this->localGameWindow->hide();
    this->soundSliderWindow->hide();
    this->shipsPlacing->hide();

    this->helloWindow->show();
    //emit closeConnection();
    emit clearFleetInShipsPlacing();
    emit messageAboutDisconnect();
}

void MainWindow::on_actionExit_triggered()
{
    switch (this->gameMode) {
    case GameMode::Multiplayer:
    {
        emit setMessageType(Message::Nothing);
        emit closeConnection();
        this->serverDisconnected();
        break;
    }
    default:
        break;
    }
    helloWindow->show();
    this->playerField->deleteAll();
    this->enemyField->deleteAll();
    ui->actionInterrupt->setEnabled(true);
    this->hide();
}


void MainWindow::on_actionHelp_triggered()
{
    QString text = "<p style='text-align: center;'><img src='images\\helm.png' alt='' width='42' height='42'></p>"
                   "<p style='text-align: center;'><strong>Sea battle</strong></p>"
                   "<p style='text-align: center;'>Version 1.5 alpha</p>"
                   "<p style='text-align: center;'>by Fomin Danil Andreevich, group AB-124. NSTU.</p>"
                   "<p style='text-align: center;'>Copyright &copy; Fomin Danil, 2023. All rights reserved.</p>"
                   "<p style='text-align: center;'>&nbsp;</p>"
                   "<p>If you have any questions, write to my email EvtDaniF@yandex.ru.</p>";

    QMessageBox::about(this, "About", text);
}

void MainWindow::receiveFleet(Fleet *fleet)
{
    this->playerField->setFleet(fleet);
}

void MainWindow::receiveDifficulty(Difficulty difficulty)
{
    this->playerField->setDifficulty(difficulty);
}

void MainWindow::winWindow()
{
    auto *msgBox = new QMessageBox;

    QPushButton* pButtonYes = msgBox->addButton(tr("Go to main menu"), QMessageBox::YesRole);
    msgBox->setWindowTitle("Congratulations!");
    QPixmap bigPict("images\\win.png");
    msgBox->setIconPixmap(bigPict.scaled(QSize(512, 512),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    msgBox->exec();
    if (msgBox->clickedButton()==pButtonYes)
    {
        this->hide();
        this->playerField->deleteAll();
        this->enemyField->Field::deleteAll();
        this->enemyField->canShoot(true);
        this->helloWindow->show();
        this->setEnableMenuBar(true);
        if (this->gameMode == GameMode::Multiplayer)
        {
            emit setMessageType(Message::Nothing);
            emit closeConnection();
            this->serverDisconnected();
        }
        delete msgBox;
    }
    this->updateStatWin();
}

void MainWindow::loseWindow()
{
    auto *msgBox = new QMessageBox;

    QPushButton* pButtonYes = msgBox->addButton(tr("Go to main menu"), QMessageBox::YesRole);
    msgBox->setWindowTitle("Oops!");
    QPixmap bigPict("images\\lose.jpg");
    msgBox->setIconPixmap(bigPict.scaled(QSize(640, 480),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    msgBox->exec();
    if (msgBox->clickedButton()==pButtonYes)
    {
        this->hide();
        this->playerField->deleteAll();
        this->enemyField->Field::deleteAll();
        this->enemyField->canShoot(true);
        this->helloWindow->show();
        this->setEnableMenuBar(true);
        if (this->gameMode == GameMode::Multiplayer)
        {
            emit setMessageType(Message::Nothing);
            emit closeConnection();
            this->serverDisconnected();
        }
        delete msgBox;
    }
    this->updateStatDefeat();
}

void MainWindow::setEnableMenuBar(bool enable)
{
    this->ui->menubar->setEnabled(enable);
}

void MainWindow::on_actionInterrupt_triggered()
{
   this->ui->actionInterrupt->setEnabled(false);
   this->playerField->interrupted = true;
   this->enemyField->interrupted = true;
   this->updateStatDefeat();
}


void MainWindow::on_actionSound_triggered()
{
    this->soundSliderWindow->show();
}


void MainWindow::on_sendBtn_clicked()
{
    if (!this->ui->lineEdit->text().isEmpty())
    {
        this->localGameWindow->client->sendToServer(this->ui->lineEdit->text());
        this->ui->lineEdit->clear();
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    if (!this->ui->lineEdit->text().isEmpty())
    {
        this->localGameWindow->client->sendToServer(this->ui->lineEdit->text());
        this->ui->lineEdit->clear();
    }
}

PlayerField::PlayerField(QWidget *parent, const QPoint &position, const QSize &size):
    Field(parent, position, size)
{
    this->bot = new GameAlgorythm();
    this->delay = new QTimer();
    connect(this->delay, &QTimer::timeout, this, &PlayerField::makeMove);
}

PlayerField::~PlayerField()
{
    this->deleteAll();
    delete this->bot;
    delete this->delay;
}

void PlayerField::setFleet(Fleet *fleet)
{
    this->fleet = fleet;
}

void PlayerField::setDifficulty(Difficulty difficulty)
{
    this->bot->setDifficulty(difficulty);
}

Difficulty PlayerField::getDifficulty() const
{
    return this->bot->getDifficulty();
}

void PlayerField::startMove()
{
    this->delay->start(GameAlgorythm::random(500,800));
}


void PlayerField::makeMove()
{
    if (!this->interrupted)
    {
        if(!playAnimation)
        {
            sf::Vector2i botMove = this->bot->makeMove();
            switch (fleet->field[botMove.x][botMove.y]) {
            case  State::Ship:
            {
                fleet->field[botMove.x][botMove.y] = State::Hit;
                this->fleet->checkShip(this->fleet->indOfShip(botMove.x,botMove.y));

                this->bot->setCellState(botMove.x,botMove.y,fleet->field[botMove.x][botMove.y]);

                this->startAniamtion(botMove.x, botMove.y, State::Hit);
                break;
            }
            case  State::Hit: case State::Miss: case State::Dead:
            {
                this->delay->stop();
                this->delay->start(100);
                break;
            }
            default:
            {
                fleet->field[botMove.x][botMove.y] = State::Miss;
                this->bot->setCellState(botMove.x,botMove.y,fleet->field[botMove.x][botMove.y]);
                this->startAniamtion(botMove.x, botMove.y, State::Miss);
                break;
            }
            }
            this->delay->stop();
        }
        else
        {
            this->delay->stop();
            this->delay->start(100);
        }
    }
}

void PlayerField::deleteAll()
{
    Field::deleteAll();
    this->bot->clearAll();
}

void PlayerField::enemyShoot(sf::Vector2i coords)
{
    switch (fleet->field[coords.x][coords.y]) {
    case  State::Ship:
    {
        fleet->field[coords.x][coords.y] = State::Hit;
        this->fleet->checkShip(this->fleet->indOfShip(coords.x,coords.y));

        auto ind = fleet->indOfShip(coords.x,coords.y);
        if (!fleet->ships[ind]->isAlive())
        {
            QString command = "ShotResult Dead " + QString::number(fleet->ships[ind]->getPosition().x)
                                     + ' ' + QString::number(fleet->ships[ind]->getPosition().y)
                                     + ' ' + QString::number(fleet->ships[ind]->getSize());
            if (fleet->ships[ind]->getDirection() == Direction::Down)
                command += " Down";
            else
                command += " Right";

            emit sendCommandToServer(command);
        }
        else
            emit sendCommandToServer("ShotResult Hit");

        this->startAniamtion(coords.x, coords.y, State::Hit);
        break;
    }
    case  State::Hit: case State::Miss: case State::Dead:
    {
        emit sendCommandToServer("IncorrectCoordsOfAttack");
        break;
    }
    default:
    {
        emit sendCommandToServer("ShotResult Miss");
        fleet->field[coords.x][coords.y] = State::Miss;
        this->startAniamtion(coords.x, coords.y, State::Miss);
        break;
    }
    }

}

void PlayerField::update()
{

    Field::update();
    for (auto elem : fleet->ships) {
        RenderWindow::draw(elem->getSprite());
    }
    for (auto &elem : fires) {
        RenderWindow::draw(*elem);
    }
    if (playAnimation)
    {
        if (!missleFell)
            RenderWindow::draw(spriteMissle);
        else if (typeOfAnimation == State::Miss)
            RenderWindow::draw(spriteSplash);
    }
}

void PlayerField::initialize()
{
    Field::initialize();
    this->spriteMissle.setRotation(115.f);
    for (auto ship : this->fleet->ships)
    {
        ship->resetWithOffsetAllDirections(offset);
    }
}

void PlayerField::nextFrame()
{
    Field::nextFrame();
    this->spriteMissle.setPosition(this->spriteMissle.getPosition().x-5, this->spriteMissle.getPosition().y+10);
}

void PlayerField::startAniamtion(int i, int j, State state)
{
    emit setEnableMenuBar(false);
    Field::startAniamtion(i,j,state);
    this->spriteMissle.setPosition(i*cell_size+3*cell_size + offset,j*cell_size-4*cell_size + offset);
}

void PlayerField::stopAnimation()
{
    Field::stopAnimation();
    emit setEnableMenuBar(true);
    if (this->typeOfAnimation == State::Hit)
    {
        if (!fleet->ships[fleet->indOfShip(this->animationPosition.x,this->animationPosition.y)]->isAlive())
        {
            for (auto deadSq : fleet->getCoordsOfShipCells(fleet->indOfShip(this->animationPosition.x,this->animationPosition.y)))
            {
                this->bot->setCellState(deadSq.x, deadSq.y, State::Dead);
            }
            for (auto redSq : fleet->fillMissAroundShip(fleet->indOfShip(this->animationPosition.x,this->animationPosition.y)))
            {
                 this->bot->setCellState(redSq.x, redSq.y, State::Miss);
                 this->addRedSquare(redSq.x, redSq.y);
            }
          this->fleet->sizeOfFleet--;

        }
        if (this->fleet->sizeOfFleet == 0)
            emit endGame();
        else if (this->gameMode == GameMode::Bot)
            this->startMove();
    }
    else if (this->fleet->sizeOfFleet != 0)
    {
        emit playerCanShoot(true);
        emit rotateArrow();
    }
}

EnemyField::EnemyField(QWidget *parent, const QPoint &position, const QSize &size):
        Field(parent, position, size), gameStarted(false), playerCanShoot(true), shipStateMultiplayer(State::Free)
{
    this->cursor.loadFromSystem(sf::Cursor::Cross);
}

EnemyField::~EnemyField()
{
    this->Field::deleteAll();
    this->gameStarted = false;
}

void EnemyField::mousePressEvent(QMouseEvent *)
{
    if(!this->playAnimation && this->playerCanShoot && !this->interrupted)
    {
        int x = sf::Mouse::getPosition().x - RenderWindow::getPosition().x;
        int y = sf::Mouse::getPosition().y - RenderWindow::getPosition().y;

        int border_thickness = 1;
        int h_plates_count = 10;
        int v_plates_count = 10;

        for (int i = 0; i < v_plates_count; ++i)
            for (int j = 0; j < h_plates_count; ++j)
            {
                if (((x > (border_thickness + j * cell_size  + offset)) && (y > (border_thickness + i * cell_size + offset))) &&
                        ((x < (border_thickness + j * cell_size + cell_size + offset)) && (y < (border_thickness + i * cell_size + cell_size + offset))))
                {
                    switch (this->gameMode) {
                    case GameMode::Bot:
                    {
                        switch (fleet->field[j][i]) {
                        case  State::Ship:
                        {
                            this->fleet->field[j][i] = State::Hit;
                            this->startAniamtion(j, i, fleet->field[j][i]);
                            this->playerCanShoot = false;
                            if (!this->gameStarted)
                            {
                                this->gameStarted = true;
                                emit gameStartedStat();
                            }
                            break;
                        }
                        case  State::Hit: case State::Miss: case State::Dead:
                        {
                            break;
                        }
                        default:
                        {
                            this->fleet->field[j][i] = State::Miss;
                            this->startAniamtion(j, i, fleet->field[j][i]);
                            this->playerCanShoot = false;
                            if (!this->gameStarted)
                            {
                                this->gameStarted = true;
                                emit gameStartedStat();
                            }
                            break;
                        }
                        }
                        break;
                    }
                    case GameMode::Multiplayer:
                    {
                        switch (fleet->field[j][i])
                        {
                        case  State::Hit: case State::Miss: case State::Dead:
                        {
                            break;
                        }
                        default:
                        {
                            if (!this->gameStarted)
                            {
                                this->gameStarted = true;
                                emit gameStartedStat();
                            }
                            this->playerCanShoot = false;
                            emit sendCommandToServer("Shoot " + QString::number(j) + ' ' + QString::number(i));
                            this->coordsOfShot = sf::Vector2i(j,i);
                            break;
                        }
                        }
                        break;
                    }
                    default:
                        break;
                    }

                }
            }
    }
}

void EnemyField::canShoot(bool canShoot)
{
    this->playerCanShoot = canShoot;
}

void EnemyField::shotResult(QStringList result)
{
    if (result.at(0) == "Dead")//позиция_позиция_размер_направление
    {
        if (result.at(4) == "Down")
            this->fleet->addShip(new Ship(result.at(1).toInt(),result.at(2).toInt(),result.at(3).toInt(),Direction::Down));
        else
            this->fleet->addShip(new Ship(result.at(1).toInt(),result.at(2).toInt(),result.at(3).toInt(),Direction::Right));
        this->fleet->field[this->coordsOfShot.x][this->coordsOfShot.y] = State::Hit;
        this->fleet->ships[this->fleet->ships.size()-1]->resetWithOffsetAllDirections(offset);
        this->shipStateMultiplayer = State::Dead;
        this->startAniamtion(this->coordsOfShot.x, this->coordsOfShot.y, State::Hit);
    }
    else if (result.at(0) == "Hit")
    {
        this->fleet->field[this->coordsOfShot.x][this->coordsOfShot.y] = State::Hit;
        this->shipStateMultiplayer = State::Hit;
        this->startAniamtion(this->coordsOfShot.x, this->coordsOfShot.y, State::Hit);
    }
    else if (result.at(0) == "Miss")
    {
        this->fleet->field[this->coordsOfShot.x][this->coordsOfShot.y] = State::Miss;
        this->startAniamtion(this->coordsOfShot.x, this->coordsOfShot.y, State::Miss);
    }
}

void EnemyField::update()
{
    Field::update();
    if (this->interrupted)
        for (auto elem : fleet->ships)
        {
            RenderWindow::draw(elem->getSprite());
        }
    else
        for (auto elem : fleet->ships)
        {
            if (!elem->isAlive())
                RenderWindow::draw(elem->getSprite());
        }
    for (auto &elem : fires)
    {
        RenderWindow::draw(*elem);
    }
    if (playAnimation)
    {
        if (!missleFell)
            RenderWindow::draw(spriteMissle);
        else if (typeOfAnimation == State::Miss)
            RenderWindow::draw(spriteSplash);
    }
}

void EnemyField::initialize()
{
    Field::initialize();
    this->fleet->clearField();
    if (this->gameMode == GameMode::Bot)
    {
        this->fleet->randomLocation();
        for (auto ship : this->fleet->ships)
        {
            ship->resetWithOffsetAllDirections(offset);
        }
    }
    this->spriteMissle.setRotation(65.f);
    sf::Window::setMouseCursor(this->cursor);
}

void EnemyField::nextFrame()
{
    Field::nextFrame();
    this->spriteMissle.setPosition(this->spriteMissle.getPosition().x+6, this->spriteMissle.getPosition().y+10);
}

void EnemyField::startAniamtion(int i, int j, State state)
{
    emit setEnableMenuBar(false);
    Field::startAniamtion(i,j,state);
    this->spriteMissle.setPosition(i*cell_size-2*cell_size + offset,j*cell_size-4*cell_size + offset);
}

void EnemyField::stopAnimation()
{
    Field::stopAnimation();
    emit setEnableMenuBar(true);
    if (this->typeOfAnimation == State::Hit)
    {
        if (this->gameMode == GameMode::Bot)
            this->fleet->checkShip(this->fleet->indOfShip(this->animationPosition.x,this->animationPosition.y));

        else if (this->shipStateMultiplayer == State::Dead)
            this->fleet->ships[this->fleet->ships.size()-1]->setAlive(false);

        if ((this->gameMode == GameMode::Multiplayer && this->shipStateMultiplayer == State::Dead) || this->gameMode == GameMode::Bot)
            if (!fleet->ships[fleet->indOfShip(this->animationPosition.x,this->animationPosition.y)]->isAlive())
            {
                for (auto redSq : fleet->fillMissAroundShip(fleet->indOfShip(this->animationPosition.x,this->animationPosition.y)))
                {
                    this->addRedSquare(redSq.x, redSq.y);
                }
                this->fleet->sizeOfFleet--;
            }
        if (this->fleet->sizeOfFleet == 0)
            emit endGame();
        else
            playerCanShoot = true;
    }
    else if (this->fleet->sizeOfFleet != 0)
    {
        if (this->gameMode == GameMode::Bot)
            emit passMove();
        else
            emit sendCommandToServer("PassMove");
        emit rotateArrow();
    }
}

ArrowWidget::ArrowWidget(QWidget *parent, const QPoint &position, const QSize &size):
    SFMLWidget(parent, position, size)
{    
    this->color = sf::Color::White;
}

ArrowWidget::~ArrowWidget()
{

}

void ArrowWidget::reset()
{
    this->initialize();
}

void ArrowWidget::update()
{
    RenderWindow::draw(this->sprite);
    RenderWindow::draw(this->spriteFrame);
}

void ArrowWidget::initialize()
{
    this->texture.loadFromFile("images\\arrow.png");
    this->sprite.setTexture(this->texture);

    this->textureFrame.loadFromFile("images\\frame.png");
    this->spriteFrame.setTexture(this->textureFrame);
    this->spriteFrame.setScale(1.6,1.8);

    this->sprite.setRotation(180.f);
    this->sprite.setPosition(64,68);
}


void ArrowWidget::rotate()
{
    if (this->sprite.getRotation() == 180.f)
    {
        this->sprite.setRotation(0.f);
        this->sprite.setPosition(this->sprite.getPosition().x-64,this->sprite.getPosition().y-64);
    }
    else
    {
        this->sprite.setRotation(180.f);
        this->sprite.setPosition(this->sprite.getPosition().x+64,this->sprite.getPosition().y+64);
    }
}

Field::Field(QWidget *parent, const QPoint &position, const QSize &size):
    SFMLWidget(parent, position, size),
    interrupted(false),
    playAnimation(false),
    missleFell(false)

{
    this->fleet = new Fleet();

     spriteTimer = new QTimer();
     fireTimer = new QTimer();

     this->color = sf::Color::White;

     connect(spriteTimer, &QTimer::timeout, this, &Field::nextFrame);
     connect(fireTimer, &QTimer::timeout, this, &Field::nextFrameFire);
}

Field::~Field()
{
    this->Field::deleteAll();

    delete this->spriteTimer;
    delete this->fireTimer;
}

void Field::deleteFleet()
{
    this->fleet->deleteShips();
    this->fleet->clearField();
}

void Field::deleteAll()
{
    this->deleteFleet();
    for (auto damage : this->fires)
        {
             delete damage;
        }
        this->fires.clear();
    this->deleteRect();
}

void Field::update()
{
    for (auto i = 0; i < 6; i++)
         {
             for (auto j = 0; j < 6; j++)
             {
                 RenderWindow::draw(this->spriteBack);
                 this->spriteBack.setPosition(i*2*cell_size,j*2*cell_size);
             }
         }

    auto ind = 0; auto num = 1;
    for (auto i = 0; i < 10; i++)
    {
        for (auto j = 0; j < 10; j++)
        {
            this->spriteCell.setPosition(i*cell_size+1 + offset, j*cell_size+1 + offset);

            RenderWindow::draw(this->spriteCell);
            if (j == 0)
            {
                this->text.setString(this->str[ind]);
                this->text.setPosition(i*cell_size + offset + 12, j*cell_size - cell_size/2);
                 RenderWindow::draw(this->text);
                ind++;
            }
            if (i == 0)
            {
                this->text.setString(std::to_string(num));
                if (num == 10)
                    this->text.setPosition(i*cell_size + 2, j*cell_size + cell_size/2);
                else
                    this->text.setPosition(i*cell_size + 12, j*cell_size + cell_size/2);
                RenderWindow::draw(this->text);
                num++;
            }
        }

    }

    for (auto &elem : this->rectangles) {
        RenderWindow::draw(*elem);
    }
}

void Field::initialize()
{
    SFMLWidget::initialize();

    this->fleet->sizeOfFleet = 10;
    this->interrupted = false;

    this->textureMissle.loadFromFile("images\\missle.png");
    this->spriteMissle.setTexture(this->textureMissle);

    this->textureSplash.loadFromFile("images\\Splash.png");
    this->spriteSplash.setTexture(this->textureSplash);
    this->spriteSplash.setScale(1.4,1.4);

    this->textureExpl.loadFromFile("images\\fire.png");

    this->textureBack.loadFromFile("images\\sea.jpg");
    this->spriteBack.setTexture(this->textureBack);
    this->spriteBack.setScale(0.09,0.09);

    this->text.setCharacterSize(50);
    this->font.loadFromFile("images\\Samson.ttf");
    this->text.setFont(this->font);
    this->text.setFillColor(sf::Color());

    this->str = "ABCDEFGHIJ";
}

void Field::addRedSquare(int i, int j)
{
    sf::Vector2f coords(this->border_thickness + i * cell_size + offset, this->border_thickness + j * cell_size + offset);
    sf::RectangleShape *rect = new sf::RectangleShape(sf::Vector2f(cell_size, cell_size));
    rect->setPosition(coords);
    rect->setFillColor(sf::Color(196,30,58,150));
    rectangles.push_back(rect);
}

void Field::nextFrame()
{
    if (spriteMissle.getPosition().y >= this->animationPosition.y*cell_size)
    {

        if (!this->missleFell)
            this->spriteRect = sf::Vector2i(0,0);

        if (this->spriteRect.x == 4)
            this->stopAnimation();

        this->missleFell = true;

        if (this->typeOfAnimation == State::Miss)
            this->spriteSplash.setTextureRect(sf::IntRect(32*this->spriteRect.x,0,32,32));

        else
        {
            this->spriteRect.x = 3;
        }

        this->spriteRect.x += 1;
    }
    else
    {
        if (this->spriteRect.x == 4)
        {
            if (this->spriteRect.y == 1)
            {
                this->spriteRect.y = 0;
                this->spriteRect.x = 1;
            }
            else
            {
                this->spriteRect.y = 1;
                this->spriteRect.x = 0;
            }
        }
        this->spriteMissle.setScale(this->spriteMissle.getScale().x-0.005, this->spriteMissle.getScale().y-0.005);
        this->spriteMissle.setTextureRect(sf::IntRect(350*this->spriteRect.x,80*this->spriteRect.y,350,80));
        this->spriteRect.x += 1;
    }
}

void Field::nextFrameFire()
{
    for (auto fire: this->fires)
    {
        int offset = fire->getTextureRect().left + 146;
        if (offset > 438)
            offset = 0;
        fire->setTextureRect(sf::IntRect(offset,0,146,200));
    }
}

void Field::startAniamtion(int i, int j, State state)
{
    this->typeOfAnimation = state;

    this->spriteTimer->start(25);
    this->animationPosition = sf::Vector2i(i,j);
    this->spriteRect = sf::Vector2i(0,0);

    this->spriteMissle.setTextureRect(sf::IntRect(0,0,350,80));
    spriteMissle.setScale(0.2,0.2);
    this->missleFell = false;

    if (this->typeOfAnimation == State::Miss)
        this->spriteSplash.setPosition(i*cell_size + offset,j*cell_size + offset);

    this->playAnimation=true;
}

void Field::stopAnimation()
{
    this->playAnimation = false;
    this->spriteTimer->stop();
    if (this->typeOfAnimation == State::Miss)
    {
        this->addRedSquare(this->animationPosition.x, this->animationPosition.y);
    }
    else
    {
        if (fires.size() == 0)
        {
            fireTimer->start(800);
        }

        sf::Sprite* fire = new sf::Sprite;
        fire->setTexture(textureExpl);
        fire->setScale(0.3,0.3);
        fire->setPosition(this->animationPosition.x * cell_size + offset,(this->animationPosition.y-1)*cell_size + 10 + offset);
        fire->setTextureRect(sf::IntRect(0,0,146,200));
        fires.push_back(fire);       
    }
}

