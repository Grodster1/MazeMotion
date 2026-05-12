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
     * @param logic Wskaźnik do współdzielonej logiki gry
     * @param parent Widget rodzica
     */
    explicit GameWidget(GameLogic *logic, QWidget *parent = nullptr);

    /**
     * @brief Zwraca wskaźnik do logiki gry.
     */
    GameLogic* getLogic() const { return logic; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void drawBoard(QPainter &painter, double offsetX, double offsetY, double boardSize);
    void drawWalls(QPainter &painter, double offsetX, double offsetY, double boardSize);
    void drawGoal(QPainter &painter, double offsetX, double offsetY, double boardSize);
    void drawBall(QPainter &painter, double offsetX, double offsetY, double boardSize);
    void drawWinOverlay(QPainter &painter, double boardSize);

    GameLogic *logic;   ///< Wskaźnik do logiki gry

    // Flagi klawiatury — lokalne dla tego widgetu
    bool goLeft = false;
    bool goRight = false;
    bool goUp = false;
    bool goDown = false;
};

#endif // GAMEWIDGET_H