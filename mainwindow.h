#pragma once

#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>
#include <QResizeEvent>
#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QLabel>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <string>
#include <QToolButton>

#include "shipsplacing.h"
#include "hellowindow.h"
#include "sfmlwidget.h"
#include "gamealgorythm.h"
#include "localgamewindow.h"
#include "statwindow.h"
#include "statistics.h"
#include "sound.h"

extern int offset;
extern int countOfMusicTracks;

namespace Ui
{
    class MainWindow;
    class PlayerField;
    class EnemyField;
    class ArrowWidget;
    class Field;    
}


class Field : public SFMLWidget
{
    Q_OBJECT
public:
    Field(QWidget *parent, const QPoint &position = QPoint(0,0), const QSize &size = QSize(0,0));
    ~Field();

    void deleteFleet();
    virtual void deleteAll();

    bool interrupted;
    enum GameMode gameMode;
signals:
    void rotateArrow();
    void endGame();
    void sendCommandToServer(QString);

protected:
    virtual void update() override ;
    virtual void initialize() override;

    sf::Texture textureBack;
    sf::Sprite spriteBack;

    sf::Font font;
    sf::Text text;

    Fleet *fleet;

    void addRedSquare(int i, int j);

    sf::Texture textureMissle;
    sf::Sprite spriteMissle;

    sf::Texture textureSplash;
    sf::Sprite spriteSplash;

    sf::Texture textureExpl;

    bool playAnimation;
    bool missleFell;

    QTimer *spriteTimer;
    QTimer *fireTimer;

    virtual void nextFrame();
    void nextFrameFire();

    virtual void startAniamtion(int i, int j, State state);
    virtual void stopAnimation();

    sf::Vector2i animationPosition;
    sf::Vector2i spriteRect;

    State typeOfAnimation;

    QVector<sf::Sprite*> fires;

    std::string str;
};


class ArrowWidget : public SFMLWidget
{
    Q_OBJECT
public:
    ArrowWidget(QWidget *parent, const QPoint &position = QPoint(0,0), const QSize &size = QSize(0,0));
    ~ArrowWidget();

    void reset();
private:
    virtual void update() override ;
    virtual void initialize() override;

    sf::Texture texture;
    sf::Sprite sprite;

    sf::Texture textureFrame;
    sf::Sprite spriteFrame;

public slots:
    void rotate();
};

class PlayerField : public Field
{
    Q_OBJECT
public:
    PlayerField(QWidget *parent, const QPoint &position = QPoint(0,0), const QSize &size = QSize(0,0));
    ~PlayerField();

    void setFleet(Fleet *fleet);
    void setDifficulty(Difficulty difficulty);
    Difficulty getDifficulty() const;

    void startMove();
    void makeMove(); 
    virtual void deleteAll() override;

public slots:
    void enemyShoot(sf::Vector2i);

signals:
    void playerCanShoot(bool);
    void setEnableMenuBar(bool enable);

private:
    virtual void update() override ;
    virtual void initialize() override;

    virtual void nextFrame() override;
    virtual void startAniamtion(int i, int j, State state) override;
    virtual void stopAnimation() override;

    GameAlgorythm *bot;
    QTimer *delay;
};

class EnemyField : public Field
{
    Q_OBJECT
public:
    EnemyField(QWidget *parent, const QPoint &position = QPoint(0,0), const QSize &size = QSize(0,0));
    ~EnemyField();

    virtual void mousePressEvent(QMouseEvent *) override;
    bool gameStarted;

public slots:
    void canShoot(bool);
    void shotResult(QStringList);

signals:
    void passMove();
    void setEnableMenuBar(bool enable);
    void gameStartedStat();

private:
    virtual void update() override ;
    virtual void initialize() override;

    virtual void nextFrame() override;
    virtual void startAniamtion(int i, int j, State state) override;
    virtual void stopAnimation() override;

    bool playerCanShoot;

    sf::Cursor cursor;
    sf::Vector2i coordsOfShot;

    enum State shipStateMultiplayer;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    enum GameMode gameMode;

    void updateStatDefeat();
    void updateStatWin();

    void startGame();
signals:
    void closeConnection();
    void messageAboutDisconnect();
    void clearFleetInShipsPlacing();
    void setMessageType(Message);

public slots:
    void volumeChanged(uint8_t volume);
    void saveConfigToFile();

    void isMultiplayer(bool isMultiplayer);

    void addTextChat(QString);

    void secondPlayer();

    void serverDisconnected();

private slots:
    void on_actionExit_triggered();
    void on_actionHelp_triggered();
    void setEnableMenuBar(bool enable);
    void on_actionInterrupt_triggered();
    void on_actionSound_triggered();
    void on_sendBtn_clicked();
    void on_lineEdit_returnPressed();

    void receiveFleet(Fleet* fleet);
    void receiveDifficulty(Difficulty difficulty);

    void winWindow();
    void loseWindow();

private:
    Ui::MainWindow *ui;

    ShipsPlacing *shipsPlacing;
    HelloWindow *helloWindow;
    LocalGameWindow *localGameWindow;

    PlayerField *playerField;
    EnemyField *enemyField;
    ArrowWidget *arrow;

    StatWindow *statWindow;

    SoundSliderWindow *soundSliderWindow;
    Music *music;

    Config *config;
};
