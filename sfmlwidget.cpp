#include "sfmlwidget.h"

SFMLWidget::SFMLWidget(QWidget *parent, const QPoint &position, const QSize &size, unsigned int FrameTime)
    : QWidget(parent)
{
    // Make the settings to directly draw the image into the widget
      setAttribute(Qt::WA_PaintOnScreen);
      setAttribute(Qt::WA_OpaquePaintEvent);
      setAttribute(Qt::WA_NoSystemBackground);

      // Set strong focus to keyboard events
      setFocusPolicy(Qt::StrongFocus);
    move(position);
    resize(size);

    timer.setInterval(FrameTime);
}

void SFMLWidget::showEvent(QShowEvent*) {
    sf::RenderWindow::create(reinterpret_cast<sf::WindowHandle>(winId()));
    initialize();
    connect(&timer, SIGNAL(timeout()), this, SLOT(repaint()));
    timer.start();
}

QPaintEngine* SFMLWidget::paintEngine() const {
    return 0;
}

void SFMLWidget::paintEvent(QPaintEvent*) {
    sf::RenderTarget::clear(this->color);
    update();
    sf::RenderWindow::display();
}

void SFMLWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    sf::RenderWindow::setPosition(sf::Vector2i(QWidget::pos().x(), QWidget::pos().y()));
}

void SFMLWidget::update() {
    for (auto i = 0; i < 10; i++)
    {
        for (auto j = 0; j < 10; j++)
        {
            spriteCell.setPosition(i*40+1,j*40+1);
            RenderWindow::draw(spriteCell);
        }
    }
    for (auto &elem : rectangles) {
        RenderWindow::draw(*elem);
    }
}

void SFMLWidget::initialize() {
    textureCell.loadFromFile("images\\frame.png");
    spriteCell.setTexture(textureCell);
}

void SFMLWidget::mousePressEvent(QMouseEvent*) {
    QWidget::update();
}

void SFMLWidget::deleteRect()
{
    for (auto rectangle : this->rectangles)
    {
         delete rectangle;
    }
    this->rectangles.clear();
}
