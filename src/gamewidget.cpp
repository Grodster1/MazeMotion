#include "gamewidget.h"
#include <cmath>

GameWidget::GameWidget(int r, int c, QWidget *parent) : rows(r), cols(c), QWidget{parent}{
    this->setFocusPolicy(Qt::StrongFocus); //potrzebne do obsługi klawiatury
    timer = new QTimer(this);
    generator = new MazeGenerator();
    generator->generate(rows, cols);
    walls = generator->getWalls();
    ballX = 1.0 / (2.0 * cols);
    ballY = 1.0 / (2.0 * rows);

    auto [goalRow, goalCol] = generator->findFarthestCell();
    goalX = (goalCol + 0.5) / cols;
    goalY = (goalRow + 0.5) / rows;

    connect(timer, &QTimer::timeout, this, &GameWidget::updatePos);
    timer->start(16);
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
    for (const QRectF &wall : walls) {
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
    double radius = ballRadius * boardSize;
    double pulse = 0.7 + 0.3 * std::sin(pulsePhase);
    double goalRadius = radius * pulse;
    double glowRadius = radius * 1.5 * pulse;
    QPointF goalPos(offsetX + goalX * boardSize, offsetY + goalY * boardSize);

    painter.setBrush(QColor(0, 200, 0, 60));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(goalPos, glowRadius, glowRadius);

    painter.setBrush(QColor(0, 200, 0, 200));
    painter.drawEllipse(goalPos, goalRadius, goalRadius);
}

void GameWidget::drawBall(QPainter &painter, double offsetX, double offsetY, double boardSize) {
    double posX = offsetX + boardSize * ballX;
    double posY = offsetY + boardSize * ballY;
    double radius = ballRadius * boardSize;

    painter.setPen(Qt::NoPen);

    painter.setBrush(QColor(0, 0, 0, 40));
    painter.drawEllipse(QPointF(posX + radius * 0.15, posY + radius * 0.15), radius, radius);

    QRadialGradient ballGradient(posX - radius * 0.3, posY - radius * 0.3, radius * 1.5);
    ballGradient.setColorAt(0, QColor(120, 120, 120));
    ballGradient.setColorAt(0.4, QColor(50, 50, 50));
    ballGradient.setColorAt(1, QColor(10, 10, 10));
    painter.setBrush(ballGradient);
    painter.drawEllipse(QPointF(posX, posY), radius, radius);

    QRadialGradient highlight(posX - radius * 0.25, posY - radius * 0.35, radius * 0.4);
    highlight.setColorAt(0, QColor(255, 255, 255, 180));
    highlight.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(highlight);
    painter.drawEllipse(QPointF(posX - radius * 0.25, posY - radius * 0.35), radius * 0.35, radius * 0.35);
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

    if (gameWon) drawWinOverlay(painter, boardSize);
}
void GameWidget::resetMaze() {
    gameWon = false;
    generator->generate(rows, cols);
    walls = generator->getWalls();
    auto [goalRow, goalCol] = generator->findFarthestCell();
    goalX = (goalCol + 0.5) / cols;
    goalY = (goalRow + 0.5) / rows;
    ballX = 1.0 / (2.0 * cols);
    ballY = 1.0 / (2.0 * rows);
    velX = velY = accX = accY = 0.0;
    update();
}

void GameWidget::updatePos(){
    if(gameWon) { update(); return; }

    velX += accX;
    velY += accY;
    velX *= (1.0 - friction);
    velY *= (1.0 - friction);
    ballX += velX;
    ballY += velY;

    if(ballX < 0.0){ballX = 0.0; velX = 0.0;}
    if(ballX > 1.0){ballX = 1.0; velX = 0.0;}
    if(ballY < 0.0){ballY = 0.0; velY = 0.0;}
    if(ballY > 1.0){ballY = 1.0; velY = 0.0;}

    for(int i = 0; i < 3; ++i){

        for(const QRectF &wall : walls){
            double closestX = qBound(wall.left(), ballX, wall.right());
            double closestY = qBound(wall.top(), ballY, wall.bottom());

            double distX = ballX - closestX;
            double distY = ballY - closestY;
            double dist = std::sqrt(distX * distX + distY * distY);

            if(dist <= ballRadius && dist > 0){
                double nx = distX / dist;
                double ny = distY / dist;

                ballX = closestX + nx * ballRadius;
                ballY = closestY + ny * ballRadius;

                double dotProduct = velX * nx + velY * ny;
                if(dotProduct < 0) {
                    velX -= dotProduct * nx;
                    velY -= dotProduct * ny;
                }
            }

            double dx = ballX - goalX;
            double dy = ballY - goalY;
            if(std::sqrt(dx*dx + dy*dy) < ballRadius) {
                gameWon = true;
                velX = velY = accX = accY = 0.0;
            }
        }
    }

    pulsePhase += 0.05;
    if(pulsePhase > 2 * M_PI) pulsePhase -= 2 * M_PI;


    update();
}

void GameWidget::keyPressEvent(QKeyEvent *event){

    if(event->isAutoRepeat()){
        event->ignore();
        return;
    }

    switch(event->key()){
    case Qt::Key_Up:
        goUp=true;
        accY -= accelStep;
        event->accept();
        break;
    case Qt::Key_Down:
        goDown=true;
        accY += accelStep;
        event->accept();
        break;
    case Qt::Key_Left:
        goLeft=true;
        accX -= accelStep;
        event->accept();
        break;
    case Qt::Key_Right:
        goRight=true;
        accX += accelStep;
        event->accept();
        break;
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat()){
        event->ignore();
        return;
    }

    switch(event->key()){
    case Qt::Key_Up:
        goUp=false;
        accY = 0.0;
        break;
    case Qt::Key_Down:
        goDown=false;
        accY = 0.0;
        break;
    case Qt::Key_Left:
        goLeft=false;
        accX = 0.0;
        break;
    case Qt::Key_Right:
        goRight=false;
        accX = 0.0;
        break;
    }
}

void GameWidget::onSensorData(float ax, float ay, float az, float gx, float gy, float gz){
    qDebug() << "ax:" << ax << "ay:" << ay << "az:" << az;
    accX = ax * scaleFactor;
    accY = -ay * scaleFactor;

}
