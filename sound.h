#pragma once

#include <QWidget>
#include <QMediaPlayer>
//#include <QMediaPlaylist>
#include <QAudioOutput>
#include <QVector>
#include <QDebug>
namespace Ui
{
    class SoundSliderWindow;
    class SoundTumbler;
}

class Music : public QWidget
{
     Q_OBJECT
public:
    Music(QWidget *parent = nullptr);
    ~Music();

    void start();
    void stop();
    void setVolume(int volume);
    void setCurrentMusic(int index);

public slots:
    void changeMusic(QMediaPlayer::PlaybackState);

private:
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVector<QString> pathsToMusic;
    int currentMusic;
};

class SoundSliderWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SoundSliderWindow(QWidget *parent = nullptr);
    ~SoundSliderWindow();

    bool isVolumeOn;
    bool canChangeBtn;
public slots:
    void setVolume(uint8_t volume);
    void setVolumeIcon(bool isVolumeOn);

signals:
    void sliderMoved(uint8_t);
    void setVolumeBtnIcon(uint8_t);

private slots:
    void on_volumeSlider_valueChanged(int value);

private:
    Ui::SoundSliderWindow *ui;


};

