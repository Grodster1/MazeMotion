#ifndef GAMEWIDGET3D_H
#define GAMEWIDGET3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QTimer>
#include "gamelogic.h"

class GameWidget3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GameWidget3D(GameLogic *logic, QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:

    GameLogic *logic;   ///< Wskaźnik do logiki gry

    void drawWalls();
    void drawBall();
    void drawGoal();
    void drawBoard();

    bool goRight = false;
    bool goLeft = false;
    bool goUp = false;
    bool goDown = false;
};

#endif
