#include "statwindow.h"
#include "ui_statwindow.h"

StatWindow::StatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    this->timer = new QTimer();
    connect(this->timer, &QTimer::timeout, this, &StatWindow::checkRank);

    this->stat = new Statistics();
    this->stat->loadFromFile();

    ui->OkBtn->setStyleSheet("QPushButton#OkBtn {"
                               "background-color: green;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");

    ui->DeleteBtn->setStyleSheet("QPushButton#DeleteBtn {"
                               "background-color: red;"
                               "border-style: outset;"
                               "border-width: 2px;"
                               "border-radius: 10px;"
                               "border-color: beige;"
                               "font: bold 20px;"
                               "padding: 6px;""}");

    this->ranks =
    {
        {0,"Private"},
        {1,"Corporal"},
        {2,"Third Sergeant"},
        {3,"Second Sergeant"},
        {4,"First Sergeant"},
        {6,"Sergeant"},
        {10,"Second Warrant Officer"},
        {12,"First Warrant Officer"},
        {14,"Third Lieutenant"},
        {16,"Second Lieutenant"},
        {18,"First Lieutenant"},
        {22,"Captain"},
        {25,"Major"},
        {28,"Lieutenant Colonel"},
        {31,"Colonel"},
        {36,"Brigadier General"},
        {41,"Major General"},
        {47,"Lieutenant General"},
        {54,"General of the Army"},
    };
}

StatWindow::~StatWindow()
{
    delete this->ui;
    delete this->stat;
    delete this->timer;
}

void StatWindow::showEvent(QShowEvent *event)
{
    uint16_t gamesMultiplayer = this->stat->defeatsMultiplayer + this->stat->winsMultiplayer;
    uint16_t gamesEasyBot = this->stat->defeatsEasyBot + this->stat->winsEasyBot;
    uint16_t gamesMediumBot = this->stat->defeatsMediumBot + this->stat->winsMediumBot;

    this->ui->defeats->setText(QString::number(this->stat->defeatsMultiplayer));
    this->ui->defeats_2->setText(QString::number(this->stat->defeatsEasyBot));
    this->ui->defeats_3->setText(QString::number(this->stat->defeatsMediumBot));

    if (gamesMultiplayer != 0)
        this->ui->kd->setText(QString::number(double(this->stat->winsMultiplayer)/double(gamesMultiplayer)*100,'f',2)+" %");
    else
        this->ui->kd->setText("0 %");

    if (gamesEasyBot != 0)
        this->ui->kd_2->setText(QString::number(double(this->stat->winsEasyBot)/double(gamesEasyBot)*100,'f',2)+" %");
    else
        this->ui->kd_2->setText("0 %");

    if (gamesMediumBot != 0)
        this->ui->kd_3->setText(QString::number(double(this->stat->winsMediumBot)/double(gamesMediumBot)*100,'f',2)+" %");
    else
        this->ui->kd_3->setText("0 %");

    this->ui->wins->setText(QString::number(this->stat->winsMultiplayer));
    this->ui->wins_2->setText(QString::number(this->stat->winsEasyBot));
    this->ui->wins_3->setText(QString::number(this->stat->winsMediumBot));

    this->ui->unfinishedGames->setText(QString::number(this->stat->unfinishedGames));
    this->ui->totallyPlayed->setText(QString::number(gamesMultiplayer+gamesEasyBot+gamesMediumBot+this->stat->unfinishedGames));

    if (this->stat->rank <= 18)
    {
        QPixmap pixmap("images\\ranks\\" + QString::number(this->stat->rank+1) + ".png");
        this->ui->label->setPixmap(pixmap);
        this->ui->label->setScaledContents(true);
        this->ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

        this->ui->rankName->setText(QString::fromStdString(this->ranks.at(this->stat->rank).name));

        if (this->stat->rank < 18)
            this->ui->winsLeft->setText(QString::number(this->ranks.at(this->stat->rank+1).winsToGet -
                                                        (this->stat->winsMultiplayer+this->stat->winsEasyBot+this->stat->winsMediumBot)));
        else
            this->ui->winsLeftLab->setText("");
    }
    else
    {
        this->ui->label->setText("Error! Unknown number of rank!");
        this->ui->rankName->setText("Error!");
    }

    QWidget::showEvent(event);
}

void StatWindow::closeEvent(QCloseEvent *event)
{
    this->stat->saveToFile();
    emit saveConfigToFile();
    event->accept();
}

void StatWindow::on_OkBtn_clicked()
{
    this->hide();
    emit helloWindow();
}


void StatWindow::on_DeleteBtn_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Attention!");
    msgBox.setText("Are you sure you want to delete statistics?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Question);

    msgBox.setDefaultButton(QMessageBox::Ok);
    int res = msgBox.exec();
    if (res == QMessageBox::Ok)
    {
        this->stat->reset();
        this->hide();
        emit helloWindow();
    }
}

void StatWindow::receiveStat(Statistics *stat)
{
    this->stat = stat;
}

void StatWindow::saveStatToFile()
{
    this->stat->saveToFile();
}

void StatWindow::checkRankDelay()
{
    this->timer->start(50);
}

void StatWindow::checkRank()
{
    uint16_t wins = this->stat->winsMultiplayer+this->stat->winsEasyBot+this->stat->winsMediumBot;
    if (this->stat->rank > 18)
    {
        this->stat->rank = 18;
    }
    if (wins < this->ranks.at(this->stat->rank).winsToGet)
    {
        emit sendRankCheckedInfo(RankChecked::Downgrading);
        for (auto i = this->ranks.size()-1; i != 0; i--)
        {
            if (wins >= this->ranks.at(i).winsToGet)
            {
                this->stat->rank = i;
                break;
            }
        }
    }
    else
        for (auto i = this->ranks.size()-1; i > this->stat->rank; i--)
        {
            if (wins >= this->ranks.at(i).winsToGet)
            {
                this->stat->rank = i;
                emit sendRankCheckedInfo(RankChecked::NewRank);
                break;
            }
        }
}

