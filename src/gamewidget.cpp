#include "gamewidget.h"


GameWidget::GameWidget(int r, int c, QWidget *parent) : rows(r), cols(c), QWidget{parent}{
    this->setFocusPolicy(Qt::StrongFocus); //potrzebne do obsługi klawiatury
    timer = new QTimer(this);
    generator = new MazeGenerator();
    generator->generate(rows, cols);
    walls = generator->getWalls();

    connect(timer, &QTimer::timeout, this, &GameWidget::updatePos);
    timer->start(16);
}

void GameWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    double boardSize = qMin(height(), width()) * 0.9;
    double offsetX = (width() - boardSize) / 2.0;
    double offsetY = (height() - boardSize) / 2.0;

    painter.setBrush(QColor::fromRgb(196, 195, 169));
    painter.drawRect(offsetX, offsetY, boardSize, boardSize);

    double radius = qMin(height(), width()) * 0.02;
    double posX = offsetX + boardSize*ballX;
    double posY = offsetY + boardSize*ballY;

    painter.setBrush(QColor::fromRgb(0,0,0));
    painter.drawEllipse(QPointF(posX, posY), radius, radius);

    painter.setBrush(QColor::fromRgb(80, 50, 20));
    for(const QRectF &wall : walls) {
        QRectF screenWall(
            offsetX + wall.x() * boardSize,
            offsetY + wall.y() * boardSize,
            wall.width() * boardSize,
            wall.height() * boardSize
            );
        painter.drawRect(screenWall);
    }

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


    // ballX = qBound(0.0, ballX, 1.0);
    // ballY = qBound(0.0, ballY, 1.0);

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
