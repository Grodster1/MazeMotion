#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamewidget.h"
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    //QLabel *gameLabel = new QLabel("Game");
    GameWidget *gameWidget = new GameWidget();
    QLabel *chartsLabel = new QLabel("Charts");
    chartsLabel->setAlignment(Qt::AlignCenter);

    splitter->addWidget(gameWidget);
    splitter->addWidget(chartsLabel);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->addWidget(splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}
