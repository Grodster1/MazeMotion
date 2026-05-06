#include "gamewidget.h"
#include <cmath>

GameWidget::GameWidget(GameLogic *logic, QWidget *parent)
    : QWidget(parent), logic(logic)
{
    setFocusPolicy(Qt::StrongFocus);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, logic, &GameLogic::updatePos);
    connect(logic, &GameLogic::stateUpdated, this, QOverload<>::of(&QWidget::update));
    timer->start(16);
}

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    double boardSize = qMin(height(), width()) * 0.9;
    double offsetX = (width() - boardSize) / 2.0;
    double offsetY = (height() - boardSize) / 2.0;

    drawBoard(painter, offsetX, offsetY, boardSize);
    drawWalls(painter, offsetX, offsetY, boardSize);
    drawGoal(painter, offsetX, offsetY, boardSize);
    drawBall(painter, offsetX, offsetY, boardSize);

    if (logic->isGameWon()) drawWinOverlay(painter, boardSize);
}

void GameWidget::drawBoard(QPainter &painter, double offsetX, double offsetY, double boardSize) {
    QLinearGradient boardGradient(offsetX, offsetY, offsetX, offsetY + boardSize);
    boardGradient.setColorAt(0, QColor(220, 210, 180));
    boardGradient.setColorAt(1, QColor(160, 150, 125));
    painter.setBrush(boardGradient);
    painter.setPen(Qt::NoPen);
    painter.drawRect(offsetX, offsetY, boardSize, boardSize);
}

void GameWidget::drawWalls(QPainter &painter, double offsetX, double offsetY, double boardSize) {
    painter.setBrush(QColor(80, 50, 20));
    painter.setPen(Qt::NoPen);
    for (const QRectF &wall : logic->getWalls()) {
        QRectF screenWall(
            offsetX + wall.x() * boardSize,
            offsetY + wall.y() * boardSize,
            wall.width() * boardSize,
            wall.height() * boardSize
            );
        painter.drawRect(screenWall);
    }
}

void GameWidget::drawGoal(QPainter &painter, double offsetX, double offsetY, double boardSize) {
    double radius = logic->getBallRadius() * boardSize;
    double pulse = 0.7 + 0.3 * std::sin(logic->getPulsePhase());
    double goalRadius = radius * pulse;
    double glowRadius = radius * 1.5 * pulse;
    QPointF goalPos(offsetX + logic->getGoalX() * boardSize,
                    offsetY + logic->getGoalY() * boardSize);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 200, 0, 60));
    painter.drawEllipse(goalPos, glowRadius, glowRadius);
    painter.setBrush(QColor(0, 200, 0, 200));
    painter.drawEllipse(goalPos, goalRadius, goalRadius);
}

void GameWidget::drawBall(QPainter &painter, double offsetX, double offsetY, double boardSize) {
    double posX = offsetX + boardSize * logic->getBallX();
    double posY = offsetY + boardSize * logic->getBallY();
    double radius = logic->getBallRadius() * boardSize;

    painter.setPen(Qt::NoPen);

    // Cień
    painter.setBrush(QColor(0, 0, 0, 40));
    painter.drawEllipse(QPointF(posX + radius * 0.15, posY + radius * 0.15), radius, radius);

    // Gradient 3D
    QRadialGradient ballGradient(posX - radius * 0.3, posY - radius * 0.3, radius * 1.5);
    ballGradient.setColorAt(0, QColor(120, 120, 120));
    ballGradient.setColorAt(0.4, QColor(50, 50, 50));
    ballGradient.setColorAt(1, QColor(10, 10, 10));
    painter.setBrush(ballGradient);
    painter.drawEllipse(QPointF(posX, posY), radius, radius);

    // Odbłysk
    QRadialGradient highlight(posX - radius * 0.25, posY - radius * 0.35, radius * 0.4);
    highlight.setColorAt(0, QColor(255, 255, 255, 180));
    highlight.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(highlight);
    painter.drawEllipse(QPointF(posX - radius * 0.25, posY - radius * 0.35),
                        radius * 0.35, radius * 0.35);
}

void GameWidget::drawWinOverlay(QPainter &painter, double boardSize) {
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width(), height());

    painter.setPen(QColor(0, 220, 0));
    QFont font = painter.font();
    font.setPixelSize(boardSize * 0.08);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, "Wygrana!");
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) { event->ignore(); return; }

    double dx = 0, dy = 0;
    switch (event->key()) {
    case Qt::Key_Up:    goUp = true;    break;
    case Qt::Key_Down:  goDown = true;  break;
    case Qt::Key_Left:  goLeft = true;  break;
    case Qt::Key_Right: goRight = true; break;
    default: QWidget::keyPressEvent(event); return;
    }

    dx = (goRight ? 1.0 : 0.0) - (goLeft ? 1.0 : 0.0);
    dy = (goDown ? 1.0 : 0.0) - (goUp ? 1.0 : 0.0);
    logic->setKeyboardAcceleration(dx, dy);
    event->accept();
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) { event->ignore(); return; }

    switch (event->key()) {
    case Qt::Key_Up:    goUp = false;    break;
    case Qt::Key_Down:  goDown = false;  break;
    case Qt::Key_Left:  goLeft = false;  break;
    case Qt::Key_Right: goRight = false; break;
    default: QWidget::keyReleaseEvent(event); return;
    }

    double dx = (goRight ? 1.0 : 0.0) - (goLeft ? 1.0 : 0.0);
    double dy = (goDown ? 1.0 : 0.0) - (goUp ? 1.0 : 0.0);
    logic->setKeyboardAcceleration(dx, dy);
    event->accept();
}