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

    gameLogic = new GameLogic(10, 10, this);
    gameWidget = new GameWidget(gameLogic, this);
    sensorReader = new SensorReader(this);
    chartPanel = new ChartPanel(this);
    gameWidget3D = new GameWidget3D(gameLogic, this);
    gameWidget3D->hide();

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(gameWidget);
    splitter->addWidget(chartPanel);
    //splitter->addWidget(gameWidget3D);  // dodaj do splittera ale ukryty


    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->addWidget(splitter);

    QToolBar *toolBar = addToolBar("Narzędzia");
    portCombo = new QComboBox();
    for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts()){
        portCombo->addItem(info.portName());
    }

    connectButton = new QPushButton("Połącz");
    newMazeButton = new QPushButton("Generuj labirynt");
    switchViewButton = new QPushButton("3D");

    toolBar->addWidget(switchViewButton);
    toolBar->addWidget(portCombo);
    toolBar->addWidget(connectButton);
    toolBar->addWidget(newMazeButton);
    toolBar->setMovable(false);
    toolBar->setStyleSheet(
        "QToolBar {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #3a3a3a, stop:1 #2a2a2a);"
        "   border-bottom: 1px solid #1a1a1a;"
        "   padding: 6px;"
        "   spacing: 12px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #5a5a5a, stop:1 #404040);"
        "   color: #e0e0e0;"
        "   border: 1px solid #666;"
        "   border-radius: 4px;"
        "   padding: 6px 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #6a6a6a, stop:1 #505050);"
        "}"
        "QPushButton:pressed {"
        "   background: #333;"
        "}"
        "QComboBox {"
        "   background: #404040;"
        "   color: #e0e0e0;"
        "   border: 1px solid #666;"
        "   border-radius: 4px;"
        "   padding: 4px 8px;"
        "   min-width: 120px;"
        "}"
        "QLabel {"
        "   color: #c0c0c0;"
        "   font-weight: bold;"
        "}"
        );

    // Dane z czujnika → logika gry (sterowanie) + wykresy
    connect(sensorReader, &SensorReader::dataReceived,
            this, [this](float ax, float ay, float az, float gx, float gy, float gz) {
                gameLogic->setSensorAcceleration(ax, ay);
            });
    connect(sensorReader, &SensorReader::dataReceived, chartPanel, &ChartPanel::onSensorData);
    connect(switchViewButton, &QPushButton::clicked, this, &MainWindow::onSwitchView);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(newMazeButton, &QPushButton::clicked, gameLogic, &GameLogic::resetMaze);
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
void MainWindow::onSwitchView() {
    if(gameWidget->isVisible()) {
        splitter->replaceWidget(0, gameWidget3D);
        gameWidget->hide();
        gameWidget3D->show();
        gameWidget3D->setFocus();
        switchViewButton->setText("2D");
    } else {
        splitter->replaceWidget(0, gameWidget);
        gameWidget3D->hide();
        gameWidget->show();
        gameWidget->setFocus();
        switchViewButton->setText("3D");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}