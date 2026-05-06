#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include <QVector>
#include <QRectF>
#include "mazegenerator.h"

/**
 * @brief Klasa zawierająca logikę gry — fizykę kulki, kolizje i zarządzanie labiryntem.
 *
 * Oddzielona od warstwy renderowania, dzięki czemu może być współdzielona
 * przez widgety 2D (GameWidget) i 3D (GameWidget3D).
 */
class GameLogic : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor inicjalizujący logikę gry.
     * @param rows Liczba wierszy labiryntu
     * @param cols Liczba kolumn labiryntu
     * @param parent Obiekt rodzica
     */
    explicit GameLogic(int rows, int cols, QObject *parent = nullptr);

    /**
     * @brief Aktualizuje pozycję kulki — fizyka, tarcie, kolizje.
     *
     * Wywoływana cyklicznie przez timer widgetu.
     */
    void updatePos();

    /**
     * @brief Resetuje stan gry — nowy labirynt, kulka na starcie.
     */
    void resetMaze();

    /**
     * @brief Ustawia przyspieszenie kulki na podstawie danych z akcelerometru.
     * @param ax Przyspieszenie liniowe w osi X [g]
     * @param ay Przyspieszenie liniowe w osi Y [g]
     */
    void setSensorAcceleration(float ax, float ay);

    /**
     * @brief Ustawia przyspieszenie kulki na podstawie klawiatury.
     * @param dx Kierunek w osi X (-1, 0, 1)
     * @param dy Kierunek w osi Y (-1, 0, 1)
     */
    void setKeyboardAcceleration(double dx, double dy);

    // Gettery — widgety odczytują stan do rysowania
    double getBallX() const { return ballX; }
    double getBallY() const { return ballY; }
    double getBallRadius() const { return ballRadius; }
    double getGoalX() const { return goalX; }
    double getGoalY() const { return goalY; }
    double getPulsePhase() const { return pulsePhase; }
    bool isGameWon() const { return gameWon; }
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    const QVector<QRectF>& getWalls() const { return walls; }

signals:
    /**
     * @brief Emitowany gdy kulka dotrze do celu.
     */
    void gameWonSignal();

    /**
     * @brief Emitowany po każdej aktualizacji stanu — widgety powinny się przerysować.
     */
    void stateUpdated();

private:
    MazeGenerator *generator;   ///< Generator labiryntu
    QVector<QRectF> walls;      ///< Lista prostokątów ścian

    int rows;   ///< Liczba wierszy labiryntu
    int cols;   ///< Liczba kolumn labiryntu

    double ballX;               ///< Znormalizowana pozycja kulki w osi X
    double ballY;               ///< Znormalizowana pozycja kulki w osi Y
    double ballRadius = 0.02;   ///< Znormalizowany promień kulki
    double scaleFactor = 0.001; ///< Współczynnik skalowania danych z akcelerometru

    double velX = 0.0;  ///< Składowa prędkości X
    double velY = 0.0;  ///< Składowa prędkości Y
    double accX = 0.0;  ///< Składowa przyspieszenia X
    double accY = 0.0;  ///< Składowa przyspieszenia Y

    const double friction = 0.02;     ///< Współczynnik tarcia
    const double accelStep = 0.0005;  ///< Przyspieszenie z klawiatury

    double goalX;   ///< Pozycja X celu
    double goalY;   ///< Pozycja Y celu

    double pulsePhase = 0.0;  ///< Faza animacji celu
    bool gameWon = false;     ///< Flaga wygranej
};

#endif // GAMELOGIC_H