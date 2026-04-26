#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include "mazegenerator.h"



/**
 * @brief Widget odpowiedzialny za renderowanie planszy i kuli.
 * 
 * Obsługuję rysowanie sceny gdy, fizykę ruchu kulki
 * oraz sterowanie za pomocą klawiatury i danych pochodzących z IMU
 */
class GameWidget : public QWidget
{
    Q_OBJECT
    double ballX; ///< Znormalizowana pozycja kulki w osi X
    double ballY; ///< Znormalizowana pozycja kulki w osi Y

    double scaleFactor = 0.001; ///< Współczynnik skalujący dane pochodzące z akcelerometru

    QTimer *timer;

    //Implementacja przyśpieszenia
    double velX = 0.0; ///< Wartość składowej prędkości X
    double velY = 0.0; ///< Wartość składowej prędkośći Y
    double accX = 0.0; ///< Wartość składowej przyśpieszenia X
    double accY = 0.0; ///< Wartość składowej przyśpieszenia Y

    const double friction = 0.02; ///< Wartość tarcia
    const double accelStep = 0.0005;
    MazeGenerator *generator;

    QVector<QRectF> walls;

    int rows;
    int cols;

    bool goLeft = false;
    bool goRight = false;
    bool goUp = false;
    bool goDown = false;

public:
    explicit GameWidget(int r, int c, QWidget *parent = nullptr);
    /**
     * @brief Rysuje scenę gry — planszę, ściany labiryntu i kulkę.
     *
     * Wywoływana automatycznie przez Qt przy każdym odświeżeniu widgetu.
     * Plansza rysowana jest jako kwadrat wycentrowany w widgecie,
     * a wszystkie elementy skalują się proporcjonalnie do rozmiaru okna.
     *
     * @param event Zdarzenie rysowania (nieużywane bezpośrednio)
     *
     */

    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Aktualizuje pozycję kulki na planszy.
     *
     * Wywoływana cyklicznie przez QTimer co 16 ms. Aktualizuje prędkość
     * kulki na podstawie przyspieszenia (z akcelerometru lub klawiatury),
     * stosuje tarcie, ogranicza pozycję do granic planszy
     * i wywołuje update() w celu przerysowania widgetu.
     */
    void updatePos();

protected:
    /**
     * @brief Obsługuje naciśnięcie klawisza strzałki.
     *
     * Ustawia przyspieszenie kulki w odpowiednim kierunku.
     * Ignoruje zdarzenia auto-repeat systemu operacyjnego.
     *
     * @param event Zdarzenie klawiatury
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Obsługuje zwolnienie klawisza strzałki.
     *
     * Zeruje przyspieszenie w danym kierunku.
     * Ignoruje zdarzenia auto-repeat systemu operacyjnego.
     *
     * @param event Zdarzenie klawiatury
     */
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    void onSensorData(float ax, float ay, float az, float gx, float gy, float gz);

signals:
};

#endif // GAMEWIDGET_H
