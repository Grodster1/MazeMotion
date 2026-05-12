#include "gamewidget3d.h"
#include <GL/glu.h>

GameWidget3D::GameWidget3D(GameLogic *logic, QWidget *parent)
    : QOpenGLWidget(parent), logic(logic)
{
    setFocusPolicy(Qt::StrongFocus);

    // QTimer *timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, logic, &GameLogic::updatePos);
    // connect(logic, &GameLogic::stateUpdated, this, QOverload<>::of(&QWidget::update));
    // timer->start(16);
    connect(logic, &GameLogic::stateUpdated, this, QOverload<>::of(&QWidget::update));
}

void GameWidget3D::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void GameWidget3D::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void GameWidget3D::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        0.5, -0.3, 1.2,   // pozycja kamery
        0.5,  0.5, 0.0,   // punkt na który patrzy
        0.0,  0.0, 1.0    // wektor "do góry"
        );


    glColor3f(0.76f, 0.74f, 0.65f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}

void GameWidget3D::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) { event->ignore(); return; }

    double dx = 0, dy = 0;
    switch (event->key()) {
    case Qt::Key_Up:    goUp = true;    break;
    case Qt::Key_Down:  goDown = true;  break;
    case Qt::Key_Left:  goLeft = true;  break;
    case Qt::Key_Right: goRight = true; break;
    default: QWidget::keyPressEvent(event); return;
    }

    dx = (goRight ? 1.0 : 0.0) - (goLeft ? 1.0 : 0.0);
    dy = (goDown ? 1.0 : 0.0) - (goUp ? 1.0 : 0.0);
    logic->setKeyboardAcceleration(dx, dy);
    event->accept();
}

void GameWidget3D::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) { event->ignore(); return; }

    switch (event->key()) {
    case Qt::Key_Up:    goUp = false;    break;
    case Qt::Key_Down:  goDown = false;  break;
    case Qt::Key_Left:  goLeft = false;  break;
    case Qt::Key_Right: goRight = false; break;
    default: QWidget::keyReleaseEvent(event); return;
    }

    double dx = (goRight ? 1.0 : 0.0) - (goLeft ? 1.0 : 0.0);
    double dy = (goDown ? 1.0 : 0.0) - (goUp ? 1.0 : 0.0);
    logic->setKeyboardAcceleration(dx, dy);
    event->accept();
}