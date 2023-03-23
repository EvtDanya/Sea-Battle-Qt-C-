#pragma once

#include <QWidget>
#include <QMainWindow>
#include <SFML/Graphics.hpp>
#include <QTimer>

namespace Ui
{
 class SFMLWidget;
}

class SFMLWidget : public QWidget, public sf::RenderWindow
{
    Q_OBJECT
public:
    SFMLWidget(QWidget *parent, const QPoint &position = QPoint(0,0), const QSize &size = QSize(0,0), unsigned int FrameTime = 0);
public:
    void showEvent(QShowEvent*) override;
    QPaintEngine* paintEngine() const override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

    void deleteRect();
protected:
    virtual void update();
    virtual void initialize();

    sf::Texture textureCell;
    sf::Sprite spriteCell;

    std::vector<sf::RectangleShape*> rectangles;

    QTimer timer;

    int border_thickness = 1;

    sf::Color color;
};

