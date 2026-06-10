#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include "gamelogic.h"

/**
 * @brief Widget odpowiedzialny za renderowanie 2D planszy i kulki.
 *
 * Obsługuje rysowanie sceny gry oraz sterowanie klawiaturą.
 * Logika gry (fizyka, kolizje, labirynt) delegowana jest do klasy GameLogic.
 */
class GameWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor tworzący widget gry 2D.
     * @param logic - Wskaźnik do współdzielonej logiki gry
     * @param parent - Widget rodzica
     */
    explicit GameWidget(GameLogic *logic, QWidget *parent = nullptr);

    /**
     * @brief Zwraca wskaźnik do logiki gry.
     */
    GameLogic* getLogic() const { return logic; }

protected:
    /**
     * @brief Rysuje scenę gry — planszę, ściany, cel i kulkę.
     * @param event - Zdarzenie rysowania
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Obsługuje naciśnięcie klawisza strzałki.
     * @param event - Zdarzenie klawiatury
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Obsługuje zwolnienie klawisza strzałki.
     * @param event - Zdarzenie klawiatury
     */
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    /** @brief Rysuje podłogę planszy z gradientem. */
    void drawBoard(QPainter &painter, double offsetX, double offsetY, double boardSize);

    /** @brief Rysuje ściany labiryntu. */
    void drawWalls(QPainter &painter, double offsetX, double offsetY, double boardSize);

    /** @brief Rysuje pulsujący punkt docelowy. */
    void drawGoal(QPainter &painter, double offsetX, double offsetY, double boardSize);

    /** @brief Rysuje kulkę z cieniowaniem 3D i odbłyskiem. */
    void drawBall(QPainter &painter, double offsetX, double offsetY, double boardSize);

    /** @brief Rysuje półprzezroczysty overlay z komunikatem wygranej. */
    void drawWinOverlay(QPainter &painter, double boardSize);

    GameLogic *logic;   ///< Wskaźnik do logiki gry

    // Flagi klawiatury — lokalne dla tego widgetu
    bool goLeft = false;    ///< Flaga naciśnięcia klawisza w lewo
    bool goRight = false;   ///< Flaga naciśnięcia klawisza w prawo
    bool goUp = false;      ///< Flaga naciśnięcia klawisza w górę
    bool goDown = false;    ///< Flaga naciśnięcia klawisza w dół
};

#endif // GAMEWIDGET_H