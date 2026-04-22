#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H
#include <QVector>
#include <QRectF>
#include <random>

struct Cell{
    bool topW = true;
    bool botW = true;
    bool leftW = true;
    bool rightW = true;
    bool visited = false;
};


class MazeGenerator
{
    QVector<QVector<Cell>> grid;
    int rows, cols;
    std::mt19937 rng;

public:
    MazeGenerator();
    void generate(int rows, int cols);
    void makePath(int row, int col);
    QVector<QRectF> getWalls();
};

#endif // MAZEGENERATOR_H
