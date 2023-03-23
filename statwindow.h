#pragma once

#include <QWidget>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <vector>
#include <QTimer>
#include <QCloseEvent>

#include "statistics.h"
namespace Ui
{
    class StatWindow;
    struct Rank;
}


struct Rank
{
    uint8_t winsToGet;
    std::string name;
};

class StatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StatWindow(QWidget *parent = nullptr);
    ~StatWindow();

    virtual void showEvent(QShowEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

    Statistics *stat;
public slots:
   void saveStatToFile();
   void checkRankDelay();

signals:
    void helloWindow();
    void sendRankCheckedInfo(RankChecked rankChecked);

    void saveConfigToFile();

private slots:
    void on_OkBtn_clicked();

    void on_DeleteBtn_clicked();

    void receiveStat(Statistics* stat);
    void checkRank();
private:
    Ui::StatWindow *ui;
    std::vector<Rank> ranks;
    QTimer *timer;
};

