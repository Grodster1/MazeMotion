#include "gamewidget.h"


GameWidget::GameWidget(QWidget *parent) : QWidget{parent}{
    this->setFocusPolicy(Qt::StrongFocus); //potrzebne do obsługi klawiatury
    timer = new QTimer(this);
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

}

void GameWidget::updatePos(){
    if(goLeft)  ballX -= 0.005;
    if(goRight) ballX += 0.005;
    if(goUp)    ballY -= 0.005;
    if(goDown)  ballY += 0.005;

    ballX = qBound(0.0, ballX, 1.0);
    ballY = qBound(0.0, ballY, 1.0);

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
        event->accept();
        qDebug() << "Przycisk up wciśnięty";
        break;
    case Qt::Key_Down:
        goDown=true;
        event->accept();
        qDebug() << "Przycisk down wciśnięty";
        break;
    case Qt::Key_Left:
        goLeft=true;
        event->accept();
        qDebug() << "Przycisk left wciśnięty";
        break;
    case Qt::Key_Right:
        goRight=true;
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
        qDebug() << "Przycisk up odkliknięty";
        break;
    case Qt::Key_Down:
        goDown=false;
        qDebug() << "Przycisk down odkliknięty";
        break;
    case Qt::Key_Left:
        goLeft=false;
        qDebug() << "Przycisk left odkliknięty";
        break;
    case Qt::Key_Right:
        goRight=false;
        qDebug() << "Przycisk right odkliknięty";
        break;
    }
}
