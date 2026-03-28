#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>


class GameWidget : public QWidget
{
    Q_OBJECT
    double ballX = 0.5;
    double ballY = 0.5;

    double scaleFactor = 0.001;

    QTimer *timer;

    //Implementacja przyśpieszenia
    double velX = 0.0;
    double velY = 0.0;
    double accX = 0.0;
    double accY = 0.0;

    const double friction = 0.02;
    const double accelStep = 0.0005;


    bool goLeft = false;
    bool goRight = false;
    bool goUp = false;
    bool goDown = false;

public:
    explicit GameWidget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void updatePos();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    void onSensorData(float ax, float ay, float az, float gx, float gy, float gz);

signals:
};

#endif // GAMEWIDGET_H
