#pragma once

#include <QCloseEvent>
#include <QWidget>
#include <QMessageBox>
#include <QPixmap>

#include "statistics.h"
namespace Ui
{
    class HelloWindow;
    enum class Message;
}

enum class Message
{
    OpponentDisconnected, ServerDisconnected, Nothing
};

class HelloWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HelloWindow(QWidget *parent = nullptr);
    ~HelloWindow();

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    bool isVolumeOn;

public slots:
    void receiveRankCheckedInfo(RankChecked rankChecked);
    void sliderMoved(uint8_t volume);
    void messageAboutDisconnect();
    void setMessage(Message);

signals:
    void shipsPlacingWindow();
    void localGameWindow();
    void statWindow();
    void checkRank();

    void saveStatToFile();
    void saveConfigToFile();

    void volumeChanged(uint8_t);
    void moveSlider(uint8_t);

    void isMultiplayer(bool);

private slots:
    void on_PlayBtn_clicked();

    void on_ExitBtn_clicked();

    void on_LocGameBtn_clicked();

    void on_StatBtn_clicked();

    void on_volumeBtn_clicked();

    void setSoundBtnIcon();

private:
    Ui::HelloWindow *ui;
    enum Message message;

};

