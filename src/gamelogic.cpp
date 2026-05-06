#include "gamelogic.h"
#include <cmath>
#include <QDebug>

GameLogic::GameLogic(int rows, int cols, QObject *parent)
    : QObject(parent), rows(rows), cols(cols)
{
    generator = new MazeGenerator();
    resetMaze();
}

void GameLogic::resetMaze() {
    generator->generate(rows, cols);
    walls = generator->getWalls();

    auto [goalRow, goalCol] = generator->findFarthestCell();
    goalX = (goalCol + 0.5) / cols;
    goalY = (goalRow + 0.5) / rows;

    ballX = 1.0 / (2.0 * cols);
    ballY = 1.0 / (2.0 * rows);
    velX = velY = accX = accY = 0.0;
    pulsePhase = 0.0;
    gameWon = false;
}

void GameLogic::setSensorAcceleration(float ax, float ay) {
    accX = ax * scaleFactor;
    accY = -ay * scaleFactor;
}

void GameLogic::setKeyboardAcceleration(double dx, double dy) {
    accX = dx * accelStep;
    accY = dy * accelStep;
}

void GameLogic::updatePos() {
    if (gameWon) {
        pulsePhase += 0.05;
        if (pulsePhase > 2 * M_PI) pulsePhase -= 2 * M_PI;
        emit stateUpdated();
        return;
    }

    velX += accX;
    velY += accY;
    velX *= (1.0 - friction);
    velY *= (1.0 - friction);
    ballX += velX;
    ballY += velY;

    if (ballX < 0.0) { ballX = 0.0; velX = 0.0; }
    if (ballX > 1.0) { ballX = 1.0; velX = 0.0; }
    if (ballY < 0.0) { ballY = 0.0; velY = 0.0; }
    if (ballY > 1.0) { ballY = 1.0; velY = 0.0; }

    for (int i = 0; i < 3; ++i) {
        for (const QRectF &wall : walls) {
            double closestX = qBound(wall.left(), ballX, wall.right());
            double closestY = qBound(wall.top(), ballY, wall.bottom());

            double distX = ballX - closestX;
            double distY = ballY - closestY;
            double dist = std::sqrt(distX * distX + distY * distY);

            if (dist <= ballRadius && dist > 0) {
                double nx = distX / dist;
                double ny = distY / dist;

                ballX = closestX + nx * ballRadius;
                ballY = closestY + ny * ballRadius;

                double dotProduct = velX * nx + velY * ny;
                if (dotProduct < 0) {
                    velX -= dotProduct * nx;
                    velY -= dotProduct * ny;
                }
            }
        }
    }

    pulsePhase += 0.05;
    if (pulsePhase > 2 * M_PI) pulsePhase -= 2 * M_PI;

    double dx = ballX - goalX;
    double dy = ballY - goalY;
    if (std::sqrt(dx * dx + dy * dy) < ballRadius) {
        gameWon = true;
        velX = velY = accX = accY = 0.0;
        emit gameWonSignal();
    }

    emit stateUpdated();
}