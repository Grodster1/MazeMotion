#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>



/**
 * @brief Widget odpowiedzialny za renderowanie planszy i kuli.
 * 
 * Obsługuję rysowanie sceny gdy, fizykę ruchu kulki
 * oraz sterowanie za pomocą klawiatury i danych pochodzących z IMU
 */
class GameWidget : public QWidget
{
    Q_OBJECT
    double ballX = 0.5; ///< Znormalizowana pozycja kulki w osi X 
    double ballY = 0.5; ///< Znormalizowana pozycja kulki w osi Y

    double scaleFactor = 0.001; ///< Współczynnik skalujący dane pochodzące z akcelerometru

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
