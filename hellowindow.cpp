#include "hellowindow.h"
#include "ui_hellowindow.h"

HelloWindow::HelloWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelloWindow), message(Message::ServerDisconnected)
{
    ui->setupUi(this);

    QPixmap bkgnd("images\\background.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    ui->PlayBtn->setStyleSheet("QPushButton#PlayBtn {"
                               "background-color: #A8E4A0;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "min-width: 10em;"
                               "padding: 6px;""}");

    ui->LocGameBtn->setStyleSheet("QPushButton#LocGameBtn {"
                               "background-color: #7B68EE;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "min-width: 10em;"
                               "padding: 6px;""}");

    ui->ExitBtn->setStyleSheet("QPushButton#ExitBtn {"
                               "background-color: red;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "min-width: 10em;"
                               "padding: 6px;""}");

    ui->StatBtn->setStyleSheet("QPushButton#StatBtn {"
                               "background-color: #FFCC00;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "min-width: 10em;"
                               "padding: 6px;""}");

    ui->volumeBtn->setStyleSheet("QPushButton#volumeBtn {"
                               //"background-color: red;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");
}

HelloWindow::~HelloWindow()
{
    delete ui;
}

void HelloWindow::resizeEvent(QResizeEvent *event)
{
    QPixmap bkgnd("images\\background.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    QWidget::resizeEvent(event);
}

void HelloWindow::closeEvent(QCloseEvent *event)
{
    emit saveStatToFile();
    emit saveConfigToFile();
    event->accept();
}

void HelloWindow::showEvent(QShowEvent *event)
{
    this->setSoundBtnIcon();
    QWidget::showEvent(event);
    emit checkRank();
}

void HelloWindow::receiveRankCheckedInfo(RankChecked rankChecked)
{
    switch (rankChecked) {
    case RankChecked::NewRank:
    {
        QMessageBox::information(this,"Congratulations!","Now you have new rank!\nCheck it in statistics");
        break;
    }
    case RankChecked::Downgrading:
    {
        QMessageBox::warning(this,"-100 social rating!","Cheating is so bad!!!\nWe downgraded you for this!");
        break;
    }
    default:
        break;
    }
}

void HelloWindow::sliderMoved(uint8_t volume)
{
    if (volume == 0 && this->isVolumeOn)
    {
        this->isVolumeOn = false;
        this->setSoundBtnIcon();
    }
    else if (volume != 0 && !this->isVolumeOn)
    {
        this->isVolumeOn = true;
        this->setSoundBtnIcon();
    }
}

void HelloWindow::messageAboutDisconnect()
{
    switch(this->message)
    {
    case Message::OpponentDisconnected:
    {
        QMessageBox::warning(this,"Info","Your opponent disconnected!\nTry to connect again!");
        break;
    }
    case Message::ServerDisconnected:
    {
        QMessageBox::warning(this,"Info","The server is not responding!\nTry to connect again!");
        break;
    }
    default:
        break;
    }
    this->message=Message::ServerDisconnected;
}

void HelloWindow::setMessage(Message msg)
{
    this->message = msg;
}

void HelloWindow::on_PlayBtn_clicked()
{
    this->hide();
    emit isMultiplayer(false);
    emit shipsPlacingWindow();
}

void HelloWindow::on_ExitBtn_clicked()
{
    QApplication::quit();
}

void HelloWindow::on_LocGameBtn_clicked()
{
    this->hide();
    emit isMultiplayer(true);
    emit localGameWindow();
}

void HelloWindow::on_StatBtn_clicked()
{
    this->hide();
    emit statWindow();
}

void HelloWindow::on_volumeBtn_clicked()
{
    if (this->isVolumeOn)
    {
        emit volumeChanged(0);
        emit moveSlider(0);
    }
    else
    {
        emit volumeChanged(100);
        emit moveSlider(100);
    }

    this->isVolumeOn = !this->isVolumeOn;
    this->setSoundBtnIcon();
}

void HelloWindow::setSoundBtnIcon()
{
    if (this->isVolumeOn)
    {
        QPixmap pixmap("images\\sound.png");
        QIcon ButtonIcon(pixmap);
        this->ui->volumeBtn->setIcon(ButtonIcon);
        this->ui->volumeBtn->setIconSize(pixmap.rect().size());
    }
    else
    {
        QPixmap pixmap("images\\mute.png");
        QIcon ButtonIcon(pixmap);
        this->ui->volumeBtn->setIcon(ButtonIcon);
        this->ui->volumeBtn->setIconSize(pixmap.rect().size());
    }
}

