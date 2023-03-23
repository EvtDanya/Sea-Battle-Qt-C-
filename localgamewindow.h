#pragma once
#include "hellowindow.h"
#include "SFML/System/Vector2.hpp"
#include <QWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>
#include <QDir>
#include <QRegularExpressionValidator>
#include <QTime>
#include <QTimer>

namespace Ui
{
    class LocalGameWindow;
    class Client;
}

class Client: public QWidget
{
    Q_OBJECT
public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

    void connectToServer(QHostAddress,uint16_t);
    void sendToServer(QString);  
    QAbstractSocket::SocketState checkSocket() const;


public slots:
    void slotReadyRead();
    void sendCommandToServer(QString);
    void disconnect();
    void updateSocket();

signals:
    void addTextChat(QString);
    void shipsPlacing();
    void hideConnectionWindow();
    void gameWindow();
    void secondPlayer();
    void shotResult(QStringList);
    void shoot(sf::Vector2i);
    void playerCanShoot(bool);
    void opponentDisconnected();
    void serverDisconnected();
    void setMessageType(Message);

private:
    QTcpSocket *tcpSocket;
    QByteArray data;
    quint16 nextBlockSize;
};

class LocalGameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LocalGameWindow(QWidget *parent = nullptr);
    ~LocalGameWindow();

    virtual void showEvent(QShowEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

    Client *client;
    void checkConnection();

    QMessageBox *mb;
signals:

    void helloWindow();

    void saveStatToFile();
    void saveConfigToFile();

    void setMessageType(Message);

private slots:
    void on_ConfirmBtn_clicked();

    void on_CancelBtn_clicked();

    void on_startServerBtn_clicked();

private:
    Ui::LocalGameWindow *ui;
    QProcess *serverProgram;
    QTimer *checkConnectionTimer;
};


