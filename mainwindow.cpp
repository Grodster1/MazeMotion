#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    gameWidget = new GameWidget(this);
    sensorReader = new SensorReader(this);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    QLabel *chartsLabel = new QLabel("Charts");
    chartsLabel->setAlignment(Qt::AlignCenter);

    splitter->addWidget(gameWidget);
    splitter->addWidget(chartsLabel);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->addWidget(splitter);

    connect(sensorReader, &SensorReader::dataReceived, gameWidget, &GameWidget::onSensorData);
    sensorReader->open("/dev/ttyACM1");
}

MainWindow::~MainWindow()
{
    delete ui;
}
