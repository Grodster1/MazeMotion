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

void GameWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    double boardSize = qMin(height(), width()) * 0.9;
    double offsetX = (width() - boardSize) / 2.0;
    double offsetY = (height() - boardSize) / 2.0;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor::fromRgb(196, 195, 169));
    painter.drawRect(offsetX, offsetY, boardSize, boardSize);

    double radius = ballRadius * boardSize;
    double posX = offsetX + boardSize*ballX;
    double posY = offsetY + boardSize*ballY;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor::fromRgb(80, 50, 20));
    painter.setRenderHint(QPainter::Antialiasing);
    for(const QRectF &wall : walls) {
        QRectF screenWall(
            offsetX + wall.x() * boardSize,
            offsetY + wall.y() * boardSize,
            wall.width() * boardSize,
            wall.height() * boardSize
            );
        painter.drawRect(screenWall);
    }

    double pulse = 0.7 + 0.3 * std::sin(pulsePhase);  // skala 0.7 - 1.0
    double goalRadius = radius * pulse;
    double glowRadius = radius * 1.5 * pulse;

    QColor glowColor(0, 200, 0, 60);
    painter.setBrush(glowColor);
    painter.drawEllipse(
        QPointF(offsetX + goalX * boardSize, offsetY + goalY * boardSize),
        glowRadius, glowRadius
        );

    QColor goalColor(0, 200, 0, 200);
    painter.setBrush(goalColor);
    painter.drawEllipse(
        QPointF(offsetX + goalX * boardSize, offsetY + goalY * boardSize),
        goalRadius, goalRadius
        );

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor::fromRgb(0,0,0));
    painter.drawEllipse(QPointF(posX, posY), radius, radius);

}

void GameWidget::updatePos(){
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
        qDebug() << "Przycisk up wciśnięty";
        break;
    case Qt::Key_Down:
        goDown=true;
        accY += accelStep;
        event->accept();
        qDebug() << "Przycisk down wciśnięty";
        break;
    case Qt::Key_Left:
        goLeft=true;
        accX -= accelStep;
        event->accept();
        qDebug() << "Przycisk left wciśnięty";
        break;
    case Qt::Key_Right:
        goRight=true;
        accX += accelStep;
        event->accept();
        qDebug() << "Przycisk right wciśnięty";
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
        qDebug() << "Przycisk up odkliknięty";
        break;
    case Qt::Key_Down:
        goDown=false;
        accY = 0.0;
        qDebug() << "Przycisk down odkliknięty";
        break;
    case Qt::Key_Left:
        goLeft=false;
        accX = 0.0;
        qDebug() << "Przycisk left odkliknięty";
        break;
    case Qt::Key_Right:
        goRight=false;
        accX = 0.0;
        qDebug() << "Przycisk right odkliknięty";
        break;
    }
}

void GameWidget::onSensorData(float ax, float ay, float az, float gx, float gy, float gz){
    qDebug() << "ax:" << ax << "ay:" << ay << "az:" << az;
    accX = ax * scaleFactor;
    accY = -ay * scaleFactor;

}
