#include "localgamewindow.h"
#include "ui_localgamewindow.h"

Client::Client(QWidget *parent):nextBlockSize(0)
{
    this->tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::slotReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, tcpSocket, &QTcpSocket::close);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::serverDisconnected);
}

Client::~Client()
{
    delete tcpSocket;
}

void Client::connectToServer(QHostAddress address,uint16_t port)
{
    this->tcpSocket->connectToHost(address, port);
}

void Client::slotReadyRead()
{
    QDataStream in(this->tcpSocket);
    in.setVersion(QDataStream::Qt_6_4);
    if (in.status() == QDataStream::Ok)
    {
        for (;;)
        {
            if (this->nextBlockSize == 0)
            {
                if (this->tcpSocket->bytesAvailable() < 2)
                    break;
                in >> nextBlockSize;
            }
            if (this->tcpSocket->bytesAvailable() < this->nextBlockSize)
                break;
            quint8 typeOfData;
            QString str;
            this->nextBlockSize = 0;
            in >> typeOfData;
            switch (typeOfData)
            {
                case 0:
                {
                    QTime time;
                    in >> time >> str;
                    emit addTextChat(time.toString() + str);
                    break;
                }
                case 1:
                {
                    in >> str;
                    QStringList pieces = str.split(" ");
                    if (pieces.at(0) == "ClientDisconnected")
                    {
                        emit setMessageType(Message::OpponentDisconnected);
                        this->tcpSocket->close();
                    }
                    else if (pieces.at(0) == "CanPlaceShips")
                    {
                        emit hideConnectionWindow();
                        emit shipsPlacing();
                    }
                    else if (pieces.at(0) == "FirstPlayer")
                    {
                        emit gameWindow();
                        emit addTextChat("You are starting first!");
                    }
                    else if (pieces.at(0) == "SecondPlayer")
                    {
                        emit gameWindow();
                        emit addTextChat("Your opponent are starting first!");
                        emit secondPlayer();
                    }
                    else if (pieces.at(0) == "Shoot")
                    {
                        emit shoot(sf::Vector2i(pieces.at(1).toInt(),pieces.at(2).toInt()));
                    }
                    else if (pieces.at(0) == "ShotResult")
                    {
                        pieces.pop_front();
                        emit shotResult(pieces);
                    }
                    else if (pieces.at(0) == "IncorrectCoordsOfAttack")
                    {
                        emit playerCanShoot(true);
                    }
                    else if (pieces.at(0) == "PassMove")
                    {
                        emit playerCanShoot(true);
                    }
                    else
                    {
                        emit addTextChat("[LOG] Unknown command from server >> " + pieces.at(0));
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }
    else
    {
        emit addTextChat("Read error!");
    }
}

void Client::sendToServer(QString str)
{
    this->data.clear();
    QDataStream out(&this->data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << quint8(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(data.size()-sizeof(quint16));
    this->tcpSocket->write(this->data);
}

void Client::sendCommandToServer(QString str)
{
    this->data.clear();
    QDataStream out(&this->data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << quint8(1) << str;
    out.device()->seek(0);
    out << quint16(data.size()-sizeof(quint16));
    this->tcpSocket->write(this->data);
}

QAbstractSocket::SocketState Client::checkSocket() const
{
    return this->tcpSocket->state();
}

void Client::disconnect()
{
    this->tcpSocket->close();
}

void Client::updateSocket()
{
    delete this->tcpSocket;
    this->tcpSocket = new QTcpSocket(this);
}

LocalGameWindow::LocalGameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalGameWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    //строка для регулярного выражения для проверки ip
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";

    QRegularExpression ipRegex ("^" + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange + "$");

    auto *ipValidator = new QRegularExpressionValidator(ipRegex,this);
    this->ui->ipaddress->setValidator(ipValidator);

    auto nicknameRegex = QRegularExpression("^[a-zA-Zа-яА-Я0-9]{1,20}$");
    auto *nicknameValidator = new QRegularExpressionValidator(nicknameRegex,this);
    this->ui->nickname->setValidator(nicknameValidator);

    this->client = new Client();
    this->serverProgram = new QProcess(this);
    this->serverProgram->waitForFinished(-1);

    this->checkConnectionTimer = new QTimer();
    connect(this->checkConnectionTimer, &QTimer::timeout, this, &LocalGameWindow::checkConnection);

    this->mb = new QMessageBox(this);
    this->mb->setWindowTitle("Waiting");
    this->mb->setText("Waiting for your opponent and a response from the server!");
    this->mb->setIcon(QMessageBox::Information);

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
                               "font: bold 18px;"
                               "padding: 6px;""}");

    ui->startServerBtn->setStyleSheet("QPushButton#startServerBtn {"
                               "background-color: #F75E25;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 18px;"
                               "padding: 6px;""}");
}

LocalGameWindow::~LocalGameWindow()
{
    delete ui;
    delete serverProgram;
    delete client;
    delete checkConnectionTimer;
    delete mb;
}

void LocalGameWindow::showEvent(QShowEvent *event)
{
    if (this->client->checkSocket() != QTcpSocket::ConnectedState)
        this->ui->ConfirmBtn->setEnabled(true);
    QWidget::showEvent(event);
}

void LocalGameWindow::closeEvent(QCloseEvent *event)
{
    emit saveStatToFile();
    emit saveConfigToFile();
    event->accept();
}

void LocalGameWindow::checkConnection()
{
    if (this->client->checkSocket() == QTcpSocket::ConnectedState)
    {
        this->client->sendCommandToServer("SetNickname \"" + this->ui->nickname->text() + "\"");
        mb->show();
    }
    else
    {
        this->ui->ConfirmBtn->setEnabled(true);
         QMessageBox::warning(this,"Connection error","Couldn't connect to the server!\nCheck entered ip address, port and whether the server is running");
    }
    this->checkConnectionTimer->stop();
}

void LocalGameWindow::on_ConfirmBtn_clicked()
{
    if (this->ui->nickname->text().size() == 0)
    {
        QMessageBox::warning(this,"Attention","Nickname not entered!");
        return;
    }
    this->client->connectToServer(QHostAddress(this->ui->ipaddress->text()),this->ui->portnumb->value());
    this->ui->ConfirmBtn->setEnabled(false);
    this->checkConnectionTimer->start(200);
}

void LocalGameWindow::on_CancelBtn_clicked()
{
    if (this->client->checkSocket() == QTcpSocket::ConnectedState)
    {
        emit setMessageType(Message::Nothing);
        this->client->disconnect();
    }
    this->hide();
    emit helloWindow();
}

void LocalGameWindow::on_startServerBtn_clicked()
{
   this->serverProgram->start("Server.exe");
}

