#include "gamewidget3d.h"
#include <GL/glu.h>

GameWidget3D::GameWidget3D(GameLogic *logic, QWidget *parent)
    : QOpenGLWidget(parent), logic(logic)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(logic, &GameLogic::stateUpdated, this, QOverload<>::of(&QWidget::update));
}

void GameWidget3D::forceRefresh() {
    makeCurrent();
    resizeGL(width(), height());
    doneCurrent();
    update();
}

void GameWidget3D::drawWalls() {
    float wallHeight = 0.05f;
    glColor3f(0.31f, 0.20f, 0.08f);

    for (const QRectF &wall : logic->getWalls()) {
        float x = wall.x();
        float y = wall.y();
        float w = wall.width();
        float h = wall.height();

        glBegin(GL_QUADS);
        // Góra
        glVertex3f(x, y, wallHeight);
        glVertex3f(x+w, y, wallHeight);
        glVertex3f(x+w, y+h, wallHeight);
        glVertex3f(x, y+h, wallHeight);

        // Przód
        glVertex3f(x, y, 0);
        glVertex3f(x+w, y, 0);
        glVertex3f(x+w, y, wallHeight);
        glVertex3f(x, y, wallHeight);

        // Tył
        glVertex3f(x, y+h, 0);
        glVertex3f(x+w, y+h, 0);
        glVertex3f(x+w, y+h, wallHeight);
        glVertex3f(x, y+h, wallHeight);

        // Lewa
        glVertex3f(x, y, 0);
        glVertex3f(x, y+h, 0);
        glVertex3f(x, y+h, wallHeight);
        glVertex3f(x, y, wallHeight);

        // Prawa
        glVertex3f(x+w, y, 0);
        glVertex3f(x+w, y+h, 0);
        glVertex3f(x+w, y+h, wallHeight);
        glVertex3f(x+w, y, wallHeight);
        glEnd();
    }
}

void GameWidget3D::drawBall(){
    float bx = logic->getBallX();
    float by = logic->getBallY();
    float br = logic->getBallRadius();

    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(bx, by, br);
    GLUquadric *quad = gluNewQuadric();
    gluSphere(quad, br, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void GameWidget3D::drawGoal() {
    float pulse = 0.7f + 0.3f * std::sin(logic->getPulsePhase());
    float gr = logic->getBallRadius() * pulse;

    glColor3f(0.0f, 0.8f, 0.0f);
    glPushMatrix();
    glTranslatef(logic->getGoalX(), logic->getGoalY(), gr);
    GLUquadric *quad = gluNewQuadric();
    gluSphere(quad, gr, 16, 16);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void GameWidget3D::drawBoard(){
    glColor3f(0.76f, 0.74f, 0.65f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
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
        0.5, 0.3, 1.8,
        0.5, 0.5, 0.0,
        0.0, 0.0, 1.0
        );

    glScalef(1.0f, -1.0f, 1.0f);
    glTranslatef(0.0f, -1.0f, 0.0f);

    drawBoard();
    drawWalls();
    drawGoal();
    drawBall();
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