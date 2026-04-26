#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    resize(1600, 800);
    //ui->setupUi(this);
    gameWidget = new GameWidget(10, 10, this);
    sensorReader = new SensorReader(this);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    //QLabel *chartsLabel = new QLabel("Charts");
    chartPanel = new ChartPanel(this);
    //chartsLabel->setAlignment(Qt::AlignCenter);

    splitter->addWidget(gameWidget);
    splitter->addWidget(chartPanel);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->addWidget(splitter);

    connect(sensorReader, &SensorReader::dataReceived, gameWidget, &GameWidget::onSensorData);
    connect(sensorReader, &SensorReader::dataReceived, chartPanel, &ChartPanel::onSensorData);
    sensorReader->open("/dev/ttyACM0");


}

MainWindow::~MainWindow()
{
    delete ui;
}
