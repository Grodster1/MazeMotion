#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QHBoxLayout>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    resize(1600, 800);
    gameWidget = new GameWidget(10, 10, this);
    sensorReader = new SensorReader(this);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    chartPanel = new ChartPanel(this);

    splitter->addWidget(gameWidget);
    splitter->addWidget(chartPanel);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->addWidget(splitter);

    QToolBar *toolBar = addToolBar("Narzędzia");
    portCombo = new QComboBox();
    for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts()){
        portCombo->addItem(info.portName());
    }

    connectButton = new QPushButton("Połącz");
    newMazeButton = new QPushButton("Generuj labirynt");

    toolBar->addWidget(portCombo);
    toolBar->addWidget(connectButton);
    toolBar->addWidget(newMazeButton);

    connect(sensorReader, &SensorReader::dataReceived, gameWidget, &GameWidget::onSensorData);
    connect(sensorReader, &SensorReader::dataReceived, chartPanel, &ChartPanel::onSensorData);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(newMazeButton, &QPushButton::clicked, gameWidget, &GameWidget::resetMaze);
    sensorReader->open("/dev/ttyACM0");

}


void MainWindow::onConnectClicked() {
    if(!connected) {
        sensorReader->open(portCombo->currentText());
        connectButton->setText("Rozłącz");
        connected = true;
    } else {
        sensorReader->close();
        connectButton->setText("Połącz");
        connected = false;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
