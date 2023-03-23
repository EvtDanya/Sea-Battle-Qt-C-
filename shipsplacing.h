#pragma once
#include <QComboBox>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QMessageBox>

#include "sfmlwidget.h"
#include "ship.h"
#include "fleet.h"
#include "gamealgorythm.h"

namespace Ui
{
    class ShipsPlacing;
    class FieldPlacing;
    class BotSetup;
}

class FieldPlacing : public SFMLWidget
{
    Q_OBJECT
public:
    FieldPlacing(QWidget *parent, const QPoint &position = QPoint(0,0), const QSize &size = QSize(0,0));
    ~FieldPlacing();

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    int getCountOfShips() const;
    int getSizeOfFleet() const;

    void fillRed();

    void randomLocation();
    void restore();
    void deleteFleet();
    void deleteShips();
    Fleet* getFleet() const;
private:
    virtual void update() override ;
    virtual void initialize() override;

    QVector<Ship*> ships;

    int sizeOfNewShip;
    int max;

    int indOfShip;

    enum Direction previousDirection;

    sf::Texture textureShipyard;
    sf::Sprite spriteShipyard;

    sf::Texture textureBack;
    sf::Sprite spriteBack;

    Fleet *fleet;

    sf::Cursor cursor;
    sf::Cursor defaultCursor;

    enum class M_State
    {
        Nothing, MoveInsideField, AddShipToField
    };
    enum M_State move;

signals:
    void sendData(bool enable);
};

class BotSetup : public QDialog
{
    Q_OBJECT
public:
    explicit BotSetup(QWidget *parent = nullptr);

    void setItems(QStringList strings);
     QComboBox* comboBox();
signals:

private:
    QComboBox *box;

};

enum class GameMode
{
    Bot, Multiplayer
};

class ShipsPlacing : public QWidget
{
    Q_OBJECT

public:
    explicit ShipsPlacing(QWidget *parent = nullptr);
    ~ShipsPlacing();

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    enum GameMode gameMode;

    QMessageBox *mb;
public slots:
    void shipsPlaced();
    void clearFleetInShipsPlacing();

signals:
    void firstWindow();
    void helloWindow();
    void sendFleet(Fleet* fleet);
    void sendDifficulty(Difficulty difficulty);

    void saveStatToFile();
    void saveConfigToFile();

    void sendCommandToServer(QString);
    void closeConnection();
private slots:
    void on_CancelBtn_clicked();

    void on_ConfirmBtn_clicked();

    void on_RestoreBtn_clicked();

    void receiveData(bool enable);

    void on_RandomBtn_clicked();

private:
    Ui::ShipsPlacing *ui;
    FieldPlacing *field;
    BotSetup *dialog;
};

