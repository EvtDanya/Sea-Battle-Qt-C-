#include "sound.h"
#include "ui_soundsliderwindow.h"

Music::Music(QWidget *parent):currentMusic(0)
{
    this->mediaPlayer = new QMediaPlayer;
    this->audioOutput = new QAudioOutput;
    this->mediaPlayer->setAudioOutput(audioOutput);

    this->pathsToMusic = {
        "barbossa_is_hungry",
        "beyond_my_beloved_horizon",
        "blackbeard",
        "hes_a_pirate",
        "hopper",
        "jack_sparrow",
        "moonlight_serenade",
        "the_medallion_calls",
        "we_have_a_battleship",
    };

    connect(this->mediaPlayer, &QMediaPlayer::playbackStateChanged,this,&Music::changeMusic);
}

Music::~Music()
{
    delete this->mediaPlayer;
    delete this->audioOutput;
}

void Music::start()
{
    this->mediaPlayer->play();
}

void Music::stop()
{
    this->mediaPlayer->stop();
}

void Music::setVolume(int volume)
{
    this->audioOutput->setVolume(float(volume)/100);
}

void Music::setCurrentMusic(int index)
{
    if (index > 0 && index < pathsToMusic.size())
        this->currentMusic = index;

    this->mediaPlayer->setSource(QUrl::fromLocalFile("sounds\\"+this->pathsToMusic.at(this->currentMusic)+".mp3"));

}

void Music::changeMusic(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::PlaybackState::StoppedState)
    {
        this->currentMusic++;
        if (this->currentMusic >= this->pathsToMusic.size())
            this->currentMusic=0;
        this->mediaPlayer->setSource(QUrl::fromLocalFile("sounds\\"+this->pathsToMusic.at(this->currentMusic)+".mp3"));
        this->mediaPlayer->play();
    }
}


SoundSliderWindow::SoundSliderWindow(QWidget *parent) :
    QWidget(parent), canChangeBtn(false),
    ui(new Ui::SoundSliderWindow)

{
    ui->setupUi(this);
    this->setFixedSize(this->size());

}

void SoundSliderWindow::setVolume(uint8_t volume)
{
    this->canChangeBtn = false;
    this->ui->volumeSlider->setSliderPosition(volume);

    if (volume > 0)
        this->isVolumeOn = true;
    else
        this->isVolumeOn = false;

    this->setVolumeIcon(this->isVolumeOn);

}

void SoundSliderWindow::setVolumeIcon(bool isVolumeOn)
{
    if (isVolumeOn)
    {
        QPixmap pixmap("images\\sound.png");
        this->ui->label->setPixmap(pixmap);
        this->ui->label->setScaledContents(true);
        this->ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }
    else
    {
        QPixmap pixmap("images\\mute.png");
        this->ui->label->setPixmap(pixmap);
        this->ui->label->setScaledContents(true);
        this->ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }
}

SoundSliderWindow::~SoundSliderWindow()
{
    delete ui;
}

void SoundSliderWindow::on_volumeSlider_valueChanged(int value)
{
    emit sliderMoved(value);
    if (value == 0 && this->isVolumeOn)
    {
        this->setVolumeIcon(false);
        if (this->canChangeBtn)
            emit setVolumeBtnIcon(0);
        this->isVolumeOn = false;
    }
    else if (value != 0 && !this->isVolumeOn)
    {
        this->setVolumeIcon(true);
        if (this->canChangeBtn)
            emit setVolumeBtnIcon(100);
        this->isVolumeOn = true;
    }
    this->canChangeBtn = true;
}



