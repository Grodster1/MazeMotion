#include "mazegenerator.h"


MazeGenerator::MazeGenerator() : rng(std::random_device{}()) {}

void MazeGenerator::generate(int rows, int cols){
    this->rows = rows;
    this->cols = cols;

    grid = QVector<QVector<Cell>>(rows, QVector<Cell>(cols));
    makePath(0, 0);

}

void MazeGenerator::makePath(int row, int col){
    grid[row][col].visited = true;
    std::vector<std::pair<int,int>> dirs = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    std::shuffle(dirs.begin(), dirs.end(), rng);

    for(auto [dr, dc] : dirs) {
        int newRow = row + dr;
        int newCol = col + dc;

        if(newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols && !grid[newRow][newCol].visited) {
            if(dr == -1 && dc == 0){
                grid[row][col].topW = false;
                grid[newRow][newCol].botW = false;
            }
            else if(dr == 1 && dc == 0){
                grid[row][col].botW = false;
                grid[newRow][newCol].topW = false;
            }
            else if(dr == 0 && dc == 1){
                grid[row][col].rightW = false;
                grid[newRow][newCol].leftW = false;
            }
            else if(dr == 0 && dc == -1){
                grid[row][col].leftW = false;
                grid[newRow][newCol].rightW = false;
            }
            makePath(newRow, newCol);
        }
    }
}

QVector<QRectF> MazeGenerator::getWalls(){
    QVector<QRectF> walls;
    double cellW = 1.0 / cols;
    double cellH = 1.0 / cols;
    double thick = 0.005;

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            double x = j * cellW;
            double y = i * cellH;

            if(grid[i][j].topW){
                walls.append(QRectF(x, y-thick/2, cellW, thick));
            }
            if(grid[i][j].leftW){
                walls.append(QRectF(x - thick/2, y, thick, cellH));
            }
        }
    }

    for(int i = 0; i < rows; ++i){
        if(grid[i][cols-1].rightW){
            walls.append(QRectF(1.0 - thick/2, i * cellH, thick, cellH));
        }
    }

    for(int j = 0; j < cols; ++j){
        if(grid[rows-1][j].botW){
            walls.append(QRectF(j * cellW, 1.0 - thick/2, cellW, thick));
        }
    }

    return walls;
}