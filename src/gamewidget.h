#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include "mazegenerator.h"

/**
 * @brief Widget odpowiedzialny za renderowanie planszy i kulki.
 *
 * Obsługuje rysowanie sceny gry, fizykę ruchu kulki
 * oraz sterowanie za pomocą klawiatury i danych pochodzących z IMU.
 * Plansza jest skalowalna i zawsze rysowana jako wycentrowany kwadrat.
 */
class GameWidget : public QWidget
{
    Q_OBJECT

    double ballX;               ///< Znormalizowana pozycja kulki w osi X (0.0 -- 1.0)
    double ballY;               ///< Znormalizowana pozycja kulki w osi Y (0.0 -- 1.0)
    double ballRadius = 0.02;   ///< Znormalizowany promień kulki do detekcji kolizji
    double scaleFactor = 0.001; ///< Współczynnik skalujący dane z akcelerometru na przyspieszenie kulki
    double pulsePhase = 0.0;    ///< Faza animacji pulsowania punktu docelowego [rad]

    QTimer *timer; ///< Timer odpalający aktualizację pozycji co 16 ms (~60 FPS)

    double velX = 0.0;  ///< Składowa prędkości kulki w osi X
    double velY = 0.0;  ///< Składowa prędkości kulki w osi Y
    double accX = 0.0;  ///< Składowa przyspieszenia kulki w osi X
    double accY = 0.0;  ///< Składowa przyspieszenia kulki w osi Y

    const double friction = 0.02;     ///< Współczynnik tarcia (0.0 -- 1.0)
    const double accelStep = 0.0005;  ///< Wartość przyspieszenia nadawanego przez klawiaturę

    MazeGenerator *generator;   ///< Generator labiryntu
    QVector<QRectF> walls;      ///< Lista prostokątów ścian w znormalizowanych współrzędnych

    int rows;  ///< Liczba wierszy labiryntu
    int cols;  ///< Liczba kolumn labiryntu

    double goalX;  ///< Znormalizowana pozycja X środka komórki docelowej
    double goalY;  ///< Znormalizowana pozycja Y środka komórki docelowej

    bool goLeft = false;   ///< Flaga naciśnięcia klawisza w lewo
    bool goRight = false;  ///< Flaga naciśnięcia klawisza w prawo
    bool goUp = false;     ///< Flaga naciśnięcia klawisza w górę
    bool goDown = false;   ///< Flaga naciśnięcia klawisza w dół

    bool gameWon = false; ///< Flaga wygranej gry

public:
    /**
     * @brief Konstruktor tworzący widget gry z labiryntem.
     *
     * Generuje labirynt o zadanym rozmiarze, wyznacza punkt docelowy
     * i uruchamia timer aktualizacji pozycji.
     *
     * @param r Liczba wierszy labiryntu
     * @param c Liczba kolumn labiryntu
     * @param parent Widget rodzica (domyślnie nullptr)
     */
    explicit GameWidget(int r, int c, QWidget *parent = nullptr);

    /**
     * @brief Rysuje scenę gry — planszę, ściany labiryntu, punkt docelowy i kulkę.
     *
     * Wywoływana automatycznie przez Qt przy każdym odświeżeniu widgetu.
     * Plansza rysowana jest jako kwadrat wycentrowany w widgecie,
     * a wszystkie elementy skalują się proporcjonalnie do rozmiaru okna.
     *
     * @param event Zdarzenie rysowania (nieużywane bezpośrednio)
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Resetuje stan gry — generuje nowy labirynt i przywraca kulkę do pozycji startowej.
     *
     * Wywoływana po dotarciu kulki do celu lub na żądanie użytkownika
     * (przycisk "Nowy labirynt"). Generuje nowy układ ścian, wyznacza
     * nowy punkt docelowy i zeruje prędkość oraz przyspieszenie kulki.
     */
    void resetMaze();


    /**
     * @brief Aktualizuje pozycję kulki na planszy.
     *
     * Wywoływana cyklicznie przez QTimer co 16 ms. Aktualizuje prędkość
     * kulki na podstawie przyspieszenia (z akcelerometru lub klawiatury),
     * stosuje tarcie, wykrywa kolizje ze ścianami labiryntu
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
    /**
     * @brief Slot odbierający dane z czujnika IMU.
     *
     * Mapuje odczyty z akcelerometru na przyspieszenie kulki
     * z uwzględnieniem współczynnika skalowania.
     *
     * @param ax Przyspieszenie liniowe w osi X [g]
     * @param ay Przyspieszenie liniowe w osi Y [g]
     * @param az Przyspieszenie liniowe w osi Z [g]
     * @param gx Prędkość kątowa w osi X [°/s]
     * @param gy Prędkość kątowa w osi Y [°/s]
     * @param gz Prędkość kątowa w osi Z [°/s]
     */
    void onSensorData(float ax, float ay, float az, float gx, float gy, float gz);

signals:
};

#endif // GAMEWIDGET_H